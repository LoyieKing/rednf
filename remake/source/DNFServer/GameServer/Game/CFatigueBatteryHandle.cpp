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
    // b28d-20: ORIG 0x084a84b6 双守卫合并短路形——ORIG `jg fail; cmpl $0,level;
    // jg ok; fail: mov$0; jmp; ok: body` 的唯一 4.4 复现形是
    // `if (a > end || level <= 0) return 0;`（正极性 ||，fail 块 fall-through、
    // ok 块直落，无 goto 的 jmp/nop 中继）。探针 /tmp/p_fat1.cpp getD 19 条与
    // ORIG 逐条同序（jg fail / jg body），残 1 条 = 4.4 常量折叠
    // `(level-1)+8`→`lea 0x7(%eax)`（ORIG `lea -0x1`+`add $0x8` 两步），N1 封顶。
    if (level - 1 > static_cast<int>(m_endLevel10) || level <= 0)
        return 0;
    return m_limitTable[level - 1 + 8];
}

// ORIG 0x84a8004：
//   商 = (charge*level) / 零扩展除数字节（idivl；除数 0 时与 ORIG 同样触发 #DE）；
//   积 = (倍率*specialCnt + 1.0) * 商，全程 x87 80 位中间精度（flds/fild/fmulp 链）；
//   fistpll 以 RC=truncate 向零截断为 64 位，仅低 32 位参与比较与返回（ret 只带 eax）：
int CFatigueBatteryHandle::ComputeFatigueBatteryCharging(int charge, int level,
                                                         int specialCnt)
{
    // b28d-20: ORIG 0x084a8004 — sp 引用（add $0x68，字段 0x5c0/0x5c4/0x5c8）；
    // 无 long double 具名局部：ORIG x87 链 `fildl quotient; fildl specialCnt;
    // flds rate; fmulp; fld1; faddp; fmulp` 即 quotient 先压栈的
    // `quotient * (specialCnt*rate + 1.0L)` 内联实参形（P243 书写序定 flds 序：
    // 先 fildl specialCnt 再 flds rate，故 specialCnt 写在乘法左侧）。
    // 残余 ndiff 16 封顶 N82：①帧 0x48 vs ORIG 0x38、long long truncated 槽方向
    // 相反（4.4 -0x18 浅于 sp/-0x20；ORIG -0x28 深于 sp/-0x10；声明序两变体
    // truncated-first/后置实测槽位集不可翻，P112/N98）；②ORIG 尾
    // `mov -0x28(%ebp),%eax; mov -0x24(%ebp),%edx` 64 位读，4.4 对
    // `(unsigned int)long long` 恒只读低 32（跨版本截断读）。quotient/-0x1c、
    // low/-0xc、除数临时/-0x2c 语义位点与 ORIG 逐条同序。
    ServerParameterScript& sp = G_CDataManager()->m_serverParameter;

    int quotient = (charge * level) /
                   static_cast<int>(static_cast<unsigned int>(sp.m_fatigueTable5c0.m_chargeDivisor8));

    long long truncated = static_cast<long long>(
        static_cast<long double>(quotient) *
        (static_cast<long double>(specialCnt) *
             static_cast<long double>(sp.m_fatigueTable5c0.m_chargeRate4) +
         1.0L));  // fistpll RC=truncate

    unsigned int low = static_cast<unsigned int>(truncated);
    if (static_cast<unsigned int>(sp.m_fatigueTable5c0.m_capLimit) < low)  // 无符号比较
        low = sp.m_fatigueTable5c0.m_capLimit;
    return static_cast<int>(low);
}

// ORIG 0x84a8094：
//   level < m_minChargeLevel9 或 level == m_endLevel10 → 不充能返回 0；
//   否则 remain = max(DEFAULT_MAX_FATIGUE-used,0) + DEFAULT_MAX_FATIGUE*v，
//   记录 trace 后返回 Compute(remain, level, specialCnt) 的截 short。
// 注意：used/alreadyFatigue/charNo/level 均按 ORIG 汇编次序读取并进入日志。
short CFatigueBatteryHandle::ChargingFatigueBattery(CUser* user, int v)
{
    // b28d-20: ORIG 0x084a8094 — 守卫为 `!(A && B)` 形（探针 /tmp/p_fat3.cpp chE
    // 逐条复现 ORIG：`jl <mov $1>` + `jne <mov $0>` + `test %al; je 主流程`，
    // 零栈槽 bool 物化）；A = get_charac_level() >= min（call 在左，字段后读，
    // ORIG `call; movzbl 0x5c9(%edx)`）、B = get_charac_level() != end。
    // sp 引用（add $0x68）存 -0x28 槽。trace 用 P227 全表达式临时链：实参
    // traceLevel/alreadyFatigue/charNo 无槽（ORIG 落 %esi/%edi/%ebx 跨 ctor 保活）。
    ServerParameterScript& sp = G_CDataManager()->m_serverParameter;

    if (!(user->get_charac_level() >=
              static_cast<int>(sp.m_fatigueTable5c0.m_minChargeLevel9) &&
          user->get_charac_level() !=
              static_cast<int>(sp.m_fatigueTable5c0.m_endLevel10)))
        return 0;

    int specialCnt = user->GetSpecialLevelCharacCnt(0x46);
    int used = static_cast<int>(user->getCurCharacUsedFatigue() & 0xffff);
    int remain = DEFAULT_MAX_FATIGUE - used;
    if (remain < 0)
        remain = 0;
    remain += DEFAULT_MAX_FATIGUE * v;

    // ORIG movswl %ax：GetFatigueBatteryCharging 返回 short，拓宽进日志。
    cMyTrace(__PRETTY_FUNCTION__, 0x32, 0)(
        "FATIGUE BATTERY : char no(%d), alreadyFatigue(%d), remainFatigue(%d), "
        "level(%d), endLevelCnt(%d), thatDayUsedFatigue(%d)",
        user->getCurCharacNo(),
        static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging())),
        remain, user->get_charac_level(), specialCnt, used);

    return static_cast<short>(
        ComputeFatigueBatteryCharging(remain, user->get_charac_level(), specialCnt));
}

