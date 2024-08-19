#include "MyModbusSensor.h"
#include "InitModbus.h"

void initMbSensor(MyModbusSensor**& _s, const int* _id, int _num) {
    _s = new MyModbusSensor*[_num];

    for (int i = 0; i < _num; ++i) {
        _s[i] = new MyModbusSensor(_id[i]);
    }

    for (int i = 0; i < _num; ++i) {
        _s[i]->init();
    }
}

void deleteMbSensor(MyModbusSensor**& _s, int _num) {
    // Delete each MyModbusSensor object
    for (int i = 0; i < _num; ++i) {
        delete _s[i];
    }

    // Delete the array of pointers
    delete[] _s;

    _s = nullptr;  // Set the pointer to nullptr to avoid dangling pointer
}