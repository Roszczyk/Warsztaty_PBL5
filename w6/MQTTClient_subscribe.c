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
#include "MQTTClient.h"

#define ADDRESS     "eu1.cloud.thethings.network:1883"
#define CLIENTID    "C-listiner123456"
#define TOPIC       "#"//"v3/pam-pbl5-app@ttn/devices/eui-70b3d57ed0062e09/up"
//#define TOPIC		"#"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

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



volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
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
                printf("Value of 'frm_payload': %ld\n", base64ToBinary(frm_payload_value));

                //printf("Decoded Value of 'frm_payload': %x\n", decoded);

            } else {
                fprintf(stderr, "Error getting value of 'frm_payload'\n");
            }
        } else {
            fprintf(stderr, "Error accessing 'frm_payload'\n");
        }
    } else {
        fprintf(stderr, "Error accessing 'uplink_message'\n");
    }

    
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
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

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
