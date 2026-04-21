#include "mqtt_client.h"
#include <string.h>

#define mqtt_port 8883
#define AUTHMETHOD  "vamsi"
#define AUTHTOKEN  "Pointbreak27"
#define TOPIC_IMPACT "asset_tracker/impact"
#define TOPIC_STATUS "asset_tracker/status"
#define Address "ssl://40db01641ba149dc91182576560ea139.s1.eu.hivemq.cloud:8883"
#define Client_id  "vamsi_tracker"
bool retain_flag = false;
typedef enum {
    QOS_0, //at most once
    QOS_1, //at least once
    QOS_2  //exactly once
}QOS_t;
    MQTTClient client;
int mqtt_init() {
    MQTTClient_connectOptions conn_ops = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions ssl_opt = MQTTClient_SSLOptions_initializer; 
    static MQTTClient_willOptions last_will = MQTTClient_willOptions_initializer;
    MQTTClient_create(&client,Address,Client_id,MQTTCLIENT_PERSISTENCE_NONE,NULL);
    conn_ops.keepAliveInterval = 20;
    conn_ops.cleansession = 1;
    conn_ops.username = AUTHMETHOD;
    conn_ops.password = AUTHTOKEN;

    //ssl options for HiveMQ
    ssl_opt.trustStore = NULL;
    ssl_opt.enableServerCertAuth = 1; //verify the server
    ssl_opt.sslVersion = MQTT_SSL_VERSION_TLS_1_2; // Force TLS 1.2
    conn_ops.ssl = &ssl_opt; //need to link it

    //last will and testanment
    last_will.topicName = TOPIC_STATUS;
    last_will.message = "{\"status\":\"offline\"}";
    last_will.retained = 1;
    last_will.qos = QOS_1;
    conn_ops.will = &last_will; //need to link it

    printf("Connecting to %s...\n", Address);
    //connect
    int rc = MQTTClient_connect(client, &conn_ops);
    if(rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        return -1;
    }
    printf("Connected to the broker\n");
    return 0;
}

void mqtt_publish(RTC_Time_t time, float x,float y, float z) {
 printf("--- Attempting Cloud Sync... ---\n");
 //creating JSON
 char str_payload[128]; //payload string
 snprintf(str_payload,sizeof(str_payload),
 "{\"timestamp\":\"%02d/%02d/%04d %02d:%02d:%02d\", \"x\":%.2f, \"y\":%.2f, \"z\":%.2f}",
    time.date,time.month,time.year,
    time.hours,time.minutes,time.seconds,
    x,y,z);
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    pubmsg.payload = str_payload;
    pubmsg.payloadlen = (int)strlen(str_payload);
    pubmsg.qos = QOS_1;  //delivers 
    pubmsg.retained = 1; //last impact for QT dashboard
    //send the message
    MQTTClient_publishMessage(client,TOPIC_IMPACT,&pubmsg, &token);
    //Wait for the broker to acknowledge
    MQTTClient_waitForCompletion(client,token,1000L);//useful of QOS1
}