// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/RentalMachinePcRoom/
// RentalMachinePcRoom：pc_room 租赁机（IRentalMachine 实现）。
// 定义源：Game/GlobalData.cpp:624-663（本批复制为头声明，实现仍在该 cpp，后续迁移）。
#ifndef DNF_GAME_RENTALMACHINEPCROOM_H_
#define DNF_GAME_RENTALMACHINEPCROOM_H_

#include <memory>
#include <set>

#include "InstanceRentalSystem.h"  // IRentalMachine / InstanceRentalSystem::RentInput / RentOutput

class CUser;

class RentalMachinePcRoom : public IRentalMachine
{
public:
    RentalMachinePcRoom();                                  // ORIG T @0x082703c0
    virtual ~RentalMachinePcRoom() {}                       // ORIG T @0x08270466 / W @0x082704c8

    // 槽2 Rent：ORIG T 0x082704e6（0x258 字节 pc_room 租赁事务链，报告 RentalMachinePcRoom/Rent.md）。
    // 返回类型 ENUM_ERROR（ORIG prett @0x8bfafe0 直证；不入 mangled/布局）。
    virtual ENUM_ERROR Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
                            InstanceRentalSystem::RentOutput& output);
    // 槽3 Ready：ORIG 0x0827073e = checkAllRentalItemScript() 尾调。
    virtual bool Ready();
    // 槽4 CheckRentableItem：ORIG 0x08270752 = m_impl->m_set.find(idx)!=end。
    virtual bool CheckRentableItem(unsigned int itemIdx);

private:
    // 嵌套实现对象（ORIG 嵌套类，C1 0x08271a80：set<unsigned>@0x104 构造 +
    // memset 前 0x104；总 0x11c）。
    struct RentalMachinePcRoomImpl
    {
        ~RentalMachinePcRoomImpl() {}  // ORIG D1 @0x08271d50：仅析构 set
        char m_data[0x104];            // +0x00（日志路径等，未还原字段区）
        std::set<unsigned int> m_set;  // +0x104
    };  // 0x11c
    bool checkAllRentalItemScript();   // ORIG T @0x082708cc
    void initLoggingEnvironment();     // ORIG T @0x08270886
    int CheckupAllRentalItems(CUser& user,
                              const InstanceRentalSystem::RentInput& input);  // ORIG T @0x08270d1e
    void log(CUser& user, int itemId);                                       // ORIG T @0x082707c0
    std::auto_ptr<RentalMachinePcRoomImpl> m_impl;  // +0x04（ORIG auto_ptr<RentalMachinePcRoomImpl>）
};

#endif  // DNF_GAME_RENTALMACHINEPCROOM_H_