// ORIG 0x84a829e：电池量 >= 表门槛(m_batteryThreshold1a8 == mgr+0x7d0) 且开关
// (m_activeSwitch1a4 == mgr+0x7cc) != 0 时，按倍率计算 grown-up buff 增量、
// 以等级上限截断，经统计代理发送 Packet_Fatigue_Battery_Money_Statistic 并回写；
// 返回本次消耗的电池量（未激活时 0）。
short CFatigueBatteryHandle::ActiveFatigueBattery_NotEndLevel(CUser* user)
{
    // b28d-20: ORIG 0x084a829e — consumed=0 最先（movl $0,-0x20），battery 次
    //（-0x1c），sp 引用最后（-0x18，add $0x68）；守卫形 = 嵌套 if + B 否定早返
    //（探针 /tmp/p_fat4.cpp neF 逐条复现 ORIG：A 假 `jg end` 直接落末尾 return，
    // B 真 `jne body`、B 假 `mov consumed; jmp end`、body 尾 fall-through 到 end）。
    // packet 构造/发送按 ORIG 次序，SendPacket 走
    // `s_statistic_proxy_mgr->GetServerProxy(0)->SendPacket(...)` 单语句链式
    //（P52：packet 地址入 %ebx 跨 GetServerProxy 保活）。
    int consumed = 0;
    int battery = static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging()));
    ServerParameterScript& sp = G_CDataManager()->m_serverParameter;

    if (static_cast<int>(sp.m_fatigueTable5c0.m_batteryThreshold1a8) <= battery)
    {
        if (static_cast<short>(sp.m_fatigueTable5c0.m_activeSwitch1a4) == 0)
            return static_cast<short>(consumed);

        // bonus = 倍率(u16)*battery / 开关(u16)；乘积取 32 位形态，除法有符号 idivl。
        // 倍率不做显式 (unsigned int) cast：ushort→int 自然提升令 4.4 产 ORIG 的
        // `movzwl 0x766(%eax),%eax; movzwl %ax,%eax; imul battery_slot,%eax` 形
        //（探针 /tmp/p_fat4.cpp mulD；加 cast 反产 `movzwl %ax,%edx` + reg-imul 3 条）。
        int bonus = static_cast<int>(
            sp.m_fatigueTable5c0.m_bonusMultiplier1a6 * battery) /
            static_cast<int>(sp.m_fatigueTable5c0.m_activeSwitch1a4);
        consumed = battery;

        int grownUpBuff =
            static_cast<int>(user->getCurCharacFatigueGrownUpBuff() & 0xffff) + bonus;
        unsigned short limit =
            sp.m_fatigueTable5c0.getLimitGrownupBuffCntFatigueBattery(
                user->get_charac_level());
        if (static_cast<int>(limit) < grownUpBuff)  // 有符号比较（jge）
            grownUpBuff = limit;

        Packet_Fatigue_Battery_Money_Statistic packet;
        packet.m_serverGroup = static_cast<unsigned char>(G_CEnvironment()->get_server_group());
        packet.m_grownupBuff = static_cast<unsigned short>(grownUpBuff);
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(0))
            ->SendPacket(reinterpret_cast<char*>(&packet), 0x11);
        user->setCurCharacFatigueGrownUpBuff(static_cast<unsigned short>(grownUpBuff));
    }
    return static_cast<short>(consumed);
}

// ORIG 0x84a83da：minLevel <= level 且 level < endLevel 时走 NotEndLevel 分支。
short CFatigueBatteryHandle::ActiveFatigueBattery(CUser* user)
{
    // b28d-20: ORIG 0x084a83da — sp = G_CDataManager()+0x68 存 -0xc 槽（ServerParameterScript&
    // 引用形，`add $0x68` 非折叠 +0x628）；两次 get_charac_level 结果均无栈槽（P0
    // 表达式右值直用，ORIG `call; cmp %eax,%ebx` 直接比较），min 字段先物化 %ebx 跨 call。
    // 守卫用合并 `&&`（P225：4.4 对 && 直产 `jg skip; jge skip` 两直跳；拆嵌套 if
    // 反产 setle/setl 物化，实测 40 vs ORIG 41 且多 4 条 setcc）。
    int result = 0;
    ServerParameterScript& sp = G_CDataManager()->m_serverParameter;
    if (static_cast<int>(sp.m_fatigueTable5c0.m_minChargeLevel9) <= user->get_charac_level() &&
        user->get_charac_level() < static_cast<int>(sp.m_fatigueTable5c0.m_endLevel10))
        result = ActiveFatigueBattery_NotEndLevel(user);  // ORIG 结果经 cwtl 截 short
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
    // [b28c-L29] ORIG 0x084a81f4：左值经 cwtl 拓宽后与 result 做 32 位 cmp
    // （`call; cwtl; cmp -0x14(%ebp),%eax`），右侧不做 short 截断。
    if (static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging())) !=
        result)
    {
        // [b28c-L29] P227：全表达式临时链令 op() 实参（getCurCharacNo）在 ctor
        // 前求值并以 %ebx 跨 ctor 保活（ORIG `call getCurCharacNo; mov %eax,%ebx`）。
        cMyTrace(__PRETTY_FUNCTION__, 0x77, 0)(
            "FATIGUE BATTERY ACTIVE ERR : char no(%d), usedFatigue(%d)",
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
