#define SYSTEM_BUS_WIDTH 32

#include "terasic_includes.h"
#include "I2C.h"

#ifdef DEBUG_I2C
    #define I2C_DEBUG(x)    DEBUG(x)  
#else
    #define I2C_DEBUG(x)
#endif

#define SDA_DIR_IN(data_base)   IOWR_ALTERA_AVALON_PIO_DIRECTION(data_base,0)
#define SDA_DIR_OUT(data_base)  IOWR_ALTERA_AVALON_PIO_DIRECTION(data_base,1) 
#define SDA_HIGH(data_base)     IOWR_ALTERA_AVALON_PIO_DATA(data_base, 1)
#define SDA_LOW(data_base)      IOWR_ALTERA_AVALON_PIO_DATA(data_base, 0)
#define SDA_READ(data_base)     IORD_ALTERA_AVALON_PIO_DATA(data_base)
#define SCL_HIGH(clk_base)     IOWR_ALTERA_AVALON_PIO_DATA(clk_base, 1)
#define SCL_LOW(clk_base)      IOWR_ALTERA_AVALON_PIO_DATA(clk_base, 0)
#define SCL_DELAY    usleep(1)

void i2c_start(alt_u32 clk_base, alt_u32 data_base);
void i2c_stop(alt_u32 clk_base, alt_u32 data_base);
bool i2c_write(alt_u32 clk_base, alt_u32 data_base, alt_u8 Data);
void i2c_read(alt_u32 clk_base, alt_u32 data_base, alt_u8 *pData, bool bAck);



bool I2C_Write(alt_u32 clk_base, alt_u32 data_base, alt_8 DeviceAddr, alt_u8 ControlAddr, alt_u8 ControlData){
    bool bSuccess = true;

    i2c_start(clk_base, data_base);
    if (!i2c_write(clk_base, data_base, DeviceAddr)){  // send ID
        bSuccess = FALSE;
    }
    if (bSuccess && !i2c_write(clk_base, data_base, ControlAddr)){ // send sub-address
        bSuccess = FALSE;
    }            
    if (bSuccess && !i2c_write(clk_base, data_base, ControlData)){  
        bSuccess = FALSE;
    }
    i2c_stop(clk_base, data_base);
    
    usleep(7*1000); // delay to wait EE2 ready (at least 5 ms delay is required)
    
    return bSuccess;

    
}

bool I2C_Read(alt_u32 clk_base, alt_u32 data_base, alt_8 DeviceAddr, alt_u8 ControlAddr, alt_u8 *pControlData){
    bool bSuccess = TRUE;
   
    i2c_start(clk_base, data_base);
    if (!i2c_write(clk_base, data_base, DeviceAddr)){  // send ID
        bSuccess = FALSE;
        I2C_DEBUG(("I2C HMB_E2 Fail: Address NACK!\n"));
    }
    if (bSuccess && !i2c_write(clk_base, data_base, ControlAddr)){ // send sub-address
        bSuccess = FALSE;
        I2C_DEBUG(("I2C HMB_E2 Fail: SubAddress NACK!\n"));
    }            
    i2c_start(clk_base, data_base);  // restart
    DeviceAddr |= 1; // Read
    if (bSuccess && !i2c_write(clk_base, data_base, DeviceAddr)){  // send id
        bSuccess = FALSE;
        I2C_DEBUG(("I2C HMB_E2 Fail: Address+1 NACK!\n"));
    }
    
    if (bSuccess){
        i2c_read(clk_base, data_base, pControlData, FALSE);  // read
    }        
    i2c_stop(clk_base, data_base);
    
    return bSuccess;
}

