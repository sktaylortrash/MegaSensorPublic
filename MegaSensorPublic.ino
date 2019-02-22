
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
#include "timer.h"


EthernetClient ethClient;
PubSubClient client(ethClient);

#include "variables.h"
#include "secrets.h"

void initHardware() {
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(53); 
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  byte i;
  byte dsAddress[8];
  Serial.println( "Searching for DS18B20..." );
  ds.reset_search();  // Start the search with the first device
  if( !ds.search(dsAddress) )
  {
    Serial.println( "none found. Using default MAC address." );
  } else {
    Serial.println( "success. Setting MAC address:" );
    Serial.print( " DS18B20 ROM  =" );
    for( i = 0; i < 8; i++)
    {
      Serial.write(' ');
      Serial.print( dsAddress[i], HEX );
    }
    Serial.println();
    // Offset array to skip DS18B20 family code, and skip mac[0]
    mac[1] = dsAddress[3];
    mac[2] = dsAddress[4];
    mac[3] = dsAddress[5];
    mac[4] = dsAddress[6];
    mac[5] = dsAddress[7];
  }    
  Serial.print( " Ethernet MAC =" );
  for( i = 0; i < 6; i++ )
  {
    Serial.write( ' ' );
    Serial.print( mac[i], HEX );
  }
  Serial.println();
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
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
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
   debouncer0.update();
   if ( debouncer0.fell() ) {
    client.publish("controller/1/pin/0/btn", "1");
   }
   debouncer1.update();
   if ( debouncer1.fell() ) {
    client.publish("controller/1/pin/1/btn", "1");
   }
  timer.setInterval(300000,  readDHT );
  timer.setInterval(300000,  readDS );
  readPIR();
  hbTimerFunc();
client.loop();
  
}
