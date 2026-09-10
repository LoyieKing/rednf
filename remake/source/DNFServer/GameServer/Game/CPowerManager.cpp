// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPowerManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <cstring>
#include <cstdio>
#include "CPowerManager.h"
#include "SigTypes.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CEnvironment.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "CGameManager.h"

#include "CGuildServerProxy.h"
#include "CServerProxyMgr.h"
#include "PacketGuard.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "CPrivateStoreMgr.h"
#include "CSharedServerMessageManager.h"
// [b28d-solo] SIG_LOAD_POWER_WAR_STATUE_INFO（0x80）载荷具名视图（P80/P99）：
// 偏移逐字段依 ORIG SetPowerWarUserStatueInfo 反汇编（0x0847ed4e 段）实证；
// +0x4d..+0x4f 为 ORIG 未访问空洞（显式 pad 表达布局，不参与写入）。
#pragma pack(push, 1)
struct PowerWarStatueView
{
    unsigned int m_characNo;   // +0x00
    char m_name[0x1e];         // +0x04
    char m_job;                // +0x22（与目标 m_rankings[].m_job 同型，P211 档③）
    char m_level;              // +0x23
    char m_grow;               // +0x24
    char m_guildName[0x28];    // +0x25
    char m_pad4d[3];           // +0x4d..+0x4f
    int m_guildId;             // +0x50
    int m_equipment[11];       // +0x54（0x2c）
};
#pragma pack(pop)
// ---- CPowerManager（PowerWar 管理器，ORIG 0x0847e998 域，布局 0x208）----
// report: docs/class_func_reports/CPowerManager.md。对象为 GlobalData.cpp
// 的 m_pad[0x208]（offset 布局一致：+0x00 winnerSide / +0x04/+0x08 分数 /
// +0x0c/+0x10 频道分数 / +0x14/+0x18 阵营人数 / +0x28..+0x1b4 雕像排行 /
// +0x1b4 CPowerWarLog / +0x1f0 CPowerWarPacketLog）。
#include "CPowerManager/CPowerManager.inc"
    : m_winnerSide(0), m_side1Point(0), m_side2Point(0),
      m_side1ChannelPoint(0), m_side2ChannelPoint(0),
      m_side1UserCount(0), m_side2UserCount(0), m_field1c(0)
{
    // ORIG 0x0847e998：初始化列表按声明序清 +0..+1c（不写 +0x20）；编译器按
    // 声明序前置成员构造——m_rankings[3] 循环调 STPowerWarStatueRankerInfo::C1、
    // CPowerWarLog::C1、CPowerWarPacketLog::C1（与 ORIG 顺序一致）；
    // 函数体再 memset (+0x24,4)、memset (+0x28,0x18c)，最后 Reset 两日志子对象。
    memset(m_powerWarStartTime, 0, sizeof(m_powerWarStartTime));
    memset(m_rankings, 0, sizeof(m_rankings));
    m_powerWarLog.Reset();
    m_powerWarPacketLog.Reset();
}

#include "CPowerManager/IsPowerWarEventOn.inc"
{
    // ORIG 0x0847f910：重复事件 0x1e（PowerWar）经 vtable+0x34 判活，无空指针检查。
    // ORIG 尾 `test %al,%al; je; mov$1; jmp; mov$0` 分支形态（P56），直 return
    // 表达式被 4.4 折叠。改 if/return true;false 保留 ORIG CFG。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x1e)->IsEventing(0) != 0)
        return true;
    return false;
}



#include "CPowerManager/GetPowerWarResponPenalty.inc"
{
    // ORIG 0x08480ec8：penalty 初始 1.0；混沌时间 < 阈值时二次读取
    // GetCurCharacChaosStateTime / G_CDataManager 计算 2.0 - chaos/threshold。
    float penalty = 1.0f;
    if ((unsigned int)user->GetCurCharacChaosStateTime() <
        (unsigned int)G_CDataManager()->m_powerParameter.m_penaltyThreshold)
    {
        // [b28c-L9] ORIG 0x08480f03：`mov $0,%edx; mov %eax,-0x20; mov %edx,-0x1c;
        // fildll` = unsigned int→float 的 64 位零扩展路径（P196 符号性）。
        penalty = 2.0f -
                  (float)(unsigned long long)(unsigned int)user->GetCurCharacChaosStateTime() /
                      (float)G_CDataManager()->m_powerParameter.m_penaltyThreshold;
    }
    return penalty;
}

