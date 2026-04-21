#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H
#include "stdbool.h"
#include "MQTTClient.h"
#include "DS3231.h"

//function definitions
int mqtt_init();
void mqtt_publish(RTC_Time_t time, float x,float y, float z);

#endif