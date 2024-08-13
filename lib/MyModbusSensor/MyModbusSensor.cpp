#include "MyModbusSensor.h"

#define REGISTER_ADDRESS_MUX 128
#define REGISTER_COUNT_MUX 32
#define REGISTER_ADDRESS_2 0
#define REGISTER_COUNT_2 16
#define REGISTER_ADDRESS_4 4
#define REGISTER_COUNT_4 2
#define REGISTER_ADDRESS_AWS 0
#define REGISTER_COUNT_AWS 100

Conductivity ConductivityData;
Gas GasData;
AWS AWSData;
SHT20 SHT20Data;
Slicer_Ultrasonic SlicerUltrasonicData;
TempSensor TempSensorData;
PZEM_MODBUS PMData;
VEGA_C23 vega_c23_data;

HardwareSerial _mb(PC7, PC6);

MyModbusSensor::MyModbusSensor(int address)
    : _address(address) {}

void MyModbusSensor::init()
{
  _mb.begin(9600);
  _node.begin(_address, _mb);
}

void MyModbusSensor::end()
{
  _mb.end();
}

float _parseFloatFromBytes(byte *buffer, int startIndex, bool are4Bytes = true, bool isShiftBytes = false)
{
  union
  {
    float fV;
    long d;
  } data;

  if (are4Bytes == true)
  {
    if (isShiftBytes == false)
    {
      long val = (buffer[startIndex] << 24) | (buffer[startIndex + 1] << 16) | (buffer[startIndex + 2] << 8) | buffer[startIndex + 3];
      data.d = val;
    }
    else
    {
      long val = (buffer[startIndex + 2] << 24) | (buffer[startIndex + 3] << 16) | (buffer[startIndex] << 8) | buffer[startIndex + 1];
      data.d = val;
    }
    return data.fV;
  }
  else
  {
    if (isShiftBytes == false)
    {
      int intVal = (buffer[startIndex] << 8) | buffer[startIndex + 1];
      return intVal;
    }
  }
  return data.fV;
}

void _convertUInt16ToByte(const uint16_t *src, byte *dest, uint16_t length)
{
  for (uint16_t i = 0; i < length; i++)
  {
    dest[i * 2] = (src[i] >> 8) & 0xFF; // Extract the high byte
    dest[i * 2 + 1] = src[i] & 0xFF;    // Extract the low byte
  }
}

PZEM_MODBUS MyModbusSensor::readPZEMSensor()
{
  uint16_t buffer[16];
  byte bBuffer[16];

  uint8_t result = _node.readInputRegisters(0, 10);

  // Check if Modbus request was successful
  if (result == _node.ku8MBSuccess)
  {
    // Print the received data
    for (int i = 0; i < 10; i++)
    {
      uint16_t value = _node.getResponseBuffer(i);
      buffer[i] = value;
      // Serial.printf("Buf %d: ",i);
      // Serial.print(buffer[i], HEX);
    }
    // Serial.println();
    _convertUInt16ToByte(buffer, bBuffer, 16);

    // Extract Voltage value (4 bytes starting from index 0)
    PMData.voltage = _parseFloatFromBytes(bBuffer, 0, false) / 10;

    // Extract Current value (4 bytes starting from index 2)
    PMData.current = _parseFloatFromBytes(bBuffer, 2, false) / 1000;

    // Extract Power value (4 bytes starting from index 16)
    PMData.power = _parseFloatFromBytes(bBuffer, 6, false) / 10;
  }
  else
  {
    PMData.voltage = -1.0;
    PMData.current = -1.0;
    PMData.power = -1.0;
  }
  memset(buffer, 0, sizeof(buffer));
  memset(bBuffer, '\0', sizeof(bBuffer));
  return PMData;
}