// [b28d-solo-CPowerManager] NEAR(18=18, gap 0) | ORIG 0x08480e92 | 语义全等
// （fildl/fmuls/fistpl 同序、指令数一致），残差仅槽号：ghostTime -0x10、cw
// -0x1a/-0x1c vs ORIG -0x1c/-0x1e/-0x20 ⇒ N82/P112 封顶。本轮穷举六族：
// double framePad 前置/后置、int[3]/char pad、3×dead int、long double、union
// {struct{int;int[3];};int;} 标量视图形、n-first 声明序——4.4 把未使用局部沉到
// 帧底、使用的标量按声明序反向分配，源槽恒 -0x10/-0xc，无法移到 ORIG 的 -0x1c
// （4.1 为不复用临时槽预留 12B 死槽；视图形虽命中 -0x1c 但 fildl 前多一次 spill）。
#include "CPowerManager/GetPowerWarGhostTime.inc"
{
    // ORIG 0x08480e92：(int)(ghostTime * f)，fistpl 就近取整。
    (void)user;
    // [b28c-L9] ORIG 帧 sub $0x28（结果槽 -0xc、转换源 -0x1c、cw -0x1e/-0x20），
    // 4.4 仅 $0x18 ⇒ P190 double framePad 撑帧；残余 2 = N82（4.1 不复用 -0xc/-0x1c）。
    // [b28d-solo] P0：ORIG 两个槽往返（源 -0x1c / 结果 -0xc）⇒ 原码有两个具名
    // 局部（4.4 对匿名转换临时会复用源槽）。
    int ghostTime = G_CDataManager()->m_powerParameter.m_ghostTime;
    int n = (int)((float)ghostTime * f);
    double framePad;
    (void)&framePad;
    return n;
}


#include "CPowerManager/IncreasePowerUserCount.inc"
{
    if (side == 1 || side == 3)
        ++m_side1UserCount;
    else
        ++m_side2UserCount;
}

#include "CPowerManager/DecreasePowerUserCount.inc"
{
    if (side == 1 || side == 3)
    {
        --m_side1UserCount;
        if (m_side1UserCount < 0)
            m_side1UserCount = 0;
    }
    else
    {
        --m_side2UserCount;
        if (m_side2UserCount < 0)
            m_side2UserCount = 0;
    }
}

#include "CPowerManager/CheckPowerWarEnterLimit.inc"
{
    int minLevel = G_CDataManager()->m_powerParameter.m_minEnterLevel;
    int minAge = G_CDataManager()->m_powerParameter.m_minEnterAge;
    if (minLevel == 0)
        minLevel = 0x1e;
    if (minAge == 0)
        minAge = 0xf;
    // ORIG 直调 CUser::CheckLimitMiniumAge（T 0x0864fc3e），禁内联展开（§4.1）。
    int outUnique;    // ORIG 不预清 -0x14（P198①：出参槽仅 lea，无 movl $0）
    // ORIG `xor $1;test;je`：`if (!bool)` 翻转直测；`==1` 反成 `test;je`（P196 极性）。
    if (!user->CheckLimitMiniumAge(minAge, outUnique))
    {
        return 0x5f;
    }
    if (user->get_charac_level() < minLevel)
        return 0xe;
    return 0;
}

#include "CPowerManager/GetWinnerSide.inc"
{
    return m_winnerSide;
}

#include "CPowerManager/IsWinerSide.inc"
{
    if (m_winnerSide == 0 || side == 0)
        return false;
    return m_winnerSide == side;
}

