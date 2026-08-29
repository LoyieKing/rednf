// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG
// （_ZN19RentalMachinePcRoom23RentalMachinePcRoomImplC1/C2 @0x08271a80 W、
//  D1/D2 @0x08271d50 W，嵌套于 RentalMachinePcRoom）+ 定义提取自
// Game/GlobalData.cpp（RentalMachinePcRoom : IRentalMachine 的 m_impl 嵌套实现对象）。
// ORIG ctor：set<unsigned>@0x104 构造 + memset 前 0x104；D1 仅析构 set；总 0x11c。
// ⚠️ GlobalData.cpp 当前为宿主实现（类内嵌套定义）；接入时由宿主迁除 cpp 内重复段。
#ifndef DNF_GAME_RENTALMACHINEPCROOMIMPL_H_
#define DNF_GAME_RENTALMACHINEPCROOMIMPL_H_

#include <set>
#include <cstring>

class RentalMachinePcRoom
{
public:
    class RentalMachinePcRoomImpl
    {
    public:
        RentalMachinePcRoomImpl() { std::memset(m_data, 0, sizeof(m_data)); }
        ~RentalMachinePcRoomImpl() {}   // ORIG D1 @0x08271d50：仅析构 set
        char m_data[0x104];            // +0x00（日志路径等，未还原字段区）
        std::set<unsigned int> m_set;  // +0x104
    };  // 0x11c
};

#endif  // DNF_GAME_RENTALMACHINEPCROOMIMPL_H_
