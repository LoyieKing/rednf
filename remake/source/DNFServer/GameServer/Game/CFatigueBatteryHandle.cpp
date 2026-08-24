// CFatigueBatteryHandle.cpp —— ORIG 报告 docs/class_func_reports/CFatigueBatteryHandle/。
#include "CFatigueBatteryHandle.h"
#include <string.h>  // memset（stFatigueBatteryConstantTable_t ctor）

#include "CDataManager.h"
#include "CEnvironment.h"
#include "CStatisticServerProxy.h"
#include "CStreamGuard.h"
#include "CUser.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include "PacketHeader.h"
#include "GameTypes.h"

// ---- Packet_Fatigue_Battery_Money_Statistic（仅本 TU 使用）----
// ORIG C1 0x084a84ee：PacketHeader(0xc4a,0x11)；byte+0xa=0、int+0xb=0、short+0xf=0。
class Packet_Fatigue_Battery_Money_Statistic : public PacketHeader
{
public:
    Packet_Fatigue_Battery_Money_Statistic()
        : PacketHeader(0xc4a, 0x11), m_serverGroup(0), m_fieldB(0), m_grownupBuff(0)
    {
    }

    unsigned char m_serverGroup;   // +0xa
    unsigned int m_fieldB;         // +0xb
    unsigned short m_grownupBuff;  // +0xf
} __attribute__((packed));

// ORIG stFatigueBatteryConstantTable_t::stFatigueBatteryConstantTable_t() 0x0836f2cc
stFatigueBatteryConstantTable_t::stFatigueBatteryConstantTable_t()
{
    m_capLimit = 0;
    m_field2 = 0;
    m_field4 = 0;
    m_field8 = 0x28;
    m_field9 = 0;
    m_fieldc = 0;
    m_field10 = 0x46;
    memset(m_data11, 0, 0x191);              // this+0x12 起 0x191 字节
    m_data11[0x1a4 - 0x11] = 1;              // word +0x1a4 = 1
    m_data11[0x1a6 - 0x11] = 0;              // word +0x1a6 = 0
    m_data11[0x1a7 - 0x11] = 0;
    m_data11[0x1a8 - 0x11] = 0;              // word +0x1a8 = 0
    m_data11[0x1a9 - 0x11] = 0;
}

// ORIG 0x084a84b6：idx=level-1；idx<=0 返回 0；否则返回 *(u16*)((char*)this + 2 + idx*2)。
unsigned short stFatigueBatteryConstantTable_t::getLimitGrownupBuffCntFatigueBattery(
    int level) const
{
    int idx = level - 1;
    if (idx <= 0)
        return 0;
    return *reinterpret_cast<const unsigned short*>(
        reinterpret_cast<const char*>(this) + 2 + idx * 2);
}

// ORIG 0x84a8004：(float(rate)*specialCnt + 1.0) * (charge*level / divisor)，
// 四舍五入取整，超过表上限(+0x628)截断。返回 64 位标量。
long long CFatigueBatteryHandle::ComputeFatigueBatteryCharging(int charge, int level,
                                                               int specialCnt)
{
    unsigned char* mgr = reinterpret_cast<unsigned char*>(G_CDataManager());
    float rate = *reinterpret_cast<float*>(mgr + 0x62c);
    int divisor = *reinterpret_cast<unsigned char*>(mgr + 0x630);
    double value = static_cast<double>(static_cast<long long>(
        (rate * static_cast<float>(specialCnt) + 1.0f) *
        static_cast<float>((charge * level) / divisor)));
    int capped = static_cast<int>(value);
    if (*reinterpret_cast<unsigned short*>(mgr + 0x628) < static_cast<unsigned short>(capped))
        capped = *reinterpret_cast<unsigned short*>(mgr + 0x628);
    return capped;
}

