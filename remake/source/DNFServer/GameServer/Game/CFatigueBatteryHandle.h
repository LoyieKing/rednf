// CFatigueBatteryHandle.h
// Reconstruction of CFatigueBatteryHandle
// (ORIG _ZN21CFatigueBatteryHandle22ChargingFatigueBatteryEP5CUseri 0x84a8094,
//       _ZN21CFatigueBatteryHandle20ActiveFatigueBatteryEP5CUser   0x84a83da).
#ifndef GAME_CFatigueBatteryHandle_H
#define GAME_CFatigueBatteryHandle_H

class CUser;

// 疲劳电池常数表 —— 内嵌于 CDataManager +0x628（推断：Compute/NotEndLevel 的
// 裸偏移 +0x628..+0x638 与 NotEndLevel 的 (mgr+0x68)+0x5c0 同一起点）。
struct stFatigueBatteryConstantTable_t
{
    stFatigueBatteryConstantTable_t();              // ORIG 0x0836f2cc W

    unsigned short getLimitGrownupBuffCntFatigueBattery(int level) const;  // ORIG 0x084a84b6 W

    unsigned short m_capLimit;    // +0x00（充电量上限）
    unsigned short m_field2;      // +0x02（按等级索引 ushort 数组起点）
    int m_field4;                 // +0x04
    unsigned char m_field8;       // +0x08（ORIG 初始化 0x28）
    unsigned char m_field9;       // +0x09
    int m_fieldc;                 // +0x0c
    unsigned char m_field10;      // +0x10（ORIG 初始化 0x46）
    // +0x11..+0x1ab：ORIG memset(this+0x12,0,0x191) 后写 word +0x1a4=1、
    // +0x1a6=0、+0x1a8=0；本数组下标 = 偏移 - 0x11。
    unsigned char m_data11[0x19b];
};

static_assert(sizeof(stFatigueBatteryConstantTable_t) == 0x1ac,
              "stFatigueBatteryConstantTable_t ABI drift (ORIG size 0x1ac)");

class CFatigueBatteryHandle
{
public:
    CFatigueBatteryHandle();  // ORIG 0x84a7ff8（W 弱符号，合法空体）
    short ChargingFatigueBattery(class CUser* user, int v);   // ORIG 0x84a8094
    short ActiveFatigueBattery(class CUser* user);            // ORIG 0x84a83da
    long long ComputeFatigueBatteryCharging(int charge, int level, int specialCnt);  // ORIG 0x84a8004
    short ActiveFatigueBattery_NotEndLevel(class CUser* user);  // ORIG 0x84a829e
    short ActiveFatigueBattery_EndLevel(class CUser* user);   // ORIG 0x84a81f4（CUser::_onLevelUp level 0x55 调用）

private:
    char m_pad[8];
};

#endif  // GAME_CFatigueBatteryHandle_H