// [b28d-solo-CPowerManager] AE | ORIG 0x0847fb02 | 388=388 clean 全等，P167 69/69。
// 本轮修复：P0/P23 lastPlay 内联、P0/P52 三值内联实参（%edi/%esi/%ebx 跨 call
// 保活）、P292 档② isOriginal char→bool、P225 while 形复现 enterLimit setne 物化、
// P198① cooldown 两句式预清。
#include "CPowerManager/ProcessJoinPowerWar.inc"
{
    // ORIG 0x0847fb02：加入/退出 PowerWar 的完整校验与处理。
    // 职业 9/10（未转职）与佣兵不可参加。
    if (user->get_charac_job() == 9)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x15);
        return 1;
    }
    if (user->get_charac_job() == 10)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x15);
        return 1;
    }
    if (user->isCompetitionMercenary())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x15);
        return 1;
    }
    if (user->get_charac_guildkey() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 100);
        return 1;
    }
    if (user->getPowerSide() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x82);
        return 1;
    }
    if (side != 0 && side != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 1);
        return 1;
    }
    int enterLimit = 0;
    if (side == 1)  // 加入
    {
        // ORIG 直调 PowerParameterScript::checkPowerWarServer(a=+0x378 serverGroup,
        // b=+0x1b0 channelNo)，G_CEnvironment() 每次重物化（P183）；禁指针缓存与
        // 发明 helper（§4.1）。m_serverGroup 于 Server_Envir+0x1e0 ⇒ CEnvironment+0x378，
        // m_channelNo 于 Server_Envir+0x18 ⇒ CEnvironment+0x1b0。
        if (G_CDataManager()->m_powerParameter.checkPowerWarServer(
                G_CEnvironment()->m_serverEnvir.m_serverGroup,
                G_CEnvironment()->m_serverEnvir.m_channelNo) != 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x80);
            return 1;
        }
        if (!GlobalData::s_power_manager->IsPowerWarEventOn())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x7f);
            return 1;
        }
        enterLimit = CheckPowerWarEnterLimit(user);
        // [b28d-solo] P225：ORIG `cmpl $0,slot; setne %al; test %al,%al; je`（4.1
        // bool 物化后条件分支）唯一复现形 = while (v != 0) { body; break; }。
        while (enterLimit != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, (unsigned char)(enterLimit & 0xff));
            return 1;
        }
        // [b28d-solo] P198①：ORIG 在 G_CDataManager call 前有 `movl $0x0,-0x1c`
        // 预清 ⇒ 声明与赋值必须两句式。
        int cooldown = 0;
        cooldown = G_CDataManager()->m_powerParameter.m_powerWarCooldownMinutes * 0x3c;
        // [b28d-solo] P0/P23：ORIG 无 lastPlay 具名槽（getCurSec→%ebx 跨 call 保活、
        // lastPlay 直落 %eax 后 sub），源码须内联右值（左操作数先求值）。
        if (GlobalData::s_systemTime_.getCurSec() -
                user->getCurCharacLastPlayTickPowerWar() <= cooldown)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x84);
            return 1;
        }
        if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 1);
            return 0;
        }
        if (user->IsBusyDisjointStore())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 1);
            return 0;
        }
    }
    user->setCurrCharacJoinPower(side);
    // [b28d-solo] P292 档②：ORIG `movzbl slot,%eax; xor $1,%eax; test %al,%al; je`
    // ⇒ 局部须 bool（char 产 `cmpb $0` 直比）。
    bool isOriginal = false;
    // ORIG 两次 getPowerSide()（cmp $3 / cmp $4 独立调用，P183 反向勿缓存）。
    if (user->getPowerSide() == 3 || user->getPowerSide() == 4)
        isOriginal = true;
    if (side == 1)
    {
        if (isOriginal)
        {
            // [b28d-solo] P0/P52：ORIG 三值全经 callee-saved（%edi/%esi/%ebx）跨
            // GetServerProxy 调用保活，无栈槽 ⇒ 具名局部必须内联为实参表达式。
            GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendApplyOriginalPowerSide((unsigned int)user->getCurCharacNo(),
                                             (unsigned int)user->get_charac_guildkey(),
                                             (unsigned char)user->getPowerSide());
        }
        user->SetPowerWarLastPlayTime(GlobalData::s_systemTime_.getCurSec());
        user->setPowerWarHP((unsigned char)user->getCurCharacStamina());
        user->resetStraightVictories();
        GlobalData::s_power_manager->IncreasePowerUserCount((char)user->getPowerSide());
    }
    else
    {
        GlobalData::s_power_manager->DecreasePowerUserCount((char)user->getPowerSide());
        user->setCurCharacLastPlayTickPowerWar(GlobalData::s_systemTime_.getCurSec());
        user->SetCurCharacChaosDieTime(0);
        user->m_characterView.enableSaveCharacView();
    }
    if (!isOriginal)
    {
        PacketGuard pkt;
        pkt.put_header(1, 0xc3);
        pkt.put_byte(1);
        pkt.put_byte((int)side);
        pkt.finalize(true);
        user->Send(pkt);

        PacketGuard pkt2;
        pkt2.put_header(0, 2);
        pkt2.put_byte(0);
        pkt2.put_short(1);
        user->make_basic_info((char*)&pkt2, 0);
        pkt2.finalize(true);
        G_GameWorld()->send_all(pkt2, user);
    }
    return 1;
}

