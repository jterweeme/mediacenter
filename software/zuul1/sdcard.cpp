#define SYSTEM_BUS_WIDTH 32

#include <errno.h>
#include <priv/alt_file.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include "sdcard.h"

bool is_sd_card_formated_as_FAT16 = false;
volatile short int *aux_status_register = NULL;
volatile int *status_register = NULL;
volatile short int *CSD_register_w0 = NULL;
volatile short int *command_register = NULL;
volatile int *command_argument_register = NULL;
volatile char *buffer_memory = NULL;
int fat_partition_offset_in_512_byte_sectors = 0;
int fat_partition_size_in_512_byte_sectors = 0;
t_FAT_12_16_boot_sector boot_sector_data;
alt_up_sd_card_dev	*device_pointer = NULL;
bool current_sector_modified = false;
unsigned int current_sector_index = 0;
t_find_data search_data;

bool SDCard::Write_Sector_Data(int sector_index, int partition_offset)
{
    bool result = false;
    short int reg_state = 0xff;
    IOWR_32DIRECT(command_argument_register, 0, (sector_index + partition_offset)*512);
    IOWR_16DIRECT(command_register, 0, CMD_WRITE_BLOCK);

    do {
        reg_state = (short int) IORD_16DIRECT(aux_status_register,0);
    } while ((reg_state & 0x04)!=0);

    if ((reg_state & 0x10) == 0)
    {
        result = true;
        current_sector_modified = false;
        current_sector_index = sector_index+partition_offset;
    }
    return result;
}


bool SDCard::Save_Modified_Sector()
{
    bool result = true;

    if (current_sector_modified)
        result = Write_Sector_Data(current_sector_index, 0);

    return result;
}


bool SDCard::Read_Sector_Data(int sector_index, int partition_offset)
{
    bool result = false;
    short int reg_state = 0xff;
        
    if (current_sector_modified)
        if (!Write_Sector_Data(current_sector_index, 0))
            return false;

    IOWR_32DIRECT(command_argument_register, 0, (sector_index + partition_offset)*512);
    IOWR_16DIRECT(command_register, 0, CMD_READ_BLOCK);

    do {
        reg_state = (short int) IORD_16DIRECT(aux_status_register,0);
    } while ((reg_state & 0x04)!=0);

    if ((reg_state & 0x10) == 0)
    {
        result = true;
        current_sector_modified = false;
        current_sector_index = sector_index+partition_offset;
    }

	return result;
}


bool SDCard::get_cluster_flag(unsigned int cluster_index, unsigned short int *flag)
{
    unsigned int sector_index = (cluster_index / 256) + fat_partition_offset_in_512_byte_sectors;
    sector_index  = sector_index + boot_sector_data.first_fat_sector_offset;
     
    if (sector_index != current_sector_index)
        if (Read_Sector_Data(sector_index, 0) == false)
            return false;

    *flag = (unsigned short int)IORD_16DIRECT(device_pointer->base, 2*(cluster_index % 256));
    return true;
}


bool SDCard::mark_cluster(unsigned int cluster_index, short int flag, bool first_fat)
{
    unsigned int sector_index = (cluster_index / 256) +  fat_partition_offset_in_512_byte_sectors;
    
    if (first_fat)
        sector_index  = sector_index + boot_sector_data.first_fat_sector_offset;
    else
        sector_index  = sector_index + boot_sector_data.second_fat_sector_offset;
     
    if (sector_index != current_sector_index)
        if (Read_Sector_Data(sector_index, 0) == false)
            return false;

    IOWR_16DIRECT(device_pointer->base, 2*(cluster_index % 256), flag);
    current_sector_modified = true;
    return true;
}


bool SDCard::Check_for_Master_Boot_Record(void)
{
	bool result = false;
	int index;
	int end, offset, partition_size;

	if (Read_Sector_Data(0, 0))
	{
        end = (short int)IORD_16DIRECT(device_pointer->base,0x1fe);

		if ((end & 0x0000ffff) == 0x0000aa55)
		{
			for (index = 0; index < 4; index++)
			{
				int partition_data_offset = (index * 16) + 0x01be;
				char type;
				type = (unsigned char) IORD_8DIRECT(device_pointer->base,partition_data_offset + 0x04);

				if ((type == 1) || (type == 4) || (type == 6) || (type == 14))
				{
					offset = (((unsigned short int) IORD_16DIRECT(device_pointer->base,partition_data_offset + 0x0A)) << 16) | ((unsigned short int) IORD_16DIRECT(device_pointer->base,partition_data_offset + 0x08));
					partition_size = (((unsigned short int) IORD_16DIRECT(device_pointer->base,partition_data_offset + 0x0E)) << 16) | ((unsigned short int) IORD_16DIRECT(device_pointer->base,partition_data_offset + 0x0C));
		            
					if (partition_size > 0)
					{
						result = true;
						fat_partition_size_in_512_byte_sectors = partition_size;
						fat_partition_offset_in_512_byte_sectors = offset;
						break;
					}                
				}
			}
		}
	}

	return result;
}


bool SDCard::Read_File_Record_At_Offset(int offset, t_file_record *record,
        unsigned int cluster_index, unsigned int sector_in_cluster)
{
	bool result = false;

    if ((offset & 0x01f) == 0)
	{
		int counter;

		for (counter = 0; counter < 8; counter++)
			record->name[counter] = (char) IORD_8DIRECT(device_pointer->base, offset+counter);

		for (counter = 0; counter < 3; counter++)
			record->extension[counter] = (char) IORD_8DIRECT(device_pointer->base, offset+counter+8);

		record->attributes          =   (char) IORD_8DIRECT(device_pointer->base, offset+11);
		record->create_time = (unsigned short int)IORD_16DIRECT(device_pointer->base, offset+14);
		record->create_date = (unsigned short int) IORD_16DIRECT(device_pointer->base, offset+16);
		record->last_access_date = (unsigned short int) IORD_16DIRECT(device_pointer->base, offset+18);
		record->last_modified_time = (unsigned short int)IORD_16DIRECT(device_pointer->base, offset+22);
		record->last_modified_date = (unsigned short int)IORD_16DIRECT(device_pointer->base, offset+24);
		record->start_cluster_index =(unsigned short int)IORD_16DIRECT(device_pointer->base, offset+26);
		record->file_size_in_bytes  =(unsigned int) IORD_32DIRECT(device_pointer->base, offset+28);
		record->file_record_cluster = cluster_index;
		record->file_record_sector_in_cluster = sector_in_cluster;
		record->file_record_offset = offset;
		result = true;
	}
	return result;
}


