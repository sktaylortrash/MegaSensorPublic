
//Multi-Sensor Ardiuno Mega 2560
//MQTT Definitions stored in secrets.h

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Bounce2.h>
#include "DHT.h"
#include <SimpleTimer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <HeartBeat.h>


EthernetClient ethClient;
PubSubClient client(ethClient);

#include "definitions.h"
#include "secrets.h"

void initHardware() {
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(53); 
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 getMAC();
}


void setup() {
  pinMode(hbPin, OUTPUT);
  initHardware();
  initDHT();
  initPIR();
  initButton();
  sensors.begin(); 
  // start the Ethernet connection:
  Ethernet.begin(mac, ip);
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  delay(200);
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the controller
  Serial.print("Controller 1 is online at ");
  Serial.println(Ethernet.localIP());
  delay(2000);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("Controller1", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }

  client.publish("controller/1", "Controller 1 is Online");
  client.subscribe("controller/1/update");
  readDHT();
  readDS();
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  if ((char)payload[0] == '1'){
    readDHT();
    readDS();
  }
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}

void loop() {
  timer.run();
  // Update the Bounce instance :
  checkButton();
  timer.setInterval(300000,  readDHT );
  timer.setInterval(300000,  readDS );
  readPIR();
  hbTimerFunc();
client.loop();
  
}