#include "CPowerManager/Init.inc"
{
    // ORIG 0x0847eabc：清零 +4,+8,+0x14,+0x18,+0xc,+0x10,+0x1c,+0x20，
    // memset (+0x24,4)、memset (+0x28,0x18c)、Reset 两日志，return 1。
    m_side1Point = 0;
    m_side2Point = 0;
    m_side1UserCount = 0;     // +0x14（ORIG 先清 +0x14/+0x18 再清 +0xc/+0x10）
    m_side2UserCount = 0;     // +0x18
    m_side1ChannelPoint = 0;  // +0x0c
    m_side2ChannelPoint = 0;  // +0x10
    m_field1c = 0;
    m_field20 = 0;
    memset(m_powerWarStartTime, 0, sizeof(m_powerWarStartTime));
    memset(m_rankings, 0, sizeof(m_rankings));
    m_powerWarLog.Reset();
    m_powerWarPacketLog.Reset();
    return 1;
}

#include "CPowerManager/LoadPowerWarInfo.inc"
{
    // ORIG 0x08480d98：DB 消息 SIG_LOAD_POWER_WAR 0x19b / -1 / server_group。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x3c4), true);
    **guard << 0x19b;
    **guard << -1;
    **guard << G_CEnvironment()->m_serverEnvir.m_serverGroup;   // ORIG env+0x378 直读
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "CPowerManager/LoadRankerInfo.inc"
{
    // ORIG 0x0847f4fe：DB 消息 0x165 / -1 / server_group。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x12e), true);
    **guard << 0x165;
    **guard << -1;
    **guard << G_CEnvironment()->m_serverEnvir.m_serverGroup;   // ORIG env+0x378 直读
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
// [b28d-solo-CPowerManager] IDENTICAL(28=28, ndiff 0) | ORIG 0x0847f1b0 | 本轮修复：
// P310 翻案（旧 N18b/P209附 判词作废）——循环局部由 unsigned 改 int + 显式
// `(unsigned int)i <= 2` 比较，4.4 即发 ORIG 的两段寄存器尾测
// `mov slot,%eax; cmp $2,%eax; setbe; test %al,%al; jne`（unsigned 局部恒折
// `cmpl $2,slot` 单段）。同族三件（GetRankingByCharacNo/SetPowerWarUserRankingInfo）
// 同法一次归零。
#include "CPowerManager/CheckCompleteDBLoadStatueInfo.inc"
{
    // [b28d-solo] P310：int 局部 + 显式 unsigned 比较复现 ORIG 0x0847f1bf 尾测
    // 两段物化（`mov -0x4(%ebp),%eax; cmp $0x2,%eax; setbe; test; jne`）。
    for (int i = 0; (unsigned int)i <= 2; ++i)
        if (m_rankings[i].m_flag != 1)
            return 0;
    return 1;
}
#include "CPowerManager/SetPowerInfo.inc"
{
    // ORIG 0x0847eb76：*this=side; +4=point; +8=channelPoint。
    m_winnerSide = side;
    m_side1Point = point;
    m_side2Point = channelPoint;
}

#include "CPowerManager/SetPowerWarStartTime.inc"
                                         unsigned char d, unsigned char h)
{
    m_powerWarStartTime[0] = y;
    m_powerWarStartTime[1] = m;
    m_powerWarStartTime[2] = d;
    m_powerWarStartTime[3] = h;
}

// [b28d-solo-CPowerManager] IDENTICAL(104=104, ndiff 0) | ORIG 0x0847ebec | 本轮修复：
// P310 同法（int i + `(unsigned int)i < 3`）复现 ORIG 0x0847ec0e 尾测两段物化
// `mov -0xc(%ebp),%eax; cmp $0x2,%eax; setbe; test; jne`；P167 CLEAR 12/12。
#include "CPowerManager/SetPowerWarUserRankingInfo.inc"
{
    // ORIG 0x0847ebec：characNo==0 -> 置 flag，否则写 +0x2c 并发 DB 消息 0x166。
    // [b28d-solo] P310：尾测须两段寄存器形 ⇒ 局部 int + 显式 unsigned 比较。
    for (int i = 0; (unsigned int)i < 3; ++i)
    {
        if (charNos[i] != 0)
        {
            m_rankings[i].m_characNo = charNos[i];
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x7d), true);
            **guard << 0x166;
            **guard << -1;
            **guard << m_rankings[i].m_characNo;   // unsigned int → operator<<(uint)（ORIG P167）
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
        else
        {
            m_rankings[i].m_flag = 1;
        }
    }
}



