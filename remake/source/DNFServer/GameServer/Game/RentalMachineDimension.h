// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + GlobalData.cpp
// RentalMachineDimension：ORIG 符号族（全部 T/W，见 docs/class_func_reports/RentalMachineDimension/）。
// has_def=1：完整复制定义自 GlobalData.cpp:660（类）与 :654（RentalMachineDimensionImpl）。
// 注：GlobalData.cpp 内仍有同名定义，本头为首次独立 .h 声明；不改任何 cpp。
#ifndef DNF_GAME_RENTALMACHINEDIMENSION_H_
#define DNF_GAME_RENTALMACHINEDIMENSION_H_

#include <string.h>

#include <set>
#include "InstanceRentalSystem.h"   // IRentalMachine / RentInput / RentOutput

class CUser;
class EventRewardItemInfo;

// RentalMachineDimensionImpl（ORIG ctor 0x0826fa6c：与 PcRoom 同构，推断）。
struct RentalMachineDimensionImpl
{
    RentalMachineDimensionImpl() { memset(m_data, 0, sizeof(m_data)); }
    char m_data[0x104];            // +0x00
    std::set<unsigned int> m_set;  // +0x104
};  // 0x11c

class RentalMachineDimension : public IRentalMachine
{
public:
    RentalMachineDimension();          // ORIG T @0x0826eafc
    virtual ~RentalMachineDimension(); // ORIG D0 @0x0826ec04 / D1 @0x0826eba2

    // 槽2 Rent：ORIG T @0x0826ec22
    // 返回类型 ENUM_ERROR（ORIG prett @0x8bf9f60 直证；不入 mangled/布局）。
    virtual ENUM_ERROR Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
                            InstanceRentalSystem::RentOutput& output);
    // 槽3 Ready：ORIG T @0x0826ee94 = checkAllRentalItemScript() 尾调
    virtual bool Ready();
    // 槽4 CheckRentableItem：ORIG T @0x0826eea8 = m_impl->m_set.find(idx)!=end
    virtual bool CheckRentableItem(unsigned int itemIdx);

private:
    bool checkAllRentalItemScript();   // ORIG T @0x0826f022
    bool checkSelectionListScript(
        const std::vector<std::vector<EventRewardItemInfo> >& list);         // T @0x0826f14e
    int GetRentItemInfo(CUser& user,
                        const InstanceRentalSystem::RentInput::RentInElem& elem,
                        EventRewardItemInfo& out);                           // T @0x0826f842
    int GetRewardItemGiveList(int job, int growType);                        // T @0x0826f814（透传）
    int CheckupAllRentalItems(CUser& user,
                              const InstanceRentalSystem::RentInput& input); // T @0x0826f52c
    bool checkRentalLevel(int level);                                        // T @0x0826f7e4
    bool checkRentalGrowJob(int growType);                                   // T @0x0826f7fc
    void log(CUser& user, int itemId);                                       // T @0x0826ef16
    void initLoggingEnvironment();                                           // ORIG T @0x0826efdc
    RentalMachineDimensionImpl* m_impl;    // +0x04（ORIG auto_ptr）
};

#endif  // DNF_GAME_RENTALMACHINEDIMENSION_H_
