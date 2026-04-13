#include "adxl.h"


//int fd;
int adxl_init() {
    //checking the DEVID //register address
    uint8_t value;
    I2Cread(fd,ADXL_BASE_ADDRESS,DEVID,&value); //reading the DEVID reg
    if(value != 0xE5) {
        perror("Error");
        return -1;
    }
    uint8_t control =0;
    control |= 0x08;
    //enable measurement mode in the power control register
    I2Cwrite(fd,ADXL_BASE_ADDRESS,POWER_CONTROL,control);
    return 0;
}

int adxl_range(volatile RANGE_t range) {
    uint8_t value;
    I2Cread(fd,ADXL_BASE_ADDRESS,DATA_FORMAT,&value);
    value &=~(0x03); //clearing bits 0 and 1
    switch(range) {
        case RANGE_2G:
            value |=0x00;
            break;
        case RANGE_4G:
            value |=0x01;
            break;
        case RANGE_8G:
            value |=0x02;
            break;
        case RANGE_16G:
            value |=0x03;
            break;
        default:
            break;
            
    }   
    I2Cwrite(fd,ADXL_BASE_ADDRESS,DATA_FORMAT,value);
    return 0;
}

//converting the raw values into gvalues using resolution=0.0039
int adxl_read(float *x,float *y,float *z) {
    uint8_t  buffer[6];
    if((I2Cread_mul(fd,ADXL_BASE_ADDRESS,DATAX0,buffer,6))<0) {
        perror("Error");
        return -1;
    }
    //shifting msb and then OR with lsb
    int16_t raw_x = (int16_t)(buffer[1]<<8 | buffer[0]);
    int16_t raw_y = (int16_t)(buffer[3]<<8 | buffer[2]);
    int16_t raw_z = (int16_t)(buffer[5]<<8 | buffer[4]);

    *x = (float)raw_x * resolution;
    *y = (float)raw_y * resolution;
    *z = (float)raw_z * resolution;
    return 0;
}

int adxl_interrupts(int fd) {
    //set the threshold for the activity
    //threshold = register value * scale factor == 32*62.5mg = 2000mg = 2.0g
    //hex value for 32 is 0x20
    I2Cwrite(fd,ADXL_BASE_ADDRESS,THRESH_ACT,0x20);
    //enabling x,y,z for direction
    I2Cwrite(fd, ADXL_BASE_ADDRESS, 0x27, 0x70);
    uint8_t value;
    //map activity to INT1(which will be set if the value there is zero);
    I2Cread(fd,ADXL_BASE_ADDRESS,INT_MAP,&value);
    value &= ~(0x10); //clearing that bit will make the value zero
    I2Cwrite(fd,ADXL_BASE_ADDRESS,INT_MAP,value);//reading it back

    //added
    uint8_t format;
    I2Cread(fd, ADXL_BASE_ADDRESS, DATA_FORMAT, &format);
    format &= ~(0x20); // Ensure Bit 5 is 0 for Active High
    I2Cwrite(fd, ADXL_BASE_ADDRESS, DATA_FORMAT, format);

    uint8_t enable;
    I2Cread(fd,ADXL_BASE_ADDRESS,INT_ENABLE,&enable);
    enable |= 0x10; //setting the interrput for activity
    return I2Cwrite(fd,ADXL_BASE_ADDRESS,INT_ENABLE,enable);
}

//reading INT_SOURCE just clears the interrupts
int interrupt_clear(void) {
    uint8_t value;
    return I2Cread(fd,ADXL_BASE_ADDRESS,INT_SOURCE,&value);
}