bool SDCard::Write_File_Record_At_Offset(int offset, t_file_record *record)
{
    bool result = false;

    if ((offset & 0x01f) == 0)
    {
        int counter;

        for (counter = 0; counter < 8; counter=counter+2)
        {
            short int two_chars = (short int) record->name[counter+1];
            two_chars = two_chars << 8;
            two_chars = two_chars | record->name[counter];
            IOWR_16DIRECT(device_pointer->base, offset+counter, two_chars);
        }

        for (counter = 0; counter < 3; counter++)
            IOWR_8DIRECT(device_pointer->base, offset+counter+8, record->extension[counter]);

        IOWR_8DIRECT(device_pointer->base, offset+11, record->attributes);
        IOWR_16DIRECT(device_pointer->base, offset+14, record->create_time);
        IOWR_16DIRECT(device_pointer->base, offset+16, record->create_date);
        IOWR_16DIRECT(device_pointer->base, offset+18, record->last_access_date);
        IOWR_16DIRECT(device_pointer->base, offset+22, record->last_modified_time);
        IOWR_16DIRECT(device_pointer->base, offset+24, record->last_modified_date);
        IOWR_16DIRECT(device_pointer->base, offset+26, record->start_cluster_index);
        IOWR_32DIRECT(device_pointer->base, offset+28, record->file_size_in_bytes);
        current_sector_modified = true;                  
        result = true;
    }
    return result;
}


bool SDCard::Check_for_DOS_FAT(int FAT_partition_start_sector)
{
	bool result = false;
	int counter = 0;
	short int end;
	result = Read_Sector_Data(0, FAT_partition_start_sector);
	end =  (short int) IORD_16DIRECT(device_pointer->base, 0x1fe);

	if (((end & 0x0000ffff) == 0x0000aa55) && (result))
	{
		int num_clusters = 0;
		boot_sector_data.jump_instruction[0] = (char) IORD_8DIRECT(device_pointer->base, 0);
		boot_sector_data.jump_instruction[1] = (char) IORD_8DIRECT(device_pointer->base, 1);
		boot_sector_data.jump_instruction[2] = (char) IORD_8DIRECT(device_pointer->base, 2);

		for (counter = 0; counter < 8; counter++)
		{
			boot_sector_data.OEM_name[counter] = (char) IORD_8DIRECT(device_pointer->base, 3+counter);
		}
		boot_sector_data.sector_size_in_bytes = (((unsigned char)IORD_8DIRECT(device_pointer->base, 12)) << 8 ) | ((char) IORD_8DIRECT(device_pointer->base, 11));
		boot_sector_data.sectors_per_cluster = ((unsigned char) IORD_8DIRECT(device_pointer->base, 13));
		boot_sector_data.reserved_sectors = ((unsigned short int) IORD_16DIRECT(device_pointer->base, 14));
		boot_sector_data.number_of_FATs = ((unsigned char) IORD_8DIRECT(device_pointer->base, 16));
		boot_sector_data.max_number_of_dir_entires = (((unsigned short int)(((unsigned char) IORD_8DIRECT(device_pointer->base, 18)))) << 8 ) | ((unsigned char) IORD_8DIRECT(device_pointer->base, 17));
		boot_sector_data.number_of_sectors_in_partition = (((unsigned short int)(((unsigned char) IORD_8DIRECT(device_pointer->base, 20)))) << 8 ) | ((unsigned char) IORD_8DIRECT(device_pointer->base, 19));
		boot_sector_data.media_descriptor = ((unsigned char) IORD_8DIRECT(device_pointer->base, 21));
		boot_sector_data.number_of_sectors_per_table = ((unsigned short int) IORD_16DIRECT(device_pointer->base, 22));
		boot_sector_data.number_of_sectors_per_track = ((unsigned short int) IORD_16DIRECT(device_pointer->base, 24));
		boot_sector_data.number_of_heads = ((unsigned short int) IORD_16DIRECT(device_pointer->base, 26));
		boot_sector_data.number_of_hidden_sectors = ((unsigned int) IORD_32DIRECT(device_pointer->base, 28));
		boot_sector_data.total_sector_count_if_above_32MB = ((unsigned int) IORD_32DIRECT(device_pointer->base, 32));
		boot_sector_data.drive_number = ((unsigned char) IORD_8DIRECT(device_pointer->base, 36));
		boot_sector_data.current_head = ((unsigned char) IORD_8DIRECT(device_pointer->base, 37));
		boot_sector_data.boot_signature = ((unsigned char) IORD_8DIRECT(device_pointer->base, 38));
		boot_sector_data.first_fat_sector_offset = boot_sector_data.reserved_sectors;
		boot_sector_data.second_fat_sector_offset = boot_sector_data.first_fat_sector_offset + boot_sector_data.number_of_sectors_per_table;
		boot_sector_data.root_directory_sector_offset = boot_sector_data.second_fat_sector_offset + boot_sector_data.number_of_sectors_per_table; 
		boot_sector_data.data_sector_offset = boot_sector_data.root_directory_sector_offset + (32*boot_sector_data.max_number_of_dir_entires / boot_sector_data.sector_size_in_bytes);    
	    
		if (boot_sector_data.number_of_sectors_in_partition > 0)
			num_clusters = (boot_sector_data.number_of_sectors_in_partition / boot_sector_data.sectors_per_cluster);
		else
			num_clusters = (boot_sector_data.total_sector_count_if_above_32MB / boot_sector_data.sectors_per_cluster);

		if (num_clusters < 4087)
			boot_sector_data.bits_for_cluster_index = 12;
		else if (num_clusters <= 65517)
			boot_sector_data.bits_for_cluster_index = 16;
		else
			boot_sector_data.bits_for_cluster_index = 32;
	    
		for (counter = 0; counter < 4; counter++)
			boot_sector_data.volume_id[counter] = ((char) IORD_8DIRECT(device_pointer->base, 39+counter));

		for (counter = 0; counter < 11; counter++)
			boot_sector_data.volume_label[counter] = ((char) IORD_8DIRECT(device_pointer->base, 43+counter));

		for (counter = 0; counter < 8; counter++)
            boot_sector_data.file_system_type[counter] = ((char)IORD_8DIRECT(device_pointer->base, 54+counter));

		for (counter = 0; counter < MAX_FILES_OPENED; counter++)
			active_files[counter].in_use = false;

		result = true;
	}
    else
    {
        result = false;
    }
	return result;
}


