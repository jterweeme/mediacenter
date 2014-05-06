#include <stdio.h>
#include <system.h>
#include <stdbool.h>
#include <Altera_UP_SD_Card_Avalon_Interface.h>
 
#define SD_BUFFER_SIZE 512

short int sd_fileh;
 
char buffer[SD_BUFFER_SIZE] = "WELCOME TO THE INTERFACE!!\r\n\0";
 
int main()
{
  printf("SD Card Access Test\n");
 
  alt_up_sd_card_dev *sd_card_dev = alt_up_sd_card_open_dev(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
 
  if(sd_card_dev != 0)
  {
      if(alt_up_sd_card_is_Present())
      {
          if(alt_up_sd_card_is_FAT16())
              printf("Card is FAT16\n");
          else
              printf("Card is not FAT16\n");
 
          sd_fileh = alt_up_sd_card_fopen("piraat.txt", true);
 
          if (sd_fileh < 0)
              printf("Problem creating file. Error %i", sd_fileh);
          else
          {
              printf("SD Accessed Successfully, writing data...");
              int index = 0;
              while (buffer[index] != '\0')
              {
                  alt_up_sd_card_write(sd_fileh, buffer[index]);
                  index = index + 1;
              }
              printf("Done!\n");
 
              printf("Closing File...");
              alt_up_sd_card_fclose(sd_fileh);
              printf("Done!\n");
          }
      }
  }
 
  return 0;
}



