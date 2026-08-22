// CFatigueBatteryHandle.h
// Reconstruction of CFatigueBatteryHandle
// (ORIG _ZN21CFatigueBatteryHandle22ChargingFatigueBatteryEP5CUseri,
//       _ZN21CFatigueBatteryHandle20ActiveFatigueBatteryEP5CUser).
// NOTE: The constructor is provided by GlobalData.cpp and is NOT defined here
// (defining it would cause a duplicate symbol).
#ifndef GAME_CFatigueBatteryHandle_H
#define GAME_CFatigueBatteryHandle_H

class CUser;

class CFatigueBatteryHandle
{
public:
    // Constructor provided by GlobalData.cpp.

    short ChargingFatigueBattery(class CUser* user, int v);
    short ActiveFatigueBattery(class CUser* user);

private:
    char m_pad[8];
};

#endif  // GAME_CFatigueBatteryHandle_H