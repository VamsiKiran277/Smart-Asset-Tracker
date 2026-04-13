#include <stdio.h>
#include <stdint.h>
#include "i2c_master.h"
#include "adxl.h"
#include "DS3231.h"
#include <gpiod.h>

struct gpiod_chip *chip;
struct gpiod_line *line;
struct gpiod_line_event event;


int main() {
    fd = I2C_init(); //initializing i2c
    if(fd < 0) return -1;
    //initializing adxl
    if(adxl_init() < 0) {
        printf("adxl failed to initialize");
        return -1;
    }
    if(DS3231_init() < 0){ // initializing DS3231
        printf("DS3231 failed to initialize");
        return -1;
    }
    adxl_interrupts(fd);// initializing interrupts
    RTC_Time_t startTime = {0, 45, 16, 1, 13, 4, 2026}; // Sec, Min, Hour, Day, Date, Month, Year
    DS3231_set_time(&startTime);  //now the system has been synced no need for it for now
    printf("RTC Time has been synced!\n");

//---------------------------gpio
        int result;
        chip = gpiod_chip_open_by_name("gpiochip4"); //chip number 4
        if(!chip) {
            perror("Error");
            return -1;
        }
        line = gpiod_chip_get_line(chip,17); //getting the pin17
        if(!line) {
            perror("Error");
            gpiod_chip_close(chip);
        }
        //requesting for raising edge events //from 3.3v to 0
        if(gpiod_line_request_rising_edge_events(line,"Activity")<0) {
            perror("Request events failed");
            gpiod_chip_close(chip);
            return -1;
        }
//..................................
    RTC_Time_t current_time;
    float x, y, z;
    printf("Logger active waiting for Vibration\n");
    while(1) {
        //-----------------gpio
        //wait for the event
        //we wait a timeout of 1s and loopback to check again
        struct timespec timeout = {1,0};
        result = gpiod_line_event_wait(line, &timeout); //waiting 
        if(result>0) {
            gpiod_line_event_read(line, &event); //getting GPIO event
        //    printf("ISR enabled signalling Activity\n");

            //------------------------
            adxl_read(&x, &y, &z);         // Get G-forces
            DS3231_get_time(&current_time);         // Get Timestamp

            //ACTUATOR LOG FILE to save the file
            FILE *logfile = fopen("impact_history.csv", "a");
            // Professional Log Output
            if (logfile != NULL) {
                fprintf(logfile, "%02d/%02d/%04d,%02d:%02d:%02d,%.2f,%.2f,%.2f\n",
                        current_time.date, current_time.month, current_time.year,
                        current_time.hours, current_time.minutes, current_time.seconds, x, y, z);
                fclose(logfile);
            }
            //TO show the user
            printf("\n[LOGGED] IMPACT DETECTED! X:%.2fg Y:%.2fg Z:%.2fg\n", x, y, z);
            interrupt_clear();
            //wait 0.5 seconds before another log
            usleep(500000);
        } else if(result ==0) {
            printf("Waiting for the activity");
            fflush(stdout);
        }
    }

    return 0;
}