bool SDCard::Look_for_FAT16()
{
    bool result = false;
    short int csd_file_format = *CSD_register_w0;
    fat_partition_offset_in_512_byte_sectors = 0;
    fat_partition_size_in_512_byte_sectors = 0;              

    if (((csd_file_format & 0x8000) == 0) && ((csd_file_format & 0x0c00) != 0x0c00))
    {
        if ((csd_file_format & 0x0c00) == 0x0400)
            result = Check_for_DOS_FAT(0);

        if ((csd_file_format & 0x0c00) == 0x0000)
            if (Check_for_Master_Boot_Record())
                result = Check_for_DOS_FAT(fat_partition_offset_in_512_byte_sectors);

        if (result == true)
        {
            if (boot_sector_data.bits_for_cluster_index != 16)
                result = false;
            else
                fat_partition_size_in_512_byte_sectors = boot_sector_data.number_of_sectors_in_partition;
        }
    }
	return result;
}
 

void SDCard::filename_to_upper_case(char *file_name)
{
    int length = strlen(file_name);
    
    for (int index = 0; index < length; index++)
        if ((file_name[index] >= 'a') && (file_name[index] <= 'z'))
            file_name[index] = (file_name[index] - 'a') + 'A';
}


bool SDCard::check_file_name_for_FAT16_compliance(char *file_name)
{
    int length = strlen(file_name);
    int index;
    int last_dir_break_position = -1;
    int last_period = -1;
    bool result = true;
    
    for(index = 0; index < length; index++)
    {
        if ((file_name[index] == ' ') ||
            ((last_dir_break_position == (index - 1)) && ((file_name[index] == '\\') || (file_name[index] == '/'))) ||
            ((index - last_period == 9) && (file_name[index] != '.')) ||
            ((last_dir_break_position != last_period) && (index - last_period > 3) &&
             (file_name[index] != '\\') && (file_name[index] != '/'))
           )
        {
            result = false;
            break;
        }
        if ((file_name[index] == '\\') || (file_name[index] == '/'))
        {
            last_period = index;
            last_dir_break_position = index;
        }

        if (file_name[index] == '.')
            last_period = index;
    }

    if ((file_name[length-1] == '\\') || (file_name[length-1] == '/'))
        result = false;

    return result;
}


int SDCard::get_dir_divider_location(char *name)
{
    int index = 0;
    int length = strlen(name);
    
    for(index = 0; index < length; index++)
        if ((name[index] == '\\') || (name[index] == '/'))
            break;
    
    if (index == length)
        index = -1;
    
    return index;
}


bool SDCard::match_file_record_to_name_ext(t_file_record *file_record, char *name, char *extension)
{
    bool match = true;
	int index;

    for (index = 0; index < 8; index++)
    {
        if (CHAR_TO_UPPER(file_record->name[index]) != CHAR_TO_UPPER(name[index]))
        {
            match = false;
			break;
        }
    }
    for (index = 0; index < 3; index++)
    {
        if (CHAR_TO_UPPER(file_record->extension[index]) != CHAR_TO_UPPER(extension[index]))
        {
            match = false;
			break;
        }
    }
	return match;
}


