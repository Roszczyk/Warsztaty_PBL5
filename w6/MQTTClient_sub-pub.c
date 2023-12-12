/*******************************************************************************
 * Copyright (c) 2012, 2022 IBM Corp., Ian Craggs
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   https://www.eclipse.org/legal/epl-2.0/
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <stdint.h>
#include <math.h>
#include "MQTTClient.h"

#define ADDRESS     "eu1.cloud.thethings.network:1883"
#define CLIENTID    "C-listiner123456"
#define TOPIC       "v3/pam-pbl5-app@ttn/devices/eui-70b3d57ed0062e09/up"
#define TOPIC_DOWN       "v3/pam-pbl5-app@ttn/devices/eui-70b3d57ed0062e09/down/push"

//#define TOPIC		"#"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_message pubmsg = MQTTClient_message_initializer;
MQTTClient_deliveryToken token;

#define RANGE 10

struct table_node{
    int value;
	char in_use;
};

uint32_t base64ToBinary(const char *base64)
{
    unsigned char binary[6] = {0};
    char decodingTable[256];
    int i, j;

    for (i = 0; i < 64; ++i)
        decodingTable["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

    int base64Len = strlen(base64);
    int binaryIndex = 0;

    for (i = 0; i < base64Len; i += 4)
    {
        unsigned int sextet1 = decodingTable[base64[i]];
        unsigned int sextet2 = decodingTable[base64[i + 1]];
        unsigned int sextet3 = decodingTable[base64[i + 2]];
        unsigned int sextet4 = decodingTable[base64[i + 3]];

        unsigned int triple = (sextet1 << 18) | (sextet2 << 12) | (sextet3 << 6) | sextet4;

        for (j = 2; j >= 0 && binaryIndex < 100; --j)
        {
            binary[binaryIndex++] = (triple >> (8 * j)) & 0xFF;
        }
    }
    uint32_t a = *(uint32_t *)binary;

    return a;
}

char *binaryToBase64(const uint8_t *binary, size_t binaryLen)
{
    const char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    size_t base64Len = ((binaryLen + 2) / 3) * 4 + 1; // Calculate the length of the base64 string
    char *base64 = (char *)malloc(base64Len);
    if (base64 == NULL)
    {
        // Handle memory allocation failure
        return NULL;
    }

    size_t i, j;
    for (i = 0, j = 0; i < binaryLen;)
    {
        uint32_t triple = (binary[i] << 16) | (i + 1 < binaryLen ? binary[i + 1] << 8 : 0) | (i + 2 < binaryLen ? binary[i + 2] : 0);

        base64[j++] = base64Chars[(triple >> 18) & 0x3F];
        base64[j++] = base64Chars[(triple >> 12) & 0x3F];

        if (i + 1 < binaryLen)
        {
            base64[j++] = base64Chars[(triple >> 6) & 0x3F];
        }
        else
        {
            base64[j++] = '=';
        }

        if (i + 2 < binaryLen)
        {
            base64[j++] = base64Chars[triple & 0x3F];
        }
        else
        {
            base64[j++] = '=';
        }

        i += 3;
    }

    base64[j] = '\0'; // Null-terminate the base64 string

    return base64;
}

void addToTable(struct table_node * table, int value)
{
    struct table_node temp1;
    struct table_node temp2;
    int i=1;
    temp1=table[0];
        do
    {
        temp2 = table[i];
        table[i] = temp1;
        temp1=temp2;
        i++;
    }     while(table[i-1].in_use == 1 && i<RANGE);
    table[0].value = value;
    table[0].in_use = 1;
}

uint32_t countAVG(struct table_node * table)
{
    uint32_t i=0;
    int sum=0;
    while(table[i].in_use==1 && i<RANGE)
    {
        sum=sum+table[i].value;
        i++;
    }
    return (uint32_t)((double)sum/(double)i);
}

uint32_t countStdDev(struct table_node * table)
{
    uint32_t i=0;
    int meter=0;
    uint32_t average = countAVG(table);
    while(table[i].in_use ==1 && i<RANGE)
    {
        meter=meter+(table[i].value-average)^2;
        i++;
    }
    return (uint32_t)(sqrt((double)meter/(double)i));
}

int pub_mean_sd (const char *payload)
{
    int rc = 0;
    pubmsg.payload = payload;
    pubmsg.payloadlen = (int)strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    if ((rc = MQTTClient_publishMessage(client, TOPIC_DOWN, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    {
         printf("Failed to publish message, return code %d\n", rc);
         exit(EXIT_FAILURE);
    }

    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), PAYLOAD, TOPIC_DOWN, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);

    // if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    // 	printf("Failed to disconnect, return code %d\n", rc);
    // MQTTClient_destroy(&client);
    return rc;
}


volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{   
    uint32_t rec_val = 0;
	uint32_t val = 0;
	uint32_t val2 = 0;
	struct table_node TABLE[RANGE];
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: %.*s\n\n\n", message->payloadlen, (char*)message->payload);

    struct json_object *jobj;
    jobj = json_tokener_parse((char*)message->payload);

    struct json_object *uplink_message;
    if (json_object_object_get_ex(jobj, "uplink_message", &uplink_message)) {
        struct json_object *frm_payload;
        if (json_object_object_get_ex(uplink_message, "frm_payload", &frm_payload)) {
            const char *frm_payload_value = json_object_get_string(frm_payload);
            if (frm_payload_value != NULL) {
                printf("Value of 'frm_payload': %s\n", frm_payload_value);
                rec_val = base64ToBinary(frm_payload_value);
                printf("Value of 'frm_payload': %ld\n", rec_val);
		addToTable(TABLE, (int)rec_val);
            } else {
                fprintf(stderr, "Error getting value of 'frm_payload'\n");
                return 1;
            }
        } else {
            fprintf(stderr, "Error accessing 'frm_payload'\n");
            return 1;
        }
    } else {
        fprintf(stderr, "Error accessing 'uplink_message'\n");
        return 1;
    }

    // Allocates storage
    uint8_t binaryData[8];
    char payload [100];
    
    // #### podmienić val i val2
	val=countAVG(TABLE);
	val2=countStdDev(TABLE);

    memcpy(binaryData, &val, 4);
    memcpy(binaryData+4, &val2, 4);

    char *base64String = binaryToBase64(binaryData, 8);
    // Prints "Hello world!" on hello_world
    sprintf(payload, "{ \"downlinks\": [{ \"f_port\": 15, \"frm_payload\": \"%s\", \"priority\": \"NORMAL\" }] }", base64String );
	printf("%s\n", payload);
    pub_mean_sd(payload);
    
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{

    int rc;

	// MQTTClient client;
    // MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    // MQTTClient_message pubmsg = MQTTClient_message_initializer;
    // MQTTClient_deliveryToken token;

    if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto exit;
    }

    if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    conn_opts.username ="pam-pbl5-app@ttn";
    conn_opts.password ="NNSXS.O4RU2XGURUSS2TVISWIH2ZCNU4KMG2X2YJM7OKA.JZCRHCOJJCDA4UTAEH5ATS6NW6F3DS2M4KB3H3H3DYFWUD2AXZVQ";
 
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    if ((rc = MQTTClient_subscribe(client, TOPIC, QOS)) != MQTTCLIENT_SUCCESS)
    {
    	printf("Failed to subscribe, return code %d\n", rc);
    	rc = EXIT_FAILURE;
    }
    else
    {
    	int ch;
    	do
    	{
        	ch = getchar();
    	} while (ch!='Q' && ch != 'q');

        if ((rc = MQTTClient_unsubscribe(client, TOPIC)) != MQTTCLIENT_SUCCESS)
        {
        	printf("Failed to unsubscribe, return code %d\n", rc);
        	rc = EXIT_FAILURE;
        }
    }

    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    {
    	printf("Failed to disconnect, return code %d\n", rc);
    	rc = EXIT_FAILURE;
    }
destroy_exit:
    MQTTClient_destroy(&client);
exit:
    return rc;
}
