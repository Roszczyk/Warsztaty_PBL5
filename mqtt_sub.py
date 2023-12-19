import random
import paho.mqtt.client as mqtt
import time


def random_integer():
    return random.randint(0, 100)


def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))


def on_message(client, userdata, message):
    print("Received message '" + str(message.payload) + "' on topic '"
          + message.topic + "' with QoS " + str(message.qos))


def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed to topic with MID " +
          str(mid) + " and QoS " + str(granted_qos))


def on_publish(client, userdata, mid):
    print("Message published")


if __name__ == '__main__':
    rand_temp = random_integer()

    client = mqtt.Client()

    client.on_connect = on_connect
    client.on_message = on_message
    client.on_subscribe = on_subscribe
    client.on_publish = on_publish

    # Connect to the MQTT broker
    client.username_pw_set("rw", "readwrite")

    # Connect to the MQTT broker
    # Replace with your MQTT broker address and port
    client.connect("test.mosquitto.org", 1884)

    # Publish a message to a topic
    topic = "t2"

    # Subscribe to the topic
    client.subscribe("PAM-PBL5-CATCHER")
    client.loop_forever()
