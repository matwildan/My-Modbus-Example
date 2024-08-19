#ifndef INIT_MODBUS_H
#define INIT_MODBUS_H

#include "Arduino.h"

extern void initMbSensor(MyModbusSensor**& _s, const int* _id, int _num);
extern void deleteMbSensor(MyModbusSensor**& _s, int _num);

#endif /* SETUP_MAIN_H */