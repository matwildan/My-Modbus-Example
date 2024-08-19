# Modbus Sensor Interface with Dynamic Sensor Management in C++

A C++ project developed using PlatformIO that interfaces for multiple Modbus sensors, dynamically manages sensor objects using double pointers, and outputs sensor data in JSON format to the serial monitor.

## Features
- Modbus Sensor Initialization: The code initializes an array of Modbus sensors with specific addresses.
- Sensor Reading: Periodically reads modbus values from the sensors.
- Data Formatting: Outputs the sensor data as a JSON string to the serial monitor.
- Modular Design: Easy to modify for additional sensors or different sensor types by adjusting the modbus address (e.g. **prID[]**) array and sensor polling function (e.g. **pressurePolling()**).

## Dependencies
- ModbusMaster Library: This project depends on the ModbusMaster library for Modbus communication. The library version used is 4-20ma/ModbusMaster@^2.0.1. [4-20ma/ModbusMaster](https://github.com/4-20ma/ModbusMaster)
- Ensure you include the following in your platformio.ini file:
```javascript
lib_deps =
    4-20ma/ModbusMaster@^2.0.1

```
- Also, include any additional libraries like **MyModbusSensor** and **InitModbus** as required.
## Usage/Examples

Include Libraries:
```cpp
#include <Arduino.h>
#include "MyModbusSensor.h"
#include "InitModbus.h"
```

Global variables example:
```cpp
// ===== Pressure Parameters =====
int prID[] = {4, 5, 6}; // Assign the address of Modbus
int numPr = sizeof(prID)/sizeof(int); // Assign the number of modbus sensor(s)
MyModbusSensor** pressure; 
```

Modbus sensor polling example:
```cpp
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
```

On setup():
```cpp
void setup() {
  // put your setup code here, to run once:
  initMbSensor(pressure,prID,numPr);
}
```

On loop():
```cpp
void loop() {
  // put your main code here, to run repeatedly:
  pressurePolling(pressure, prID, numPr);

  /* Delete when you are done */
  // deleteMbSensor(pressure, numPr);
}
``````
## Explanation of the Double Pointer (MyModbusSensor** pressure)
### What is a Double Pointer?
A double pointer is a pointer to a pointer. In this context, '**MyModbusSensor\*\* pressure**' is a pointer to an array of pointers, where each pointer in the array points to an instance of the **MyModbusSensor** class.

### Why Use a Double Pointer?
The double pointer '**MyModbusSensor\*\* pressure**' is used to manage an array of MyModbusSensor objects dynamically. This approach is particularly useful when:

- The number of sensors **('numPr')** is not known at compile time.
- The sensors need to be managed dynamically, such as allocating or deallocating them at runtime.

### How the Double Pointer Works in the Code
1. Initialization:
  * The '**pressure**' variable is passed to the '**initMbSensor**' function, which allocates memory for an array of '**MyModbusSensor**' pointers and initializes each sensor.
  * The double pointer allows the function to modify the original pointer array, so that the '**pressure**' variable in the main code points to the correctly initialized sensors.
  ```cpp
  void setup() {
    initMbSensor(pressure, prID, numPr);
}
  ```
2. Accessing Sensor Data:
- In the '**pressurePolling**' function, the double pointer allows access to each **MyModbusSensor** object through '**pressure[i]**'.
```cpp
float prVal = (*pressure[i]).readPressureSensor();
```
- '**pressure[i]**' gives the pointer to the '**MyModbusSensor**' object at index i.
- '**(*pressure[i])**' dereferences this pointer to access the actual object, allowing you to call the '**readPressureSensor()**' method.

3. Memory Management:
- Using a double pointer facilitates dynamic memory management, as you can allocate or deallocate memory for the sensor objects as needed. This is crucial in embedded systems where memory is limited.
```cpp
// deleteMbSensor(pressure, numPr); /* Uncomment this to deallocate object */
```

## References

- [4-20ma / ModbusMaster](https://github.com/4-20ma/ModbusMaster) Library

## 🔗 Links
[![linkedin](https://img.shields.io/badge/linkedin-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/ahmad-wildan-160798/)

