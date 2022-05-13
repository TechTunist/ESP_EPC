#include "SimpleEspNowConnection.h"
#include "DHT.h"
#define Type DHT11

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::CLIENT);

String inputString;
String serverAddress;

int sensePin = 2;
DHT HT(sensePin, Type);

typedef struct dhtStruct {
  char type;
  float temp;
  float humidity;
} dhtStruct;




bool sendStruct() {
  dhtStruct dhtData;

  dhtData.type = '#';
  dhtData.temp = HT.readTemperature();
  dhtData.humidity = HT.readHumidity();
  sprintf ("Greetings from %s", simpleEspConnection.myAddress.c_str());
  
  return (simpleEspConnection.sendMessage((uint8_t *)&dhtData, sizeof(dhtData)));
}

void OnSendError(uint8_t* ad)
{
  Serial.println("SENDING TO '"+simpleEspConnection.macToStr(ad)+"' WAS NOT POSSIBLE!");
}

void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
  if((char)message[0] == '#')
  {
    dhtStruct dhtData;
  
    memcpy(&dhtData, message, len);   
    Serial.printf("%f,%f,Main Office\n", dhtData.temp, dhtData.humidity);
  }
  else
    Serial.printf("MESSAGE:[%d]%s from %s\n", len, (char *)message, simpleEspConnection.macToStr(ad).c_str());
}


void OnNewGatewayAddress(uint8_t *ga, String ad)
{  
  Serial.println("New GatewayAddress '"+ad+"'");
  serverAddress = ad;

  simpleEspConnection.setServerMac(ga);
}


void setup() {
  pinMode(sensePin, INPUT_PULLUP);

  Serial.begin(115200);

  simpleEspConnection.begin();

  simpleEspConnection.setServerMac(serverAddress);
  simpleEspConnection.onNewGatewayAddress(&OnNewGatewayAddress);    
  simpleEspConnection.onSendError(&OnSendError);  
  simpleEspConnection.onMessage(&OnMessage);  
  
  Serial.println(WiFi.macAddress());  

  // initialise sensor activity
  HT.begin();

  // send automated pair request
  simpleEspConnection.startPairing(30);

}

void loop() {

  simpleEspConnection.loop();
  
  sendStruct();
  delay(3000);
}
