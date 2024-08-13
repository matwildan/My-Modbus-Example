/*
 * Created by: @matwildan
 * Created date: 22-06-2023
 */
#ifndef MY_MODBUS_SENSOR_H
#define MY_MODBUS_SENSOR_H

#include <Arduino.h>
#include <ModbusMaster.h>

struct SHT20{
  float SHT20Temp;
  float SHT20Humid;
};

struct TempSensor{
  float TempSensorTemp;
  float TempSensorHumid;
};

struct Slicer_Ultrasonic{
  float dist;
  float lin;
};

struct Conductivity{
  float ec;
  float temp;
  float salinity;
};

struct Gas{
  float pm;
  float co;
  float so2;
  float no;
};

struct VEGA_C23{
  float pv;
  float sv;
  float tv;
  float qv;
};

struct AWS{
  float windDir;
  float windSpeed;
  float temp;
  float humid;
  float rainfallAcc;
  float dust;
  float radiation;
  float pm1_0;
  float pm10;
};

struct PZEM_MODBUS{
  float voltage;
  float current;
  float power;
  float energy;
};

class MyModbusSensor {
  private:
    int _address;
    ModbusMaster _node;
    ModbusMaster nodeTC;
    
  public:

    /**
     * @brief My Modbus Sensor Instance
     * 
     * @param Modbus Address
     * 
    */
    MyModbusSensor(int address);

    /**
     * @brief Init My Modbus Sensor
     * 
    */
    void init();

    void end();

    /**
     * @brief Read conductivity sensor value
     * 
     * @return struct Conductivity Sensor
     * 
    */
    Conductivity readConductivitySensor();

    /**
     * @brief Read Gas sensor value
     * 
     * @return struct Gas Sensor
     * 
    */
    Gas readGasSensor();

    /**
     * @brief Read AWS value
     * 
     * @return struct AWS
     * 
    */
    AWS readAWS();

    /**
     * @brief Read SHT20 sensor value
     * 
     * @return struct SHT20 Sensor
     * 
    */
    SHT20 readSHT20();

    /**
     * @brief Read Probe Temp (modbus) sensor value
     * 
     * @return struct Probe Temp (modbus) Sensor
     * 
    */
    TempSensor readTempSensor();

    /**
     * @brief Read PZEM004T sensor value
     * 
     * @return struct PZEM004T Sensor
     * 
    */
    PZEM_MODBUS readPZEMSensor();

    /**
     * @brief Read Mux N4D1M
     * 
     * @param char array destination
     * 
    */
    void readMuxValue(char dest[]);

    /**
     * @brief Read level transmitter sensor value
     * 
     * @return float level transmitter value
     * 
    */
    float readLevelTransmitterSensor();

    /**
     * @brief Read pressure sensor value
     * 
     * @return float Pressure Sensor value
     * 
    */
    float readPressureSensor();

    /**
     * @brief Read Gas Flow sensor value
     * 
     * @return float Gas Flow Sensor value
     * 
    */
    float readGasFlowSensor();

    /**
     * @brief Setup TK4S
     * 
     * @param Modbus TK4S Address
     * 
    */
    void TCSetup(uint8_t tcID);

    /**
     * @brief Read TK4S sensor value
     * 
     * @return float Conductivity Sensor value
     * 
    */
    float readTC();

    /**
     * @brief Wellpro Modbus Control All DO
     * 
     * @return Vega C23 Sensor Value
     * 
    */
    VEGA_C23 readVegaC23();
};

#endif