Conductivity MyModbusSensor::readConductivitySensor()
{
  uint16_t buffer[16];
  byte bBuffer[16];

  uint8_t result = _node.readHoldingRegisters(REGISTER_ADDRESS_2, REGISTER_COUNT_2);

  // Check if Modbus request was successful
  if (result == _node.ku8MBSuccess)
  {
    for (int i = 0; i < REGISTER_COUNT_2; i++)
    {
      uint16_t value = _node.getResponseBuffer(i);
      buffer[i] = value;
    }
    _convertUInt16ToByte(buffer, bBuffer, 16);

    // Extract EC value (4 bytes starting from index 0)
    ConductivityData.ec = _parseFloatFromBytes(bBuffer, 0);

    // Extract temperature value (4 bytes starting from index 8)
    ConductivityData.temp = _parseFloatFromBytes(bBuffer, 8);

    // Extract salinity value (4 bytes starting from index 16)
    ConductivityData.salinity = _parseFloatFromBytes(bBuffer, 16);
  }
  else
  {
    ConductivityData.ec = -1.0;
    ConductivityData.temp = -1.0;
    ConductivityData.salinity = -1.0;
  }
  memset(buffer, 0, sizeof(buffer));
  memset(bBuffer, '\0', sizeof(bBuffer));
  return ConductivityData;
}

void MyModbusSensor::readMuxValue(char dest[])
{
  uint16_t muxBuffer[32];
  char cMuxBufffer[50];
  memset(muxBuffer, 0, sizeof(muxBuffer));
  memset(cMuxBufffer, '\0', sizeof(cMuxBufffer));

  // Read the 16-byte data from Modbus slave - Address 128
  uint8_t result = _node.readHoldingRegisters(REGISTER_ADDRESS_MUX, REGISTER_COUNT_MUX);

  // Check if Modbus request was successful
  if (result == _node.ku8MBSuccess)
  {
    for (int i = 0; i < REGISTER_COUNT_MUX; i++)
    {
      uint16_t value = _node.getResponseBuffer(i);
      muxBuffer[i] = value;
    }
    sprintf(cMuxBufffer, "{\"state\":[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]}",
            muxBuffer[0], muxBuffer[1], muxBuffer[2], muxBuffer[3], muxBuffer[4],
            muxBuffer[5], muxBuffer[6], muxBuffer[7], muxBuffer[8], muxBuffer[9], muxBuffer[10],
            muxBuffer[11], muxBuffer[12], muxBuffer[13], muxBuffer[14], muxBuffer[15]);
  }
  else
  {
    int muxErr = -1;
    sprintf(cMuxBufffer, "{\"state\":[%d]}", muxErr);
  }
  sprintf(dest, "%s", cMuxBufffer);
}

float MyModbusSensor::readLevelTransmitterSensor()
{
  uint16_t trBuffer[8];
  byte bTrBuffer[8];
  // Read the 4-byte data from Modbus slave - Address 4
  float tr;
  uint8_t result = _node.readHoldingRegisters(REGISTER_ADDRESS_4, REGISTER_COUNT_4);

  // Check if Modbus request was successful for Slave 2
  if (result == _node.ku8MBSuccess)
  {
    // Print the received data
    for (int i = 0; i < REGISTER_COUNT_4; i++)
    {
      uint16_t value = _node.getResponseBuffer(i);
      trBuffer[i] = value;
    }
    _convertUInt16ToByte(trBuffer, bTrBuffer, 8);
    tr = _parseFloatFromBytes(bTrBuffer, 0, false);
  }
  else
  {
    // Print an error message for Slave 2
    tr = -1.0;
  }
  memset(trBuffer, 0, sizeof(trBuffer));
  memset(bTrBuffer, '\0', sizeof(bTrBuffer));
  return tr;
}

float MyModbusSensor::readPressureSensor()
{
  uint16_t prBuffer[8];
  byte bPrBuffer[8];

  // Read the 4-byte data from Modbus slave - Address 4
  float pr;
  uint8_t result = _node.readHoldingRegisters(REGISTER_ADDRESS_4, REGISTER_COUNT_4);

  // Check if Modbus request was successful for Slave 2
  if (result == _node.ku8MBSuccess)
  {
    // Print the received data
    for (int i = 0; i < REGISTER_COUNT_4; i++)
    {
      uint16_t value = _node.getResponseBuffer(i);
      prBuffer[i] = value;
    }
    _convertUInt16ToByte(prBuffer, bPrBuffer, 8);
    pr = _parseFloatFromBytes(bPrBuffer, 0, false);
  }
  else
  {
    // Print an error message
    pr = -1.0;
  }
  memset(prBuffer, 0, sizeof(prBuffer));
  memset(bPrBuffer, '\0', sizeof(bPrBuffer));
  return pr;
}

