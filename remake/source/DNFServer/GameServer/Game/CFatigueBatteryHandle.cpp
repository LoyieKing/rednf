// CFatigueBatteryHandle.cpp —— ORIG 报告 docs/class_func_reports/CFatigueBatteryHandle/。
//
// 布局证明（本 TU 内静态断言）：ORIG 全部裸偏移基于 G_CDataManager()+0x68 起点，
// 对应本工程 CDataManager::m_serverParameter(+0x68)；表内 +0x5c0 即
// ServerParameterScript::m_fatigueTable5c0，绝对偏移 0x68+0x5c0 == 0x628。
// NotEndLevel 的 mgr+0x7cc/0x7ce/0x7d0 落在同一表的 +0x1a4/+0x1a6/+0x1a8。
#include "CFatigueBatteryHandle.h"
#include <stddef.h>  // offsetof（布局断言）
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

// 布局等价证明：m_serverParameter 必须在 CDataManager+0x68（表成员自身偏移已由
// ServerParameterScript.h 的 static_assert 锁定为 +0x5c0）。
static_assert(offsetof(CDataManager, m_serverParameter) == 0x68,
              "CDataManager::m_serverParameter drift: fatigue battery table must sit at +0x628");

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
// 指令序：word+0x1a4=1 → word+0x1a6=0 → word+0x1a8=0 → memset(this+0x12,0,0x191)
// （词写与 memset 区间不相交，顺序保持 ORIG 形态）。+0x11 与 +0x1aa 保持未初始化。
stFatigueBatteryConstantTable_t::stFatigueBatteryConstantTable_t()
{
    m_capLimit = 0;                        // movw $0x0,(%eax)
    m_chargeRate4 = 0.0f;                  // movl $0x0,0x4(%eax)（float 槽位清零）
    m_chargeDivisor8 = 0x28;               // movb $0x28,0x8(%eax)
    m_minChargeLevel9 = 0;                 // movb $0x0,0x9(%eax)
    m_fieldc = 0;                          // movl $0x0,0xc(%eax)
    m_endLevel10 = 0x46;                   // movb $0x46,0x10(%eax)
    m_activeSwitch1a4 = 1;                 // movw $0x1,0x1a4(%eax)
    m_bonusMultiplier1a6 = 0;              // movw $0x0,0x1a6(%eax)
    m_batteryThreshold1a8 = 0;             // movw $0x0,0x1a8(%eax)
    memset(&m_data11[1], 0, 0x191);        // this+0x12 起 0x191 字节
}

// ORIG 0x084a84b6：
//   idx = level-1；idx > (int)m_endLevel10（字节零扩展）→ 返回 0；
//   level <= 0 → 返回 0；
//   否则返回 *(u16*)(&m_field2 + idx + 8)，与 ORIG 的
//   movzwl 0x2(this,idx+8,2) 同址。
unsigned short stFatigueBatteryConstantTable_t::getLimitGrownupBuffCntFatigueBattery(
    int level) const
{
    int idx = level - 1;
    if (idx > static_cast<int>(m_endLevel10))
        return 0;
    if (level <= 0)
        return 0;
    return *reinterpret_cast<const unsigned short*>(&m_field2 + idx + 8);
}

// ORIG 0x84a8004：
//   商 = (charge*level) / 零扩展除数字节（idivl；除数 0 时与 ORIG 同样触发 #DE）；
//   积 = (倍率*specialCnt + 1.0) * 商，全程 x87 80 位中间精度（flds/fild/fmulp 链）；
//   fistpll 以 RC=truncate 向零截断为 64 位，仅低 32 位参与比较与返回（ret 只带 eax）：
int CFatigueBatteryHandle::ComputeFatigueBatteryCharging(int charge, int level,
                                                         int specialCnt)
{
    stFatigueBatteryConstantTable_t& table =
        G_CDataManager()->m_serverParameter.m_fatigueTable5c0;

    int quotient = (charge * level) /
                   static_cast<int>(static_cast<unsigned int>(table.m_chargeDivisor8));

    long double product =
        (static_cast<long double>(table.m_chargeRate4) *
             static_cast<long double>(specialCnt) +
         1.0L) *
        static_cast<long double>(quotient);

    long long truncated = static_cast<long long>(product);  // fistpll RC=truncate
    unsigned int low = static_cast<unsigned int>(
        static_cast<unsigned long long>(truncated));
    if (static_cast<unsigned int>(table.m_capLimit) < low)  // 无符号比较
        low = table.m_capLimit;
    return static_cast<int>(low);
}

// ORIG 0x84a8094：
//   level < m_minChargeLevel9 或 level == m_endLevel10 → 不充能返回 0；
//   否则 remain = max(DEFAULT_MAX_FATIGUE-used,0) + DEFAULT_MAX_FATIGUE*v，
//   记录 trace 后返回 Compute(remain, level, specialCnt) 的截 short。
// 注意：used/alreadyFatigue/charNo/level 均按 ORIG 汇编次序读取并进入日志。
short CFatigueBatteryHandle::ChargingFatigueBattery(CUser* user, int v)
{
    stFatigueBatteryConstantTable_t& table =
        G_CDataManager()->m_serverParameter.m_fatigueTable5c0;

    int level = user->get_charac_level();
    if (static_cast<int>(table.m_minChargeLevel9) > level)
        return 0;                          // ORIG jl → al=1（低于最低等级不充能）
    level = user->get_charac_level();      // ORIG 二次调用 get_charac_level
    if (level == static_cast<int>(table.m_endLevel10))
        return 0;                          // ORIG 相等落入 al=1（满级不充能）


    int specialCnt = user->GetSpecialLevelCharacCnt(0x46);
    int used = static_cast<int>(user->getCurCharacUsedFatigue() & 0xffff);
    int remain = DEFAULT_MAX_FATIGUE - used;
    if (remain < 0)
        remain = 0;
    remain += DEFAULT_MAX_FATIGUE * v;

    int traceLevel = user->get_charac_level();
    // ORIG movswl %ax：getter 返回 ax 后按 short 符号扩展进日志。
    int alreadyFatigue =
        static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging()));
    int charNo = user->getCurCharacNo();

    cMyTrace trace(__PRETTY_FUNCTION__,
                   0x32, 0);
    trace("FATIGUE BATTERY : char no(%d), alreadyFatigue(%d), remainFatigue(%d), "
          "level(%d), endLevelCnt(%d), thatDayUsedFatigue(%d)",
          charNo, alreadyFatigue, remain, traceLevel, specialCnt, used);

    level = user->get_charac_level();  // ORIG Compute 前再次读取
    return static_cast<short>(ComputeFatigueBatteryCharging(remain, level, specialCnt));
}