// [b28d-solo-CPowerManager] 语义完成 / byte 层 N（352=352, gap 0）| ORIG 0x0847ed4e |
// 此前修复：真语义缺口 m_flag=1（ORIG memcpy 后 `movb $0x1,0x8(%eax)`）+ pack(1)
// 具名视图消 add 链（P80/P99/P183）+ P222 正向 if 包裹 + P227/P139 全表达式临时链
// + __PRETTY_FUNCTION__ + P211 档③ 同型化。本轮：P310 消 N18b 尾测 1 条。
// 残差 = N4/N86b（13 处 4.1 `lea 0x0(,%eax,4),%edx` vs 4.4 `shl $0x2,%eax` 强度
// 归约形；探针 /tmp/probe2d{,2}.cpp 八形——一维/二维[3][1]/`*(arr+i)`/
// `(&arr[0])[i]`/指针局部/unsigned 索引/int 数组下标——4.4 恒 shl，源形不可择）。
// P167 CLEAR 7/7。
#include "CPowerManager/SetPowerWarUserStatueInfo.inc"
{
    // ORIG 0x0847ed4e：按 characNo（info+0x00）匹配条目，拷贝雕像字段并置 flag，
    // 完成后若 DB 加载齐则 SendPowerWarUserStatueInfo(0)。
    // SIG_LOAD_POWER_WAR_STATUE_INFO 仅建模为 m_pad[0x100]，字段按 ORIG 偏移访问：
    // +0x00 characNo / +0x04 name(0x1e) / +0x22 job / +0x23 level / +0x24 grow /
    // +0x25 guildName(0x28) / +0x50 guildId / +0x54 equipment(0x2c)。
    // [b28d-solo] P183：ORIG 每字段重读形参并 disp 折叠（`mov 0xc(%ebp),%eax;
    // movzbl 0x22(%eax),%edx`），禁 src 指针缓存（缓存生 add 链且帧 +4）。
    // [b28d-solo] P222/P59：ORIG `cmp; jne <循环尾>` 匹配即直落 body ⇒ 正向
    // `if (匹配) { body; break; }`（原 `if (!=) continue;` 产反向 je 块布局）。
    for (int i = 0; (unsigned int)i < 3; ++i)
    {
        if (m_rankings[i].m_characNo == ((const PowerWarStatueView*)info)->m_characNo)
        {
            m_rankings[i].m_job = ((const PowerWarStatueView*)info)->m_job;
            m_rankings[i].m_level = ((const PowerWarStatueView*)info)->m_level;
            m_rankings[i].m_grow = ((const PowerWarStatueView*)info)->m_grow;
            m_rankings[i].m_guildId = ((const PowerWarStatueView*)info)->m_guildId;
            strncpy(m_rankings[i].m_name,
                    ((const PowerWarStatueView*)info)->m_name, 0x1e);
            strncpy(m_rankings[i].m_guildName,
                    ((const PowerWarStatueView*)info)->m_guildName, 0x28);
            memcpy(m_rankings[i].m_equipment,
                   ((const PowerWarStatueView*)info)->m_equipment, 0x2c);
            // [b28d-solo] 真语义缺口修复：ORIG 0x0847f0xx `movb $0x1,0x8(%eax)`
            // （this+i*0x84+0x28 = 元素 +0x00 m_flag）在 memcpy 之后、cMyTrace 之前。
            m_rankings[i].m_flag = 1;
            // ORIG 0x0847f0e5：cMyTrace(3 参) + "Set Statue Ranker ..." 变参日志。
            // 注意 ORIG 压栈 job/lev 槽位互换（lev 值在 job 槽、job 值在 lev 槽），
            // 且 EQUP 用 m_equipment[0..8] 与 [10]（跳过 [9]）——忠实保留。
            // [b28d-solo] P227/P139：ORIG 在 ctor 前求值全部 varargs 并跨 ctor 保活
            // （`mov 0x378…` 类装载先于 `call cMyTrace::C1`），且 file 串为
            // `_ZZ…__PRETTY_FUNCTION__` 局部静态符号（ORIG 0x08c72f80）⇒ 全表达式
            // 临时链 + __PRETTY_FUNCTION__。
            cMyTrace(__PRETTY_FUNCTION__, 0xa4, 0)
                ("Set Statue Ranker %s(%d), job:%d, lev:%d, grow:%d, guild_name:%s, guild_id:%d, EQUP [head:%d][hair:%d][face:%d][jacket:%d][pants:%d][shoes:%d][breast:%d][waist:%d][skin:%d][weapon:%d]",
                  m_rankings[i].m_name, m_rankings[i].m_characNo,
                  static_cast<int>(m_rankings[i].m_level),
                  static_cast<int>(m_rankings[i].m_job),
                  static_cast<int>(m_rankings[i].m_grow),
                  m_rankings[i].m_guildName, m_rankings[i].m_guildId,
                  m_rankings[i].m_equipment[0], m_rankings[i].m_equipment[1],
                  m_rankings[i].m_equipment[2], m_rankings[i].m_equipment[3],
                  m_rankings[i].m_equipment[4], m_rankings[i].m_equipment[5],
                  m_rankings[i].m_equipment[6], m_rankings[i].m_equipment[7],
                  m_rankings[i].m_equipment[8], m_rankings[i].m_equipment[10]);
            break;
        }
    }
    if (CheckCompleteDBLoadStatueInfo())
        SendPowerWarUserStatueInfo(0);
}