Gas MyModbusSensor::readGasSensor()
{
  uint16_t gasBuffer[16];
  byte bGasBuffer[16];

  // Read the 2-byte data from Modbus slave - Address 0
  uint8_t result = _node.readHoldingRegisters(REGISTER_ADDRESS_2, REGISTER_COUNT_2);

  // Check if Modbus request was successful
  if (result == _node.ku8MBSuccess)
  {
    // Print the received data
    for (int i = 0; i < REGISTER_COUNT_2; i++)
    {
      uint16_t value = _node.getResponseBuffer(i);
      gasBuffer[i] = value;
    }
    _convertUInt16ToByte(gasBuffer, bGasBuffer, 16);

    // Extract gas PM value (4 bytes starting from index 0)
    GasData.pm = _parseFloatFromBytes(bGasBuffer, 0, false);

    // Extract gas CO value (4 bytes starting from index 2)
    GasData.co = _parseFloatFromBytes(bGasBuffer, 2, false);

    // Extract gas SO2 value (4 bytes starting from index 4)
    GasData.so2 = (_parseFloatFromBytes(bGasBuffer, 4, false)) / 10;

    // Extract gas NO value (4 bytes starting from index 6)
    GasData.no = _parseFloatFromBytes(bGasBuffer, 6, false);
  }
  else
  {
    GasData.pm = -1.0;
    GasData.co = -1.0;
    GasData.so2 = -1.0;
    GasData.no = -1.0;
  }
  memset(gasBuffer, 0, sizeof(gasBuffer));
  memset(bGasBuffer, '\0', sizeof(bGasBuffer));
  return GasData;
}

AWS MyModbusSensor::readAWS()
{
  uint16_t awsBuffer[100];
  byte bGasBuffer[100];

  // Read the 4-byte data from Modbus slave - Address 4
  uint8_t result = _node.readHoldingRegisters(REGISTER_ADDRESS_AWS, REGISTER_COUNT_AWS);

  // Check if Modbus request was successful
  if (result == _node.ku8MBSuccess)
  {
    // Print the received data
    for (int i = 0; i < REGISTER_COUNT_AWS; i++)
    {
      uint16_t value = _node.getResponseBuffer(i);
      awsBuffer[i] = value;
      Serial.printf("Buffer[%d]: ", i);
      Serial.println(awsBuffer[i], HEX);
    }
    _convertUInt16ToByte(awsBuffer, bGasBuffer, 100);

    AWSData.windDir = _parseFloatFromBytes(bGasBuffer, 2, false);
    AWSData.windSpeed = _parseFloatFromBytes(bGasBuffer, 4, true, true);
    AWSData.temp = _parseFloatFromBytes(bGasBuffer, 8, true, true);
    AWSData.humid = _parseFloatFromBytes(bGasBuffer, 12, true, true);
    // float pressure = _parseFloatFromBytes(bGasBuffer, 16, true, true);
    // float elecCompass = _parseFloatFromBytes(bGasBuffer, 20, false);
    // float rainsnow = _parseFloatFromBytes(bGasBuffer, 22, false);
    // float rainfall = _parseFloatFromBytes(bGasBuffer, 24, true, true);
    AWSData.rainfallAcc = _parseFloatFromBytes(bGasBuffer, 28, true, true);
    // float rainfallUnit = _parseFloatFromBytes(bGasBuffer, 32, false);
    // float posStatus = _parseFloatFromBytes(bGasBuffer, 34, false);
    // float shipSpeed = _parseFloatFromBytes(bGasBuffer, 36, true, true);
    // float course = _parseFloatFromBytes(bGasBuffer, 40, false);
    // float longitude = _parseFloatFromBytes(bGasBuffer, 42, true, true);
    // float latitude = _parseFloatFromBytes(bGasBuffer, 46, true, true);
    AWSData.dust = _parseFloatFromBytes(bGasBuffer, 50, true, true);
    // float visibility = _parseFloatFromBytes(bGasBuffer, 54, true, true);
    // float illuminance = _parseFloatFromBytes(bGasBuffer, 58, true, true);
    // float radAccu = _parseFloatFromBytes(bGasBuffer, 62, true, true);
    AWSData.radiation = _parseFloatFromBytes(bGasBuffer, 66, true, true);
    // float realWinDir = _parseFloatFromBytes(bGasBuffer, 70, true, true);
    // float altitude = _parseFloatFromBytes(bGasBuffer, 74, true, true);
    // float realWindSpeed = _parseFloatFromBytes(bGasBuffer, 78, true, true);
    // float snowThick = _parseFloatFromBytes(bGasBuffer, 82, true, true);
    // float uv = _parseFloatFromBytes(bGasBuffer, 86, true, true);
    AWSData.pm10 = _parseFloatFromBytes(bGasBuffer, 90, true, true);
    AWSData.pm1_0 = _parseFloatFromBytes(bGasBuffer, 94, true, true);
    // Serial.print("Wind Dir: ");
    // Serial.println(AWSData.windDir);
    // Serial.print("Wind Speed: ");
    // Serial.println(AWSData.windSpeed);
    // Serial.print("Temp: ");
    // Serial.println(AWSData.temp);
    // Serial.print("Humid: ");
    // Serial.println(AWSData.humid);
    // Serial.print("Rainfall acc.: ");
    // Serial.println(AWSData.rainfallAcc);
    // Serial.print("Dust: ");
    // Serial.println(AWSData.dust);
    // Serial.print("Radiation: ");
    // Serial.println(AWSData.radiation);
    // Serial.print("PM1.0: ");
    // Serial.println(AWSData.pm1_0);
    // Serial.print("PM10: ");
    // Serial.println(AWSData.pm10);
  }
  else
  {
    AWSData.windDir = -1.0;
    AWSData.windSpeed - 1.0;
    AWSData.temp = -1.0;
    AWSData.humid = -1.0;
    AWSData.rainfallAcc = -1.0;
    AWSData.dust = -1.0;
    AWSData.radiation = -1.0;
    AWSData.pm10 = -1.0;
    AWSData.pm1_0 = -1.0;
  }
  memset(awsBuffer, 0, sizeof(awsBuffer));
  memset(bGasBuffer, '\0', sizeof(bGasBuffer));
  return AWSData;
}

