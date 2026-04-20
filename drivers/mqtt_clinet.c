#include "mqtt_client.h"
#define mqtt_port 8883
#define AUTHMETHOD  "vamsi"
#define AUTHTOKEN  "pointbreak"
#define topic_name "asset_tracker/impact"
#define Address  "ssl://40db01641ba149dc91182576560ea139.s1.eu.hivemq.cloud:8883"
#define Client_id  "vamsi_tracker"
bool retain_flag = false;
typedef enum {
    QOS_0, //at most once
    QOS_1, //at least once
    QOS_2  //exactly once
}QOS_t;

void mqtt_init() {
    MQTTClient client;
    MQTTClient_connectOptions conn_ops = MQTTClient_createOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    MQTTClient_create(&client,Address,Client_id,MQTTCLIENT_PERSISTENCE_NONE,NULL);
    conn_ops.keepAliveInterval = 20;
    conn_ops.cleansession = 1;
    conn_ops.username = AUTHMETHOD;
    conn_ops.password = AUTHTOKEN;

    //connect
    int rc;
    if((rc=MQTTClient_connect(client,&conn_ops)) != MQTTCLIENT_SUCCESS) {
        printf("failed to connect,return code %d\n", rc);
        return -1;
    }

    printf("Connected to the broker\n");
}

void mqtt_publish(RTC_Time_t time, float x,float y, float z) {

}