// ORIG 0x84a8094：等级在 [mgr+0x631, mgr+0x638] 区间内不充能（返回 0）；
// 否则按 剩余疲劳 + DEFAULT_MAX_FATIGUE*v 计算充能并写 trace 日志。
short CFatigueBatteryHandle::ChargingFatigueBattery(CUser* user, int v)
{
    unsigned char* table = reinterpret_cast<unsigned char*>(G_CDataManager()) + 0x628;
    int level = user->get_charac_level();
    bool skip = false;
    if (static_cast<int>(table[9]) <= level && level == static_cast<int>(table[0x10]))
        skip = true;
    if (skip)
        return 0;

    int specialCnt = user->GetSpecialLevelCharacCnt(0x46);
    int remain = DEFAULT_MAX_FATIGUE - (user->getCurCharacUsedFatigue() & 0xffff);
    if (remain < 0)
        remain = 0;
    remain += DEFAULT_MAX_FATIGUE * v;
    level = user->get_charac_level();

    cMyTrace trace("short int CFatigueBatteryHandle::ChargingFatigueBattery(CUser*, int)", 0x32, 0);
    trace("charge=%d level=%d special=%d", remain, level, specialCnt);
    return static_cast<short>(
        ComputeFatigueBatteryCharging(remain, level, specialCnt));
}

// ORIG 0x84a829e：电池量 >= 表基准(mgr+0x7d0) 且开关(mgr+0x7cc)!=0 时，
// 计算 grown-up buff 上限并经统计代理发送 Packet_Fatigue_Battery_Money_Statistic；
// 返回本次消耗的电池量。
short CFatigueBatteryHandle::ActiveFatigueBattery_NotEndLevel(CUser* user)
{
    unsigned char* mgr = reinterpret_cast<unsigned char*>(G_CDataManager());
    int battery = user->GetFatigueBatteryCharging();
    short consumed = 0;
    if (static_cast<int>(*reinterpret_cast<unsigned short*>(mgr + 0x7d0)) <= battery &&
        *reinterpret_cast<short*>(mgr + 0x7cc) != 0)
    {
        int bonus = static_cast<int>(
            static_cast<unsigned int>(*reinterpret_cast<unsigned short*>(mgr + 0x7ce)) *
            battery) /
            static_cast<int>(*reinterpret_cast<unsigned short*>(mgr + 0x7cc));
        consumed = static_cast<short>(battery);
        int grownUpBuff = (user->getCurCharacFatigueGrownUpBuff() & 0xffff) + bonus;
        int level = user->get_charac_level();
        stFatigueBatteryConstantTable_t* table =
            reinterpret_cast<stFatigueBatteryConstantTable_t*>(mgr + 0x628);
        unsigned short limit = table->getLimitGrownupBuffCntFatigueBattery(level);
        if (static_cast<int>(limit) < grownUpBuff)
            grownUpBuff = limit;

        Packet_Fatigue_Battery_Money_Statistic packet;
        packet.m_serverGroup = static_cast<unsigned char>(G_CEnvironment()->get_server_group());
        packet.m_grownupBuff = static_cast<unsigned short>(grownUpBuff);
        CServerProxyMgr<CStatisticServerProxy>* mgrProxy = GlobalData::s_statistic_proxy_mgr;
        CStatisticServerProxy* proxy =
            mgrProxy->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(0));
        proxy->SendPacket(reinterpret_cast<char*>(&packet), 0x11);
        user->setCurCharacFatigueGrownUpBuff(static_cast<unsigned short>(grownUpBuff));
    }
    return consumed;
}

// ORIG 0x84a83da：等级 >= mgr+0x631 且 < mgr+0x638 时调用 NotEndLevel 分支。
short CFatigueBatteryHandle::ActiveFatigueBattery(CUser* user)
{
    unsigned char* mgr = reinterpret_cast<unsigned char*>(G_CDataManager());
    int result = 0;
    int level = user->get_charac_level();
    bool inRange =
        (static_cast<int>(mgr[0x631]) <= level) &&
        (level < static_cast<int>(mgr[0x638]));
    if (inRange)
        result = ActiveFatigueBattery_NotEndLevel(user);
    return static_cast<short>(result);
}