SHT20 MyModbusSensor::readSHT20()
{
  uint16_t shtBuffer[8];
  byte bShtBuffer[8];

  // Read the 4-byte data from Modbus slave - Address 4
  uint8_t result = _node.readInputRegisters(1, 2);

  // Check if Modbus request was successful
  if (result == _node.ku8MBSuccess)
  {
    // Print the received data
    for (int i = 0; i < 2; i++)
    {
      uint16_t value = _node.getResponseBuffer(i);
      shtBuffer[i] = value;
    }
    _convertUInt16ToByte(shtBuffer, bShtBuffer, 8);

    // Extract temp value (4 bytes starting from index 0)
    SHT20Data.SHT20Temp = (_parseFloatFromBytes(bShtBuffer, 0, false))/10;

    // Extract humid value (4 bytes starting from index 2)
    SHT20Data.SHT20Humid = (_parseFloatFromBytes(bShtBuffer, 2, false))/10;
  }
  else
  {
    SHT20Data.SHT20Temp = -1.0;
    SHT20Data.SHT20Humid = -1.0;
  }
  memset(shtBuffer, 0, sizeof(shtBuffer));
  memset(bShtBuffer, '\0', sizeof(bShtBuffer));
  return SHT20Data;
}

TempSensor MyModbusSensor::readTempSensor()
{
  uint16_t tempSensorBuffer[8];
  byte bTempSensorBuffer[8];

  // Read the 4-byte data from Modbus slave - Address 4
  uint8_t result = _node.readHoldingRegisters(0, 2);

  // Check if Modbus request was successful
  if (result == _node.ku8MBSuccess)
  {
    // Print the received data
    for (int i = 0; i < 2; i++)
    {
      uint16_t value = _node.getResponseBuffer(i);
      tempSensorBuffer[i] = value;
    }
    _convertUInt16ToByte(tempSensorBuffer, bTempSensorBuffer, 8);

    // Extract temp value (4 bytes starting from index 0)
    TempSensorData.TempSensorTemp = (_parseFloatFromBytes(bTempSensorBuffer, 0, false))/10;

    // Extract humid value (4 bytes starting from index 2)
    TempSensorData.TempSensorHumid = (_parseFloatFromBytes(bTempSensorBuffer, 2, false))/10;
  }
  else
  {
    TempSensorData.TempSensorTemp = -1.0;
    TempSensorData.TempSensorHumid = -1.0;
  }
  memset(tempSensorBuffer, 0, sizeof(tempSensorBuffer));
  memset(bTempSensorBuffer, '\0', sizeof(bTempSensorBuffer));
  return TempSensorData;
}