bool SDCard::get_home_directory_cluster_for_file(char *file_name,
        int *home_directory_cluster, t_file_record *file_record)
{
    bool result = false;
    int home_dir_cluster = 0;
    int location, index;
    int start_location = 0;
    location = get_dir_divider_location( file_name );

    while (location > 0)
    {
        char name[8] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
        char extension[3] = { ' ', ' ', ' ' };
        int ext_index = -1;
        int new_cluster = home_dir_cluster;
        
        for (index = 0; index < location; index++)
        {
            if (file_name[index+start_location] == '.')
                ext_index = index;
            else if (ext_index < 0)
                name[index] = file_name[index+start_location];
            else
                extension[index-ext_index] = file_name[index+start_location];
        }
        
        if (home_dir_cluster == 0)
        {
            int max_root_dir_sectors = ((32*boot_sector_data.max_number_of_dir_entires) / boot_sector_data.sector_size_in_bytes);
            int sector_index;
            
            for (sector_index = 0; sector_index < max_root_dir_sectors; sector_index++)
            {
                if (Read_Sector_Data(sector_index + boot_sector_data.root_directory_sector_offset,
                        fat_partition_offset_in_512_byte_sectors))
                {
                    int file_counter;
                    
                    for (file_counter = 0; file_counter < 16; file_counter++)
                    {
                       
                        Read_File_Record_At_Offset(file_counter*32, file_record, 0, sector_index);
                        if ((file_record->name[0] != 0xe5) && (file_record->name[0] != 0x00))
                        {
                            bool match = match_file_record_to_name_ext(file_record, name, extension);
                            if (match)
                            {
                                new_cluster = file_record->start_cluster_index;
                                file_record->file_record_cluster = 1;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    break;
                }
                if (new_cluster != home_dir_cluster)
                {
                    break;
                }
            }
            if (new_cluster != home_dir_cluster)
            {
                home_dir_cluster = new_cluster;
                start_location = start_location+location+1;
            }
            else
            {
                return false;
            }
        }
        else
        {
            int cluster = home_dir_cluster;
            
            do
            {
                int start_sector = (cluster - 2 ) * (boot_sector_data.sectors_per_cluster) +
                        boot_sector_data.data_sector_offset;

                int sector_index;
                
                for (sector_index = 0; sector_index < boot_sector_data.sectors_per_cluster; sector_index++)
                {
                    if (Read_Sector_Data(sector_index + start_sector, fat_partition_offset_in_512_byte_sectors))
                    {
                        int file_counter;
                        
                        for (file_counter = 0; file_counter < 16; file_counter++)
                        {                         
                            Read_File_Record_At_Offset(file_counter*32, file_record, cluster, sector_index);
                            if ((file_record->name[0] != 0xe5) && (file_record->name[0] != 0x00))
                            {
								bool match = match_file_record_to_name_ext(file_record, name, extension);
                                if (match)
                                {
                                    new_cluster = file_record->start_cluster_index;
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        break;
                    }

                    if (new_cluster != home_dir_cluster)
                        break;
                }
                if (new_cluster == home_dir_cluster)
                {
					unsigned short int next_cluster;

					if (get_cluster_flag(new_cluster, &next_cluster))
					{
						if ((next_cluster & 0x0000fff8) == 0x0000fff8)
							return false;

						new_cluster = (next_cluster & 0x0000fff8);
					}
					else
					{
						return false;
					}
                }              
            } while ((cluster < 0x0000fff8) && (new_cluster == home_dir_cluster));
            if (new_cluster != home_dir_cluster)
            {
                home_dir_cluster = new_cluster;
                start_location = start_location+location+1;
            }
            else
            {
                return false;
            }            
        }
        location = get_dir_divider_location(&(file_name[start_location]));

        if (location < 0)
            result = true;
    }
    
    *home_directory_cluster = home_dir_cluster;
    if (home_dir_cluster == 0)
    {
        file_record->file_record_cluster = 0;
		result = true;
    }
    return result;
}


bool SDCard::find_file_in_directory(int directory_start_cluster,
            char *file_name, t_file_record *file_record)
{
    Uart *uart = Uart::getInstance();
    uart->puts("find-file-in-directory\r\n");
    int location = get_dir_divider_location( file_name );
    int last_dir_separator = 0;
    char name[8] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
    char extension[3] = { ' ', ' ', ' ' };
    int ext_index = -1;
    int cluster = directory_start_cluster;
    int index;
	int length = strlen(file_name);
    bool result = false;
    
    while (location > 0)
    {
        last_dir_separator = last_dir_separator+location+1;
        location = get_dir_divider_location( &(file_name[last_dir_separator]) );
    }
        
    for (index = last_dir_separator; index < length; index++)
    {
        if (file_name[index] == '.')
        {
            ext_index = index;
        }
        else if (ext_index < 0)
        {
            name[index-last_dir_separator] = file_name[index];
        }
        else
        {
            extension[index-ext_index-1] = file_name[index];
        }
    }

    if (directory_start_cluster == 0)
    {
        int max_root_dir_sectors = ((32*boot_sector_data.max_number_of_dir_entires) / boot_sector_data.sector_size_in_bytes);
        int sector_index;
        
        for (sector_index = 0; sector_index < max_root_dir_sectors; sector_index++)
        {
            if (Read_Sector_Data(   sector_index + boot_sector_data.root_directory_sector_offset,
                                    fat_partition_offset_in_512_byte_sectors))
            {
                int file_counter;
                
                for (file_counter = 0; file_counter < 16; file_counter++)
                {
                    Read_File_Record_At_Offset(file_counter*32, file_record, 0, sector_index);
                    if ((file_record->name[0] != 0xe5) && (file_record->name[0] != 0x00))
                    {
                        bool match = match_file_record_to_name_ext(file_record, name, extension);

                        if (match)
                        {
                            result = true;
                            break;
                        }
                    }
                }
            }
            else
            {
                break;
            }
            if (result)
            {
                break;
            }
        }
    }
    else
    {          
        do
        {
            int start_sector = (cluster - 2 ) * (boot_sector_data.sectors_per_cluster) +
                    boot_sector_data.data_sector_offset;

            int sector_index;
            
            for (sector_index = 0; sector_index < boot_sector_data.sectors_per_cluster; sector_index++)
            {
                if (Read_Sector_Data(sector_index + start_sector, fat_partition_offset_in_512_byte_sectors))
                {
                    for (int file_counter = 0; file_counter < 16; file_counter++)
                    {
                        Read_File_Record_At_Offset(file_counter*32, file_record, cluster, sector_index);

                        if ((file_record->name[0] != 0xe5) && (file_record->name[0] != 0x00))
                        {
                            bool match = match_file_record_to_name_ext(file_record, name, extension);

                            if (match)
                            {                               
                                result = true;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    break;
                }
                if (result)
                {
                    break;
                }
            }
            if (result == false)
            {
				unsigned short int new_cluster;

				if (get_cluster_flag(cluster, &new_cluster))
				{
					if ((new_cluster & 0x0000fff8) == 0x0000fff8)
						return false;

					cluster = (new_cluster & 0x0000fff8);
				}
				else
                {
                    return false;
                }
            }              
        }
        while ((cluster < 0x0000fff8) && (result == false));
    }
    
    return result;   
}


bool SDCard::find_first_empty_cluster(unsigned int *cluster_number)
{
    unsigned int sector = boot_sector_data.first_fat_sector_offset;
    unsigned int cluster_index = 2;
    short int cluster = -1;
    bool result = false;
	unsigned max_cluster_index = 0;
	unsigned int non_data_sectors = boot_sector_data.data_sector_offset;
	unsigned int less_than_32 = boot_sector_data.number_of_sectors_in_partition;
	unsigned int greater_than_32 = boot_sector_data.total_sector_count_if_above_32MB;

	if (less_than_32 > greater_than_32)
		max_cluster_index = ((less_than_32 - non_data_sectors) / boot_sector_data.sectors_per_cluster) + 1;
	else
		max_cluster_index = ((greater_than_32 - non_data_sectors) / boot_sector_data.sectors_per_cluster) + 1;

    while (sector != boot_sector_data.second_fat_sector_offset)
    {
        if (Read_Sector_Data( sector, fat_partition_offset_in_512_byte_sectors))
        {
            do {
                cluster = ((unsigned short int) IORD_16DIRECT(device_pointer->base, 2*(cluster_index % 256)));
                if (cluster == 0)
                {
                    break;
                }
                else
                {
                    cluster_index++;
                } 
            } while ((cluster_index % 256) != 0);
        }
        if (cluster == 0)
        {
            break;
        }
        sector++;
    }
    if ((cluster == 0) && (cluster <= max_cluster_index))
    {
        *cluster_number = cluster_index;
		result = true;
    }
    return result;
}


int SDCard::find_first_empty_record_in_a_subdirectory(int start_cluster_index)
{
    int result = -1;
    int cluster = start_cluster_index;
    do {
        int start_sector = ( cluster - 2 ) * ( boot_sector_data.sectors_per_cluster ) + boot_sector_data.data_sector_offset;
        int sector_index;
        
        for (sector_index = 0; sector_index < boot_sector_data.sectors_per_cluster; sector_index++)
        {
            if (Read_Sector_Data(sector_index + start_sector, fat_partition_offset_in_512_byte_sectors))
            {
                int file_counter;
                
                for (file_counter = 0; file_counter < 16; file_counter++)
                {
                    unsigned short int leading_char;                    
                    leading_char = ((unsigned char) IORD_8DIRECT(device_pointer->base, file_counter*32));

                    if ((leading_char == 0x00e5) || (leading_char == 0))
                    {
                        result = (cluster) | ((sector_index*16 + file_counter) << 16);
                        return result;
                    }
                }
            }
            else
            {
                break;
            }
        }
        if (result < 0)
        {
			unsigned short int new_cluster;
			if (get_cluster_flag(cluster, &new_cluster))
			{
				if ((new_cluster & 0x0000fff8) == 0x0000fff8)
				{
					int new_dir_cluster; 
					if (find_first_empty_cluster((unsigned int *)&new_dir_cluster))
					{
						if (mark_cluster(cluster, ((short int) (new_dir_cluster)), true) &&
							mark_cluster(new_dir_cluster, ((short int) (0xffff)), true) &&
							mark_cluster(cluster, ((short int) (new_dir_cluster)), false) &&
							mark_cluster(new_dir_cluster, ((short int) (0xffff)), false))
						{
							Save_Modified_Sector();
							result = new_dir_cluster;                           
						}
					}
					cluster = (new_cluster & 0x0000fff8);
				}
			}
			else
			{
				result = -1;
			}
        }              
    } while ((cluster < 0x0000fff8) && (result == -1)); 
    return result; 
}


int SDCard::find_first_empty_record_in_root_directory()
{
    int max_root_dir_sectors = ((32*boot_sector_data.max_number_of_dir_entires) / boot_sector_data.sector_size_in_bytes);
    int sector_index;
    int result = -1;
    
    for (sector_index = 0; sector_index < max_root_dir_sectors; sector_index++)
    {
        if (Read_Sector_Data(sector_index + boot_sector_data.root_directory_sector_offset,
                                fat_partition_offset_in_512_byte_sectors))
        {
            int file_counter;
            
            for (file_counter = 0; file_counter < 16; file_counter++)
            {
                unsigned short int leading_char;
                leading_char = ((unsigned char) IORD_8DIRECT(device_pointer->base, file_counter*32));
                if ((leading_char == 0x00e5) || (leading_char == 0))
                {
                    result = (sector_index*16 + file_counter) << 16;
                    return result;
                }
            }
        }
        else
        {
            break;
        }
    }
    return result;
}

void SDCard::convert_filename_to_name_extension(char *filename, char *name, char *extension)
{
    int counter;
    int local = 0;
    
    for(counter = 0; counter < 8; counter++)
    {
        if (filename[local] != '.')
        {
            name[counter] = filename[local];
            if (filename[local] != 0) local++;
        }
        else
        {
            name[counter] = ' ';
        }
    }
    if (filename[local] == '.') local++;
    for(counter = 0; counter < 3; counter++)
    {
        if (filename[local] != 0)
        {
            extension[counter] = filename[local];
            local++;
        }
        else
        {
            extension[counter] = ' ';
        }
    }

}

bool SDCard::create_file(char *name, t_file_record *file_record, t_file_record *home_dir)
{
    unsigned int cluster_number;
    bool result = false;
    
    if (find_first_empty_cluster(&cluster_number))
    {
        int record_index;
        
        if (home_dir->file_record_cluster == 0)
            record_index = find_first_empty_record_in_root_directory();
        else
            record_index = find_first_empty_record_in_a_subdirectory(home_dir->start_cluster_index);           

        if (record_index >= 0)
        {   
            unsigned int file_record_sector;
            int location = get_dir_divider_location( name );
            int last_dir_separator = 0;

            while (location > 0)
            {
                last_dir_separator = last_dir_separator+location+1;
                location = get_dir_divider_location( &(name[last_dir_separator]) );
            }
            
            convert_filename_to_name_extension(&(name[last_dir_separator]), (char *)file_record->name, (char *)file_record->extension);
                         
            file_record->attributes = 0;
            file_record->create_time = 0;
            file_record->create_date = 0;
            file_record->last_access_date = 0;
            file_record->last_modified_time = 0;
            file_record->last_modified_date = 0;
            file_record->start_cluster_index = cluster_number;
            file_record->file_size_in_bytes = 0;
            file_record->current_cluster_index = cluster_number;
            file_record->current_sector_in_cluster = 0;
            file_record->current_byte_position = 0;
            file_record->file_record_cluster = record_index & 0x0000ffff;
            file_record->file_record_sector_in_cluster = ((record_index >> 16) & 0x0000ffff) / 16;
            file_record->file_record_offset = (((record_index >> 16) & 0x0000ffff) % 16)*32;   
            file_record->home_directory_cluster = home_dir->start_cluster_index;
            file_record->in_use = true;
            file_record->modified = true;
            file_record_sector = (file_record->file_record_cluster == 0) ? 
                                    (boot_sector_data.root_directory_sector_offset + file_record->file_record_sector_in_cluster):  
                                    (boot_sector_data.data_sector_offset + (file_record->file_record_cluster-2)*boot_sector_data.sectors_per_cluster +
                                     file_record->file_record_sector_in_cluster);

			if (Read_Sector_Data(file_record_sector, fat_partition_offset_in_512_byte_sectors))
            {
                if (Write_File_Record_At_Offset(file_record->file_record_offset, file_record))
                {
                    Save_Modified_Sector();
                    mark_cluster(cluster_number, ((short int) (0xffff)), true);

                    if (mark_cluster(cluster_number, ((short int) (0xffff)), false))
                        result = true;
                }
            }
        }

    }
    return result;           
}


void SDCard::copy_file_record_name_to_string(t_file_record *file_record, char *file_name)
{
	int index;
	int flength = 0;

	for (index = 0; index < 8; index++)
	{
		if (file_record->name[index] != ' ')
		{
			file_name[flength] = file_record->name[index];
			flength = flength + 1;
		}
	}

	if (file_record->extension[0] != ' ')
	{
		file_name[flength] = '.';
		flength = flength + 1;
		for (index = 0; index < 3; index++)
		{
			if (file_record->extension[index] != ' ')
			{
				file_name[flength] = file_record->extension[index];
				flength = flength + 1;
			}
		}
	}
	file_name[flength] = 0;
}

alt_up_sd_card_dev* SDCard::alt_up_sd_card_open_dev(const char* name, volatile void *base)
{
    this->base = base;
	alt_up_sd_card_dev *dev = (alt_up_sd_card_dev *) alt_find_dev(name, &alt_dev_list);

	if (dev != NULL)
	{
		aux_status_register = ((short int *) SD_CARD_AUX_STATUS(dev->base));
		status_register = ((int *) SD_CARD_STATUS(dev->base));
		CSD_register_w0 = ((short int *) SD_CARD_CSD(dev->base, 0));
		command_register = ((short int *) SD_CARD_COMMAND(dev->base));
		command_argument_register = ((int *) SD_CARD_ARGUMENT(dev->base));
		buffer_memory = (char *) SD_CARD_BUFFER(dev->base, 0);
		device_pointer = dev;
		initialized = false;
		is_sd_card_formated_as_FAT16 = false;
		search_data.valid = false;
	}
	return dev;
}


bool SDCard::alt_up_sd_card_is_Present()
{
    bool result = false;

    if ((device_pointer != NULL) && ((IORD_16DIRECT(aux_status_register,0) & 0x02) != 0))
    {
        result = true;
    }
	else if (initialized == true)
	{
		initialized = false;
		search_data.valid = false;
		is_sd_card_formated_as_FAT16 = false;

		for (int index = 0; index < MAX_FILES_OPENED; index++)
		{
			active_files[index].in_use = false;
			active_files[index].modified = false;
		}
	}
    return result;
}


bool SDCard::alt_up_sd_card_is_FAT16(void)
{
	bool result = false;

	if (alt_up_sd_card_is_Present())
	{
		if (initialized == false)
		{
			is_sd_card_formated_as_FAT16 = Look_for_FAT16();
			initialized = is_sd_card_formated_as_FAT16;
			search_data.valid = false;
		}
		result = is_sd_card_formated_as_FAT16;
	}
	else
	{
		initialized = false;
		is_sd_card_formated_as_FAT16 = false;
	}

	return result;
}


short int SDCard::alt_up_sd_card_find_first(char *directory_to_search_through, char *file_name)
{
	short int result = 2;
    int home_directory_cluster;
    t_file_record file_record;

    if (get_home_directory_cluster_for_file(directory_to_search_through,
                &home_directory_cluster, &file_record))
    {
        search_data.directory_root_cluster = home_directory_cluster;
        search_data.current_cluster_index = home_directory_cluster;
		search_data.current_sector_in_cluster = 0;
		search_data.file_index_in_sector = -1;
		search_data.valid = true;
		result = alt_up_sd_card_find_next(file_name);
	}
    else
    {
        result = 1;
    }
	return result;
}

short int SDCard::alt_up_sd_card_find_next(char *file_name, t_file_record *fr)
{
    short int result = 2;

    if (search_data.valid)
    {
        t_file_record file_record;
        int cluster = search_data.current_cluster_index;

        if (cluster == 0)
        {
				int max_root_dir_sectors = ((32*boot_sector_data.max_number_of_dir_entires) /
                        boot_sector_data.sector_size_in_bytes);

				int sector_index = search_data.current_sector_in_cluster;
				int file_counter = search_data.file_index_in_sector+1;
    
				for (; sector_index < max_root_dir_sectors; sector_index++)
				{
					if (Read_Sector_Data(sector_index + boot_sector_data.root_directory_sector_offset,
											fat_partition_offset_in_512_byte_sectors))
					{
						for (; file_counter < 16; file_counter++)
						{
                            if (Read_File_Record_At_Offset(file_counter*32, &file_record,
                                    0, sector_index))
							{
								if ((file_record.name[0] != 0) && (file_record.name[0] != 0xe5))
								{
									search_data.file_index_in_sector = file_counter;
									search_data.current_sector_in_cluster = sector_index;
									copy_file_record_name_to_string(&file_record, file_name);
                                    ::memcpy(fr, &file_record, sizeof(file_record));
									return 0;
								}
							}
						}
						file_counter = 0;
					}
					else
					{
						break;
					}
				}
				result = -1;
			}
			else
			{
				int file_counter = search_data.file_index_in_sector+1;
				do 
				{
					int start_sector = ( cluster - 2 ) * ( boot_sector_data.sectors_per_cluster ) + boot_sector_data.data_sector_offset;
					int sector_index = search_data.current_sector_in_cluster;
			        
					for (; sector_index < boot_sector_data.sectors_per_cluster; sector_index++)
					{
						if (Read_Sector_Data(sector_index + start_sector, fat_partition_offset_in_512_byte_sectors))
						{        
							for (; file_counter < 16; file_counter++)
							{
								if (Read_File_Record_At_Offset(file_counter*32, &file_record, cluster, sector_index))
								{
									if ((file_record.name[0] != 0) && (file_record.name[0] != 0xe5))
									{
										search_data.current_cluster_index = cluster;
										search_data.file_index_in_sector = file_counter;
										search_data.current_sector_in_cluster = sector_index;
										copy_file_record_name_to_string(&file_record, file_name);
                                        ::memcpy(fr, &file_record, sizeof(file_record));
										return 0;
									}
								}
							}
							file_counter = 0;
						}
						else
						{
							break;
						}
					}
					if (sector_index >= boot_sector_data.sectors_per_cluster)
					{
						short int new_cluster;

						if (get_cluster_flag(cluster, (unsigned short *)&new_cluster))
						{
							if ((new_cluster & 0x0000fff8) == 0x0000fff8)
							{
								result = -1;
								search_data.valid = false;
							}
							cluster = ((new_cluster) & 0x0000fff8);
						}
						else
						{
							result = -1;
						}
                }              
            }
            while (cluster < 0x0000fff8);
        }
    }
    else
    {
        result = 3;
    }
    return result;
}

short int SDCard::alt_up_sd_card_find_next(char *file_name)
{
	short int result = 2;
	//if ((alt_up_sd_card_is_Present()) && (is_sd_card_formated_as_FAT16))
	{
		if (search_data.valid)
		{
			t_file_record file_record;
			int cluster = search_data.current_cluster_index;

			if (cluster == 0)
			{
				int max_root_dir_sectors = ((32*boot_sector_data.max_number_of_dir_entires) /
                        boot_sector_data.sector_size_in_bytes);

				int sector_index = search_data.current_sector_in_cluster;
				int file_counter = search_data.file_index_in_sector+1;
    
				for (; sector_index < max_root_dir_sectors; sector_index++)
				{
					if (Read_Sector_Data(sector_index + boot_sector_data.root_directory_sector_offset,
											fat_partition_offset_in_512_byte_sectors))
					{
						for (; file_counter < 16; file_counter++)
						{
                            if (Read_File_Record_At_Offset(file_counter*32, &file_record,
                                    0, sector_index))
							{
								if ((file_record.name[0] != 0) && (file_record.name[0] != 0xe5))
								{
									search_data.file_index_in_sector = file_counter;
									search_data.current_sector_in_cluster = sector_index;
									copy_file_record_name_to_string(&file_record, file_name);
									return 0;
								}
							}
						}
						file_counter = 0;
					}
					else
					{
						break;
					}
				}
				result = -1;
			}
			else
			{
				int file_counter = search_data.file_index_in_sector+1;
				do 
				{
					int start_sector = ( cluster - 2 ) * ( boot_sector_data.sectors_per_cluster ) + boot_sector_data.data_sector_offset;
					int sector_index = search_data.current_sector_in_cluster;
			        
					for (; sector_index < boot_sector_data.sectors_per_cluster; sector_index++)
					{
						if (Read_Sector_Data(sector_index + start_sector, fat_partition_offset_in_512_byte_sectors))
						{        
							for (; file_counter < 16; file_counter++)
							{
								if (Read_File_Record_At_Offset(file_counter*32, &file_record, cluster, sector_index))
								{
									if ((file_record.name[0] != 0) && (file_record.name[0] != 0xe5))
									{
										search_data.current_cluster_index = cluster;
										search_data.file_index_in_sector = file_counter;
										search_data.current_sector_in_cluster = sector_index;
										copy_file_record_name_to_string(&file_record, file_name);
										return 0;
									}
								}
							}
							file_counter = 0;
						}
						else
						{
							break;
						}
					}
					if (sector_index >= boot_sector_data.sectors_per_cluster)
					{
						short int new_cluster;

						if (get_cluster_flag(cluster, (unsigned short *)&new_cluster))
						{
							if ((new_cluster & 0x0000fff8) == 0x0000fff8)
							{
								result = -1;
								search_data.valid = false;
							}
							cluster = ((new_cluster) & 0x0000fff8);
						}
						else
						{
							result = -1;
						}
					}              
				} while (cluster < 0x0000fff8);
			}
		}
		else
		{
			result = 3;
		}
	}
	return result;
}

uint16_t SDCardEx::fopen(char *name, bool create)
{
    Uart *uart = Uart::getInstance();
    short int file_record_index = -1;
    unsigned int home_directory_cluster = 0;
    t_file_record home_dir;
    filename_to_upper_case(name);

    if (check_file_name_for_FAT16_compliance(name))
    {
        int index;

        if (!get_home_directory_cluster_for_file(name, (int *)&home_directory_cluster, &home_dir))
            return -1;
 
        for (index = 0; index < MAX_FILES_OPENED; index++)
        {
    		if (active_files[index].in_use == false)
    		{
    			file_record_index = index;
    			break;
    		}
    	}
    	if (file_record_index >= 0)
    	{
            if (this->find_file_in_directory(home_directory_cluster,
                    name, &(active_files[file_record_index])))
            {
                if (create)
                    return -1;

                active_files[file_record_index].current_cluster_index =
                            active_files[file_record_index].start_cluster_index;

                active_files[file_record_index].current_sector_in_cluster = 0;
                active_files[file_record_index].current_byte_position = 0;
                active_files[file_record_index].in_use = true;
                active_files[file_record_index].modified = false;

                for (index = 0; index < MAX_FILES_OPENED; index++)
                {
                    if ((file_record_index != index) && (active_files[index].in_use == true))
                    {
                        if ((active_files[file_record_index].file_record_cluster ==
                                active_files[index].file_record_cluster) &&
								(active_files[file_record_index].file_record_sector_in_cluster ==
                                active_files[index].file_record_sector_in_cluster) &&
								(active_files[file_record_index].file_record_offset ==
                                active_files[index].file_record_offset))
                        {
                            file_record_index = -2;
                            break;
                        }
                    }
                }
            }
            else if (create)
            {
                if (create_file(name, &(active_files[file_record_index]), &home_dir))
                {
                    active_files[file_record_index].in_use = true;
                    active_files[file_record_index].modified = true;
                }
                else
                {
                    file_record_index = -1;
                }                
            }
            else
            {
                file_record_index = -1;
            }
        }
    }
	return file_record_index;
}


void SDCard::alt_up_sd_card_set_attributes(short int file_handle, short int attributes)
{
    if ((file_handle >= 0) && (file_handle < MAX_FILES_OPENED))
        if (active_files[file_handle].in_use)
            active_files[file_handle].attributes = ((char)(attributes & 0x00ff));
}


short int SDCard::alt_up_sd_card_get_attributes(short int file_handle)
{
	short int result = -1;

    if ((file_handle >= 0) && (file_handle < MAX_FILES_OPENED))
        if (active_files[file_handle].in_use)
			result = ((active_files[file_handle].attributes) & 0x00ff);

	return result;
}

short int SDCard::alt_up_sd_card_read(short int file_handle)
{
    short int ch = -1;
    
    if ((file_handle >= 0) && (file_handle < MAX_FILES_OPENED))
    {
        if (active_files[file_handle].in_use)
        {
            if (active_files[file_handle].current_byte_position < active_files[file_handle].file_size_in_bytes)
            {
                int data_sector = boot_sector_data.data_sector_offset + (active_files[file_handle].current_cluster_index - 2)*boot_sector_data.sectors_per_cluster +
                                  active_files[file_handle].current_sector_in_cluster;
                
                if ((active_files[file_handle].current_byte_position > 0) && ((active_files[file_handle].current_byte_position % 512) == 0))
                {
                    if (active_files[file_handle].current_sector_in_cluster == boot_sector_data.sectors_per_cluster - 1)
                    {
                        unsigned short int next_cluster;
                        if (get_cluster_flag(active_files[file_handle].current_cluster_index, &next_cluster))
                        {
                            if ((next_cluster & 0x0000fff8) == 0x0000fff8)
                            {
                                return -1;
                            } 
                            else
                            {
                                active_files[file_handle].current_cluster_index = next_cluster;
								active_files[file_handle].current_sector_in_cluster = 0;
                                data_sector = boot_sector_data.data_sector_offset + (active_files[file_handle].current_cluster_index - 2)*boot_sector_data.sectors_per_cluster +
                                  active_files[file_handle].current_sector_in_cluster;                                
                            }
                        }
                        else
                        {
                            return -2;
                        }
                    }
                    else
                    {
                        active_files[file_handle].current_sector_in_cluster = active_files[file_handle].current_sector_in_cluster + 1;
                        data_sector = data_sector + 1;
                    }
                }
                if (current_sector_index != (data_sector + fat_partition_offset_in_512_byte_sectors))
                    if (!Read_Sector_Data(data_sector, fat_partition_offset_in_512_byte_sectors))
						return -2;

                ch = (unsigned char) IORD_8DIRECT(buffer_memory, (active_files[file_handle].current_byte_position % 512));
                active_files[file_handle].current_byte_position = active_files[file_handle].current_byte_position + 1;
            }
        }
    }
    
    return ch;
}


bool SDCard::alt_up_sd_card_write(short int file_handle, char byte_of_data)
{
    bool result = false;
    
    if ((file_handle >= 0) && (file_handle < MAX_FILES_OPENED))
    {
        if (active_files[file_handle].in_use)
        {
            int data_sector = boot_sector_data.data_sector_offset + (active_files[file_handle].current_cluster_index - 2)*boot_sector_data.sectors_per_cluster +
                              active_files[file_handle].current_sector_in_cluster;
			short int buffer_offset = active_files[file_handle].current_byte_position % boot_sector_data.sector_size_in_bytes;

			if (active_files[file_handle].current_byte_position < active_files[file_handle].file_size_in_bytes)
            {
                if ((active_files[file_handle].current_byte_position > 0) && (buffer_offset == 0))
                {
                    if (active_files[file_handle].current_sector_in_cluster == boot_sector_data.sectors_per_cluster - 1)
                    {
                        unsigned short int next_cluster;
                        if (get_cluster_flag(active_files[file_handle].current_cluster_index, &next_cluster))
                        {
                            if (next_cluster < 0x0000fff8)
                            {
                                active_files[file_handle].current_cluster_index = next_cluster;
								active_files[file_handle].current_sector_in_cluster = 0;
                                data_sector = boot_sector_data.data_sector_offset + (active_files[file_handle].current_cluster_index - 2)*boot_sector_data.sectors_per_cluster +
                                  active_files[file_handle].current_sector_in_cluster;                                
                            }
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        active_files[file_handle].current_sector_in_cluster = active_files[file_handle].current_sector_in_cluster + 1;
                        data_sector = data_sector + 1;
                    }
                }
            }
			else
			{
				if ((active_files[file_handle].current_byte_position > 0) && (buffer_offset == 0))
				{
					if (active_files[file_handle].current_sector_in_cluster == boot_sector_data.sectors_per_cluster - 1)
					{
						unsigned int cluster_number;

						if (find_first_empty_cluster(&cluster_number))
						{
							mark_cluster(active_files[file_handle].current_cluster_index, ((unsigned short int) (cluster_number & 0x0000ffff)), true);
							mark_cluster(cluster_number, 0xffff, true);
							mark_cluster(active_files[file_handle].current_cluster_index, ((unsigned short int) (cluster_number & 0x0000ffff)), false);
							mark_cluster(cluster_number, 0xffff, false);
							active_files[file_handle].current_cluster_index = cluster_number;
							active_files[file_handle].current_sector_in_cluster = 0;
						}
						else
						{
							return false;
						}
					}
					else
					{
						active_files[file_handle].current_sector_in_cluster = active_files[file_handle].current_byte_position / boot_sector_data.sector_size_in_bytes;
					}
					data_sector = boot_sector_data.data_sector_offset + (active_files[file_handle].current_cluster_index - 2)*boot_sector_data.sectors_per_cluster +
                          active_files[file_handle].current_sector_in_cluster;
				}
			}
            if (current_sector_index != data_sector + fat_partition_offset_in_512_byte_sectors)
            {
                if (!Read_Sector_Data(data_sector, fat_partition_offset_in_512_byte_sectors))
                {
					return false;
                }
            }
			IOWR_8DIRECT(buffer_memory, buffer_offset, byte_of_data);
			active_files[file_handle].current_byte_position = active_files[file_handle].current_byte_position + 1;

			if (active_files[file_handle].current_byte_position >= active_files[file_handle].file_size_in_bytes)
			{
				active_files[file_handle].file_size_in_bytes = active_files[file_handle].file_size_in_bytes + 1;
				active_files[file_handle].modified = true;
			}
            current_sector_modified = true;
			result = true;
		}
    }
    
    return result;
}


bool SDCard::alt_up_sd_card_fclose(short int file_handle)
{
    bool result = false;
    //if ((alt_up_sd_card_is_Present()) && (is_sd_card_formated_as_FAT16))
    {
        if (active_files[file_handle].in_use) 
        {
			if (active_files[file_handle].modified)
			{
				unsigned int record_sector = active_files[file_handle].file_record_sector_in_cluster;
				if (active_files[file_handle].file_record_cluster == 0)
				{
					record_sector = record_sector + boot_sector_data.root_directory_sector_offset;
				}
				else
				{
					record_sector = record_sector + boot_sector_data.data_sector_offset + 
									(active_files[file_handle].file_record_cluster - 2)*boot_sector_data.sectors_per_cluster;
				}
				if (Read_Sector_Data(record_sector, fat_partition_offset_in_512_byte_sectors))
				{
					if (Write_File_Record_At_Offset(active_files[file_handle].file_record_offset, &(active_files[file_handle])))
					{
						result = Save_Modified_Sector();
					}
				}
			}
			active_files[file_handle].in_use = false;
			result = true;
        }
    }
    
    return result;
}

short int MyFile::read()
{
    sd->readFile(fd);
}

size_t MyFile::fread(void *ptr, size_t size, size_t nmemb)
{
    for (int i = 0; i < size * nmemb; i++)
        *((uint8_t *)ptr + i) = (uint8_t)this->read();
}

unsigned int MyFile::getSize()
{
    return sd->active_files[this->fd].file_size_in_bytes;
}

bool SDCardEx::write(int sd_fileh, char c)
{
    return this->alt_up_sd_card_write(sd_fileh, c);
}

bool SDCardEx::fclose(int sd_fileh)
{
    return alt_up_sd_card_fclose(sd_fileh);
}

MyFileRecord::MyFileRecord(t_file_record *fr)
{
    this->fr = new t_file_record;
    ::memcpy(fr, this->fr, sizeof(t_file_record));
}

const char *MyFileRecord::toString()
{
    //static char filename[13] = "hello\0\0\0\0";
    //::memset(filename, 0, sizeof(filename));
    //::strncpy(filename, (char *)fr->name, 5);
    Uart::getInstance()->puts((const char *)this->fr->name);
    return (const char *)fr->name;
}



