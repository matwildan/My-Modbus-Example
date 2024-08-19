#include <Arduino.h>
#include "MyModbusSensor.h"
#include "InitModbus.h"

// ===== Pressure Parameters =====
int prID[] = {4, 5, 6}; // Assign the address of Modbus
int numPr = sizeof(prID)/sizeof(int);
MyModbusSensor** pressure;

// Example of pressure modbus sensor value polling
void pressurePolling(MyModbusSensor *pressure[], int prID[], int prNum)
{
  const char* sensor = "pressure";
  char prData[512];
  String sPrData = "[";
  for(int i = 0; i < prNum; i++){
    float prVal = (*pressure[i]).readPressureSensor();
    Serial.printf("[%s-%d] Value: %s\n", sensor, i, String(prVal).c_str());
    sprintf(prData, "{\"id\":%d,\"value\":%s}", prID[i], String(prVal).c_str());
    sPrData += String(prData);
    if(prNum > i + 1){
      sPrData += ",";
    }
    delay(500);
  }
  sPrData += "]";
  Serial.println(sPrData);
  memset(prData,'\0',sizeof(prData));
}

void setup() {
  // put your setup code here, to run once:
  initMbSensor(pressure,prID,numPr);
}

void loop() {
  // put your main code here, to run repeatedly:
  pressurePolling(pressure, prID, numPr);

  /* Delete when you are done */
  // deleteMbSensor(pressure, numPr);

  delay(5000);
}