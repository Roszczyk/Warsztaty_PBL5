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
#include "MQTTClient.h"
#include <json-c/json.h>

#define ADDRESS     "eu1.cloud.thethings.network:1883"
#define CLIENTID    "C-listiner123456"
#define TOPIC       "v3/pam-pbl5-app@ttn/devices/eui-70b3d57ed0062e09/down/push"
#define PAYLOAD     "{ \"downlinks\": [{ \"f_port\": 15, \"frm_payload\": \"vu8=\", \"priority\": \"NORMAL\" }] }"
#define QOS         1
#define TIMEOUT     10000L

#define USERNAME	"pam-pbl5-app@ttn"
#define PASSWORD	"NNSXS.O4RU2XGURUSS2TVISWIH2ZCNU4KMG2X2YJM7OKA.JZCRHCOJJCDA4UTAEH5ATS6NW6F3DS2M4KB3H3H3DYFWUD2AXZVQ"


int main(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

	conn_opts.username =  "pam-pbl5-app@ttn";
	conn_opts.password = "NNSXS.O4RU2XGURUSS2TVISWIH2ZCNU4KMG2X2YJM7OKA.JZCRHCOJJCDA4UTAEH5ATS6NW6F3DS2M4KB3H3H3DYFWUD2AXZVQ";

//	printf("%s\n", conn_opts.username);

    if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
         printf("Failed to create client, return code %d\n", rc);
         exit(EXIT_FAILURE);
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = (int)strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    {
         printf("Failed to publish message, return code %d\n", rc);
         exit(EXIT_FAILURE);
    }

    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);

    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    	printf("Failed to disconnect, return code %d\n", rc);
    MQTTClient_destroy(&client);
    return rc;
}