// ORIG 0x84a829e：电池量 >= 表门槛(m_batteryThreshold1a8 == mgr+0x7d0) 且开关
// (m_activeSwitch1a4 == mgr+0x7cc) != 0 时，按倍率计算 grown-up buff 增量、
// 以等级上限截断，经统计代理发送 Packet_Fatigue_Battery_Money_Statistic 并回写；
// 返回本次消耗的电池量（未激活时 0）。
short CFatigueBatteryHandle::ActiveFatigueBattery_NotEndLevel(CUser* user)
{
    stFatigueBatteryConstantTable_t& table =
        G_CDataManager()->m_serverParameter.m_fatigueTable5c0;

    // ORIG cwtl：short 符号扩展后参与有符号比较与乘除。
    int battery = static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging()));
    int consumed = 0;
    if (static_cast<int>(table.m_batteryThreshold1a8) <= battery &&
        static_cast<short>(table.m_activeSwitch1a4) != 0)
    {
        // bonus = 倍率(u16)*battery / 开关(u16)；乘积取 32 位形态，除法有符号 idivl。
        int bonus = static_cast<int>(
            static_cast<unsigned int>(table.m_bonusMultiplier1a6) * battery) /
            static_cast<int>(table.m_activeSwitch1a4);
        consumed = battery;

        int grownUpBuff =
            static_cast<int>(user->getCurCharacFatigueGrownUpBuff() & 0xffff) + bonus;
        int level = user->get_charac_level();
        unsigned short limit =
            table.getLimitGrownupBuffCntFatigueBattery(level);
        if (static_cast<int>(limit) < grownUpBuff)  // 有符号比较（jge）
            grownUpBuff = limit;

        Packet_Fatigue_Battery_Money_Statistic packet;
        packet.m_serverGroup = static_cast<unsigned char>(G_CEnvironment()->get_server_group());
        packet.m_grownupBuff = static_cast<unsigned short>(grownUpBuff);
        CServerProxyMgr<CStatisticServerProxy>* proxyMgr = GlobalData::s_statistic_proxy_mgr;
        CStatisticServerProxy* proxy =
            proxyMgr->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(0));
        proxy->SendPacket(reinterpret_cast<char*>(&packet), 0x11);
        user->setCurCharacFatigueGrownUpBuff(static_cast<unsigned short>(grownUpBuff));
    }
    return static_cast<short>(consumed);
}

// ORIG 0x84a83da：minLevel <= level 且 level < endLevel 时走 NotEndLevel 分支。
short CFatigueBatteryHandle::ActiveFatigueBattery(CUser* user)
{
    stFatigueBatteryConstantTable_t& table =
        G_CDataManager()->m_serverParameter.m_fatigueTable5c0;
    int result = 0;
    int level = user->get_charac_level();
    if (static_cast<int>(table.m_minChargeLevel9) <= level)   // ORIG jg 跳过
    {
        level = user->get_charac_level();                     // ORIG 二次调用
        if (level < static_cast<int>(table.m_endLevel10))     // ORIG jge 跳过
            result = ActiveFatigueBattery_NotEndLevel(user);  // ORIG 结果经 cwtl 截 short
    }
    return static_cast<short>(result);
}

// ORIG 0x84a81f4：used=(int)(short)GetFatigueBatteryCharging()；result=used；
// 若二次读取与 result 不等则记 trace "FATIGUE BATTERY ACTIVE ERR ..." 并重取。
// ORIG 另有 call G_CDataManager(); add $0x68,%eax 死存储（结果从不读取），按原样保留。
short CFatigueBatteryHandle::ActiveFatigueBattery_EndLevel(CUser* user)
{
    int result = 0;
    int used = static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging()));

    ServerParameterScript& unusedScript = G_CDataManager()->m_serverParameter;  // ORIG 死存储
    (void)unusedScript;

    result = used;
    if (static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging())) !=
        static_cast<short>(result))
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x77, 0);
        trace("FATIGUE BATTERY ACTIVE ERR : char no(%d), usedFatigue(%d)",
              user->getCurCharacNo(), result);
        result = static_cast<int>(
            static_cast<short>(user->GetFatigueBatteryCharging()));
    }
    return static_cast<short>(result);
}

// ORIG 0x84a7ff8 / 0x84a7ffe：均为纯 ret 空体（W 弱符号）。
// （此前 ctor 定义暂置于 GlobalData.cpp，现随本 TU 统一收口。）
CFatigueBatteryHandle::CFatigueBatteryHandle() {}
CFatigueBatteryHandle::~CFatigueBatteryHandle() {}
