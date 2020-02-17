/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo.h>

const int sPin1 = D1;
const int sPin2 = D2;
const int sPin3 = D3;
const int sPin4 = D4;
const int sPin5 = D5;

int value1; //save analog value
int value2;
int value3;
int value4;
int value5;

char val1[10];
char val2[10];
char val3[10];
char val4[10];
char val5[10];

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Your ssid"
#define WLAN_PASS       "Your password"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "Your ip"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Subscribe base = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/base");
Adafruit_MQTT_Subscribe ljoint = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ljoint");
Adafruit_MQTT_Subscribe hjoint = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/hjoint");
Adafruit_MQTT_Subscribe xrot = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/xrot");
Adafruit_MQTT_Subscribe yrot = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/yrot");
Adafruit_MQTT_Subscribe pincher = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/pincher");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  servo1.attach(sPin1);
  servo1.write(0);
  servo2.attach(sPin2);
  servo2.write(0);
  servo3.attach(sPin3);
  servo3.write(0);
  servo4.attach(sPin4);
  servo4.write(0);
  servo5.attach(sPin5);
  servo5.write(0);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&base);
  mqtt.subscribe(&ljoint);
  mqtt.subscribe(&hjoint);
  mqtt.subscribe(&xrot);
  mqtt.subscribe(&yrot);
  mqtt.subscribe(&pincher);
}



void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &base) {
      Serial.print(F("Got: "));
      Serial.println((char *)base.lastread);
      strcpy(val1,(char*)base.lastread);
      value1 = atoi(val1);
      servo1.write(value1); 
    }
    if (subscription == &ljoint) {
      Serial.print(F("Got: "));
      Serial.println((char *)ljoint.lastread);
      strcpy(val2,(char*)ljoint.lastread);
      value2 = atoi(val2);
      Serial.println(value2);
      servo2.write(value2);
    }
    if (subscription == &hjoint) {
      Serial.print(F("Got: "));
      Serial.println((char *)hjoint.lastread);
      strcpy(val3,(char*)hjoint.lastread);
      value3 = atoi(val3);
      servo3.write(value3);
    }
    if (subscription == &xrot) {
      Serial.print(F("Got: "));
      Serial.println((char *)xrot.lastread);
      strcpy(val4,(char*)xrot.lastread);
      value4 = atoi(val4);
      servo4.write(value4);
    }
    if (subscription == &yrot) {
      Serial.print(F("Got: "));
      Serial.println((char *)yrot.lastread);
      strcpy(val5,(char*)yrot.lastread);
      value5 = atoi(val5);
      servo5.write(value5);
    }
    if (subscription == &pincher) {
      Serial.print(F("Got: "));
      Serial.println((char *)pincher.lastread);
      strcpy(val1,(char*)pincher.lastread);
      value1 = atoi(val1);
    }
  }

  // Now we can publish stuff!
 

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