float MyModbusSensor::readGasFlowSensor()
{
  uint16_t gasFlowBuffer[8];
  byte bGasFlowBuffer[8];
  // Read the 4-byte data from Modbus slave - Address 4
  float gf;
  uint8_t result = _node.readHoldingRegisters(3, REGISTER_COUNT_4);

  // Check if Modbus request was successful for Slave 2
  if (result == _node.ku8MBSuccess)
  {
    // Print the received data
    for (int i = 0; i < REGISTER_COUNT_4; i++)
    {
      uint16_t value = _node.getResponseBuffer(i);
      gasFlowBuffer[i] = value;
    }
    _convertUInt16ToByte(gasFlowBuffer, bGasFlowBuffer, 8);
    gf = _parseFloatFromBytes(bGasFlowBuffer, 0);
  }
  else
  {
    // Print an error message for Slave 2
    gf = -1.0;
  }
  memset(gasFlowBuffer, 0, sizeof(gasFlowBuffer));
  memset(bGasFlowBuffer, '\0', sizeof(bGasFlowBuffer));
  return gf;
}

void MyModbusSensor::TCSetup(uint8_t tcID)
{
  _mb.begin(9600, SERIAL_8N1);
  _mb.setTimeout(2000);
  /* communicate with Modbus slave ID 1 over Serial (port 0) */
  _node.begin(tcID, _mb);
}

float MyModbusSensor::readTC()
{
  float fData;
  
  /* slave: read (1) 16-bit registers starting at register 0x3E8 to RX buffer */
  uint8_t result = _node.readInputRegisters(0x3E8, 1);

  /* do something with data if read is successful */
  if (result == _node.ku8MBSuccess)
  {
    uint16_t dataBuffer = _node.getResponseBuffer(0);
    fData = (dataBuffer * 0.1);
  }
  else
  {
    fData = -1.0;
  }
  return fData;
}

VEGA_C23 MyModbusSensor::readVegaC23()
{
  uint16_t ulBuffer[8];
  byte bUlBuffer[8];

  // Read the 2-byte data from Modbus slave - Address 0
  uint8_t result = _node.readHoldingRegisters(2002, 8);

  // Check if Modbus request was successful
  if (result == _node.ku8MBSuccess)
  {
    // Print the received data
    for (int i = 0; i < 8; i++)
    {
      uint16_t value = _node.getResponseBuffer(i);
      ulBuffer[i] = value;
    }
    _convertUInt16ToByte(ulBuffer, bUlBuffer, 8);

    // Extract PV value (4 bytes starting from index 0)
    vega_c23_data.pv = _parseFloatFromBytes(bUlBuffer, 0, false);

    // Extract SV value (4 bytes starting from index 2)
    vega_c23_data.sv = _parseFloatFromBytes(bUlBuffer, 2, false);

    // Extract TV value (4 bytes starting from index 4)
    vega_c23_data.tv = _parseFloatFromBytes(bUlBuffer, 4, false);

    // Extract QV value (4 bytes starting from index 6)
    vega_c23_data.qv= _parseFloatFromBytes(bUlBuffer, 8, false);
  }
  else
  {
    vega_c23_data.pv = -1.0;
    vega_c23_data.sv = -1.0;
    vega_c23_data.tv = -1.0;
    vega_c23_data.qv= -1.0;
  }
  memset(ulBuffer, 0, sizeof(ulBuffer));
  memset(bUlBuffer, '\0', sizeof(bUlBuffer));
  return vega_c23_data;
}