// [b28d-solo-CPowerManager] IDENTICAL(186=186, ndiff 0) | ORIG 0x0847f5de | clean
// 全等（P227 全表达式临时链复现两 trace 槽序 + varargs 跨 ctor 保活）；P139 恢复
// `_ZZN13CPowerManager12ClearMVPInfoEvE19__PRETTY_FUNCTION__` 局部静态符号。
#include "CPowerManager/ClearMVPInfo.inc"
{
    // [b28d-28] ORIG 0x0847f5fe/-0x38 与 0x0847f83x/-0x28：两个 cMyTrace 槽
    // 深→浅（4.1 方向）；具名对象 4.4 恒反向 ⇒ 用全表达式临时链（P227/P221）
    // 复现槽序，同时使 `tr(...)` 的 varargs（G_CEnvironment()->+0x378）在
    // ctor 前求值并以 %ebx 跨 call 保活（ORIG 0x0847f84x `mov 0x378(%eax),%ebx`）。
    // [b28d-solo] P139：ORIG file 串为 `_ZZN13CPowerManager12ClearMVPInfoEvE19
    // __PRETTY_FUNCTION__`（0x08c72f40）⇒ 传 __PRETTY_FUNCTION__ 恢复局部静态符号。
    cMyTrace(__PRETTY_FUNCTION__, 0x143, 0)("CLEAR MVP INFO");
    memset(m_rankings, 0, sizeof(m_rankings));
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x149), true);
    **guard << 0x1a5;
    **guard << -1;
    // ORIG 直读 G_CEnvironment()+0x378（m_serverEnvir.m_serverGroup），两次
    // 各自重物化（P183）；禁 getter 调用（EXTRA symbol）与跨调用缓存。
    **guard << G_CEnvironment()->m_serverEnvir.m_serverGroup;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

    cMyTrace(__PRETTY_FUNCTION__, 0x152, 0)
        ("PUT DB MSG - E_DELETE_POWER_WAR_STATUE_MESSAGE / server_group : %d",
         G_CEnvironment()->m_serverEnvir.m_serverGroup);

    CSharedServerMessageManager* mgr =
        G_CGameManager()->GetSharedServerMessageManager();
    if (mgr != 0)
    {
        mgr->DeleteSpecificMessage(1);
        mgr->DeleteSpecificMessage(2);
        mgr->DeleteSpecificMessage(3);
    }

    PacketGuard pkt;
    pkt.put_header(0, 0xc0);
    pkt.put_byte(2);
    pkt.put_byte(3);
    char* msg = (char*)"null string";   // ORIG char* → put_str(char*,int) 重载
    int len = static_cast<int>(strlen(msg));
    for (int i = 0; i < 3; ++i)
    {
        pkt.put_byte(i + 1);
        pkt.put_int(len);
        pkt.put_str(msg, len);
        pkt.put_int(len);
        pkt.put_str(msg, len);
    }

    pkt.finalize(true);
    G_GameWorld()->send_all_with_state(pkt, 3);
}