bool I2C_MultipleRead(alt_u32 clk_base, alt_u32 data_base, alt_8 DeviceAddr, alt_u8 szData[], alt_u16 len){
    int i;
    bool bSuccess = TRUE;
    alt_u8 ControlAddr = 0;
   
    i2c_start(clk_base, data_base);
    if (!i2c_write(clk_base, data_base, DeviceAddr)){  // send ID
        bSuccess = FALSE;
        I2C_DEBUG(("I2C HMB_E2 Fail: Address NACK!\n"));
    }
    if (bSuccess && !i2c_write(clk_base, data_base, ControlAddr)){ // send sub-address
        bSuccess = FALSE;
        I2C_DEBUG(("I2C HMB_E2 Fail: SubAddress NACK!\n"));
    }    
    if (bSuccess)        
        i2c_start(clk_base, data_base);  // restart
    DeviceAddr |= 1; // Read
    if (bSuccess && !i2c_write(clk_base, data_base, DeviceAddr)){  // send id
        bSuccess = FALSE;
        I2C_DEBUG(("I2C HMB_E2 Fail: Address+1 NACK!\n"));
    }
    
    if (bSuccess){
        for(i=0;i<len && bSuccess;i++){
            i2c_read(clk_base, data_base, &szData[i], (i==(len-1))?FALSE:TRUE);  // read
        }            
    }        
    i2c_stop(clk_base, data_base);
    
    return bSuccess;    
    
}

void i2c_start(alt_u32 clk_base, alt_u32 data_base){
    
    // make sure it is in normal state
    SDA_DIR_OUT(data_base);  // data output enabled
    
    
    
    // start condition 
    SDA_HIGH(data_base); // data high
    SCL_HIGH(clk_base);
    SCL_DELAY;
     
    SDA_LOW(data_base); // data low
    SCL_DELAY; 
    SCL_LOW(clk_base); // clock low
    SCL_DELAY;
}

// SDA 0->1 while SCL=1
void i2c_stop(alt_u32 clk_base, alt_u32 data_base){
    // assume SCL = 0
    
    SDA_DIR_OUT(data_base);  // data output enabled
    SDA_LOW(data_base); // Data Low
    //SCL_DELAY; 
    SCL_HIGH(clk_base);  // clock high
    SCL_DELAY; // clock high long delay
    SDA_HIGH(data_base); // data high
    SCL_DELAY; // data high delay
}

bool i2c_write(alt_u32 clk_base, alt_u32 data_base, alt_u8 Data){ // return true if device response ack
    alt_u8 Mask = 0x80;
    bool bAck;
    int i;
    
    // assume, SCL = 0
    
    SDA_DIR_OUT(data_base);  // data write mode
    
    for(i=0;i<8;i++){
        SCL_LOW(clk_base);  // new, make sure data change at clk low
        // output data on bus
        if (Data & Mask){ // there is a delay in this command
            SDA_HIGH(data_base);
        }else{    
            SDA_LOW(data_base);
        }
        Mask >>= 1;
        SCL_HIGH(clk_base);
        SCL_DELAY;
        SCL_LOW(clk_base);
        SCL_DELAY;
    }
    
    SDA_DIR_IN(data_base);
    SCL_HIGH(clk_base);
    SCL_DELAY;
    bAck = SDA_READ(data_base)?FALSE:TRUE;  // get ack
    SCL_LOW(clk_base); // clock low         
    SCL_DELAY; // clock low delay
    return bAck;
}    

void i2c_read(alt_u32 clk_base, alt_u32 data_base, alt_u8 *pData, bool bAck){ // return true if device response ack
    alt_u8 Data=0;
    int i;
    
    // assume SCL = low
    
    SDA_DIR_IN(data_base);  // set data read mode
    SCL_LOW(clk_base); // clock low
    SCL_DELAY; // clock low delay

    for(i=0;i<8;i++){
        Data <<= 1;
        SCL_HIGH(clk_base);  // clock high
        SCL_DELAY;
        if (SDA_READ(data_base))  // read data   
            Data |= 0x01;
        SCL_LOW(clk_base);  // clock log  
        SCL_DELAY;
    }
    
    SCL_LOW(clk_base);
    SDA_DIR_OUT(data_base);
    if (bAck)
        SDA_LOW(data_base);
    else
        SDA_HIGH(data_base);
    SCL_HIGH(clk_base); // clock high
    SCL_DELAY; // clock high  delay
    SCL_LOW(clk_base); // clock low
    SCL_DELAY; // clock low delay
    SDA_LOW(data_base);  // data low
    SCL_DELAY; // data low delay
    *pData = Data;
}

