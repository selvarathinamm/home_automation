/* 
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#ifndef SWITCH_CASE_INIT
  #define SWITCH_CASE_INIT
    #define SWITCH(X) for (char* __switch_p__ = X, __switch_next__=1 ; __switch_p__ ; __switch_p__=0, __switch_next__=1) { {
      #define CASE(X)     } if (!__switch_next__ || !(__switch_next__ = strcmp(__switch_p__, X))) {
    #define DEFAULT       } {
  #define END     }}
#endif

// Update these with values suitable for your network.

const char* ssid = "FIBERNET";
const char* password = "pass1342";
const char* mqtt_server = "192.168.0.111";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  SWITCH(topic)
    CASE("home/light1")
      relayAction(D0, payload);
    break;
    CASE("home/light2")
      relayAction(D1, payload);
    break;
    CASE("home/light3")
      relayAction(D2, payload);
    break;
    CASE("home/light4")
      relayAction(D3, payload);
    break;
    CASE("home/fan1")
      relayAction(D4, payload);
    break;
    CASE("home/fan2")
      relayAction(D5, payload);
    break;
    CASE("home/fan3")
      relayAction(D6, payload);
    break;
    CASE("home/fan4")
      relayAction(D7, payload);
    break;
    DEFAULT

    break;
END

}
void relayAction(int pin, byte* payload){
  if ((char)payload[0] == '1') {
    digitalWrite(pin, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(pin, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("keepalive", "I'm alive");
      // ... and resubscribe
      client.subscribe("home/light1");
      client.subscribe("home/light2");
      client.subscribe("home/light3");
      client.subscribe("home/light4");
      client.subscribe("home/fan1");
      client.subscribe("home/fan2");
      client.subscribe("home/fan3");
      client.subscribe("home/fan4");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "I'm alive #%ld", value);
    //Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("keepalive", "I'm alive");
  }
}