#include "CPowerManager/IsAbleToRevival.inc"
{
    // ORIG 0x0847f952：复活冷却判定。penalty 同 GetPowerWarResponPenalty，
    // ghost=ROUND(ghostTime*penalty)，与 (charNo - 混沌死亡时刻) 比较。
    // [b28d-28] ORIG 0x0847f98d：unsigned int→float 走 64 位零扩展
    // （`mov $0,%edx; mov %eax,-0x28; mov %edx,-0x24; fildll`，同
    // GetPowerWarResponPenalty 0x08480f03）；尾块 ORIG 为
    // `jle L_then`（ghost<=delta → result=0;return 1 在后）、
    // ghost>delta 直落（result=ghost-delta; return 0）⇒ 源码取正向早返形。
    float penalty = 1.0f;
    if ((unsigned int)user->GetCurCharacChaosStateTime() <
        (unsigned int)G_CDataManager()->m_powerParameter.m_penaltyThreshold)
    {
        penalty = 2.0f -
                  (float)(unsigned long long)(unsigned int)user->GetCurCharacChaosStateTime() /
                      (float)G_CDataManager()->m_powerParameter.m_penaltyThreshold;
    }
    int ghost = (int)((float)G_CDataManager()->m_powerParameter.m_ghostTime * penalty);
    // [b28d-28] ORIG 0x0847f9f2 装载序：`call GetCurCharacChaosDieTime` 之后才
    // `mov 0x10(%ebp),%edx`（charNo）⇒ 无符号中间量 d = charNo - (unsigned)die
    // 使 4.4 求值序翻为「先 call 后读 charNo」（探针 /tmp/l28_probe/p3.cpp g4 全等）。
    int delta = (int)(charNo - (unsigned int)user->GetCurCharacChaosDieTime());
    if (ghost > delta)
    {
        result = ghost - delta;
        return false;
    }
    result = 0;
    return true;
}

#include "CPowerManager/IncreaePowerWarPointInChannel.inc"
{
    if (side == 1)
        m_side1ChannelPoint += point;
    else
        m_side2ChannelPoint += point;
}

// [b28d-solo-CPowerManager] IDENTICAL(237=237, ndiff 0) | ORIG 0x0847f200 | 本轮修复：
// P310（循环变量 i 由 unsigned 改 int + 显式 `(unsigned int)i < 3`）复现 ORIG 尾测
// 两段物化 `mov -0x14(%ebp),%eax; cmp $0x2,%eax; setbe; test; jne`，旧「N18b 尾测
// 1 条」判词作废。此前：len 具名缓存消 EXTRA +2 strlen（P167 22/22 CLEAR）、
// P47 正极性 if/else、P212 全头部声明（槽位全等 pkt-0x28/count-0x1c/len-0x18/
// i-0x14/i2-0x10/j-0xc）、P171 死预清 len=0。
#include "CPowerManager/SendPowerWarUserStatueInfo.inc"
{
    // ORIG 0x0847f200：组 SEND 雕像信息包（0xb6），含各条目姓名/职业/等级/
    // 成长/公会名/公会id/装备，user==0 全服广播，否则单发。
    // [b28d-solo] P212：ORIG 槽深序 count(-0x1c) < len(-0x18) < i(-0x14) <
    // i2(-0x10) < j(-0xc)；4.4 块内局部按首次使用序分配 ⇒ 全部提至函数头声明
    // （声明序 = ORIG 槽深序），块内仅赋值。
    PacketGuard pkt;
    int count;
    int len;
    int i;
    int i2;
    int j;
    pkt.put_header(0, 0xb6);
    count = 0;
    for (i = 0; (unsigned int)i < 3; ++i)
        if (m_rankings[i].m_characNo != 0)
            ++count;
    pkt.put_byte((int)(char)count);
    len = 0;   // [b28d-solo] P171：ORIG 0x0847f2xx `movl $0x0,-0x18` 死预清（照抄）
    for (i2 = 0; i2 < count; ++i2)
    {
        // [b28d-solo] P0/P102：ORIG 用单一具名 len 槽（-0x18）缓存 strlen 结果，
        // put_int/put_str 共用 ⇒ 每处仅 1 次 strlen（原内联两次 = EXTRA +2 strlen）。
        len = (int)strlen(m_rankings[i2].m_name);
        pkt.put_int(len);
        pkt.put_str(m_rankings[i2].m_name, len);
        pkt.put_byte((int)(char)m_rankings[i2].m_job);
        pkt.put_byte((int)(char)m_rankings[i2].m_level);
        pkt.put_byte((int)(char)m_rankings[i2].m_grow);
        len = (int)strlen(m_rankings[i2].m_guildName);
        pkt.put_int(len);
        pkt.put_str(m_rankings[i2].m_guildName, len);
        pkt.put_int(m_rankings[i2].m_guildId);
        for (j = 0; j < 0xb; ++j)
        {
            if (m_rankings[i2].m_equipment[j] == 0 || j == 9)
                pkt.put_int(-1);
            else
                pkt.put_int(m_rankings[i2].m_equipment[j]);
        }
    }
    pkt.finalize(true);
    // [b28d-solo] P47/P222：ORIG `cmp; je <send_all 段>`（Send 直落）⇒ 正极性
    // `if (user != 0) Send; else send_all;`（原 ==0 首臂产反向 jne 块布局）。
    if (user != 0)
        user->Send(pkt);
    else
        G_GameWorld()->send_all(pkt);
}

#include "CPowerManager/GetPowerUserCount.inc"
{
    return side == 1 ? m_side1UserCount : m_side2UserCount;
}

#include "CPowerManager/GetPowerWarPoint.inc"
{
    return (int)side == 1 ? m_side1Point : m_side2Point;
}

#include "CPowerManager/GetPowerWarPointInChannel.inc"
{
    return side == 1 ? m_side1ChannelPoint : m_side2ChannelPoint;
}

// [b28d-solo-CPowerManager] IDENTICAL(28=28, ndiff 0) | ORIG 0x0847eba0 | 本轮修复：
// P310 翻案（旧 N18b/P209附 判词作废）——int 局部 + 显式 `(unsigned int)i <= 2`
// 复现 ORIG 0x0847ebd8 尾测 `mov slot,%eax; cmp $2,%eax; setbe; test; jne`。
#include "CPowerManager/GetRankingByCharacNo.inc"
{
    // [b28d-solo] P310：int 局部 + 显式 unsigned 比较复现两段寄存器尾测。
    for (int i = 0; (unsigned int)i <= 2; ++i)
        if (m_rankings[i].m_characNo == charNo)
            return (int)i + 1;
    return 0;
}

// ORIG 0x084ebe2e（_ZN14CPowerManager16SetPowerWarPointEii，W）：
//   *(int*)(this+0x04) = arg1; *(int*)(this+0x08) = arg2; ret。
#include "CPowerManager/SetPowerWarPoint.inc"
{
    m_side1Point = sideIdx;
    m_side2Point = point;
}
