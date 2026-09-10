// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W8_CU_G/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CUser.h"
#include "Inven_Item.h"
#include "CDataManager.h"                    // G_CDataManager / getItemList(+0x0c)
#include "GlobalData.h"                      // s_systemTime_/s_capchaDataManager/
                                             // g_autoPunishRuleHackTypeMgr/s_guild_proxy_mgr/s_monitor_proxy_mgr/s_statisticMgr
#include "CStatisticMgr.h"                   // 本 TU 新建权威视图
#include "CGuildServerProxy.h"               // SendCharLevelGrowType
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"                 // GetServerProxy<T> 完整模板
#include "CInventory.h"                      // insertItemIntoInventory / md5_* 声明（md5_context 兼容视图）
#include "GameWorld.h"
#include "TimerQueue.h"                      // TIMER_MESSAGE/InsertTimer/G_TimerQueue
#include "GameTypes.h"                       // cMyTrace
#include "Divide_C_ClassIP_EndIP.h"
#include "CUserCharacInfo.h"                 // ContinuousItemInfo/CSystemTime 等
#include "CSystemTime.h"
#include "CCapchaDataManager.h"              // WongWork::CCapchaDataManager + MSG_UDP_CAPCHA_CERTIFY
#include "CMailBoxHelper.h"                  // ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"         // g_scriptStringManager_/findString(4,key,0)
#include "Arad_DataManager.h"                // ARAD::Singleton<Arad_DataManager>::Get/findGameScript
#include "Arad_EventPeriodDataManager.h"     // isApplied/checkCreateTime（producer 批提供定义）
#include "CUserProc.h"                       // APSystem::CUserProc::ClearActionAndSendtoUser
#include "CLevelDungeonPlayStatistic.h"
#include "CExpertJob.h"                      // expert_job::CExpertJob::IncreaseExpertJobExp(static)
#include "SkillSlot.h"                      // SkillSlot::get_remain_*_at_index
#include "CParty.h"                         // send_to_party（GetParty 完整类型）
#include "W6_HackPunishDeps.h"               // CAutoPunishRuleHackTypeMgr::checkTotalTradeRuleOver

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------
// 外部 DB 请求类最小声明区（owner=DB 域批次，定义不在本批）；
// namespace/class 形态与 ORIG mangled 对齐，均 nm 实证。
// ---------------------------------------------------------------------------
// b28d-solo-W8_CU_G：权威声明见 W7LinkDB.cpp:28/44 与 CUserOnlyOrig.cpp:93 ——
// 两者均 **static void**。ORIG 调用点 0x0867fc32/0x0867fcfb 的 (%esp) 是第 1 实参
// （get_acc_id()），非 this 槽 ⇒ 静态调用（P88）；原非静态声明多压临时对象 this，
// 使全部实参右移 4B。
struct DB_AutoPunishedSecondUserData {       // ORIG makeRequest T 0x0843a790 _ZN...makeRequestEjyjj
    static void makeRequest(unsigned int accId, unsigned long long gold,
                            unsigned int uid, unsigned int who);
};
struct DB_InsertAutoPunishSecondUser {       // ORIG makeRequest T 0x0842e34e _ZN...makeRequestEjjyb
    static void makeRequest(unsigned int accId, unsigned int who,
                            unsigned long long gold, bool punishFlag);
};
// 其余五类同 W7LinkDB.cpp:36/50/60/66/72 权威声明：均 static void（原 int 非静态
// 声明使调用点多压临时对象 this + 返回值形态错，P88）。
struct DB_HackIPSave {                       // ORIG makeRequest T 0x08436ff8 _ZN...makeRequestEPKcS1_jj
    static void makeRequest(const char* startIp, const char* rawIp,
                            unsigned int a, unsigned int b);
};
struct DB_InsertAutoPunishFirstUser {        // ORIG makeRequest T 0x0842dd7c _ZN...makeRequestEjPKcjjjjtj
    static void makeRequest(unsigned int accId, const char* ip,
                            unsigned int p1, unsigned int p2, unsigned int p3,
                            unsigned int p4, unsigned short type, unsigned int p6);
};
struct DB_ForceSaveSimpleSSO {               // ORIG makeRequest T 0x084281ae _ZN...makeRequestEji
    static void makeRequest(unsigned int accId, int errCnt);
};
struct DB_CountOfHumanCertify {              // ORIG makeRequest T 0x084283ca _ZN...makeRequestEj
    static void makeRequest(unsigned int accId);
};
struct DB_SaveGiveAvengerTitleFlag {         // ORIG makeRequest T 0x084422dc _ZN...makeRequestEijj
    static void makeRequest(int uid, unsigned int accId, unsigned int characNo);
};


// ===========================================================================
// 支撑小类 ctor（ORIG W，仅写前两个 uint；objdump 0x869380e/0x8693824 直转写）
// ===========================================================================
#include "W8_CU_G/stLookOuts.inc"
#include "W8_CU_G/tyIPCounters.inc"

// ===========================================================================
// Hack trade money 统计族（objdump 直转写；m_379 字段组）
// ===========================================================================
#include "W8_CU_G/getTotalHackUserTradeGold.inc"
{
    return m_379.m_hackTradeGold;
}

#include "W8_CU_G/getTotalHackUserTradeCnt.inc"
{
    return static_cast<int>(m_379.m_hackTradeCnt);
}

#include "W8_CU_G/incTotalHackUserTradeCnt.inc"
{
    m_379.m_hackTradeCnt += n;
}

#include "W8_CU_G/incTotalHackUserTradeGold.inc"
{
    // ORIG：内联 64 位读+adc 累加（mov 0x8d1b0/0x8d1ac + add + adc），无 getter 调用。
    m_379.m_hackTradeGold += gold;
}

#include "W8_CU_G/getCharacAntibotSerialNum.inc"
{
    return m_field8ec24;
}

// ORIG 0x0867fbba：已 loading 时累加并按规则判定惩罚入库；否则置 loading 标志
// 并记首条未加载流水（DB_AutoPunishedSecondUserData::makeRequest(accId,gold,uid,who)）。
// b28d-solo-W8_CU_G: DIFF/N7 封顶（100/101）— 本波修 5 处：
// ①真 ABI bug：两个 DB 请求类局部声明非静态（多压临时对象 this，实参整体右移 4B）
//   ⇒ 改 `static void`（权威声明 W7LinkDB.cpp:28/44、CUserOnlyOrig.cpp:93；ORIG
//   调用点 0x0867fc32/0x0867fcfb 的 (%esp) 为第 1 实参 get_acc_id()，P88）；
// ②真语义 bug：末尾 DB_InsertAutoPunishSecondUser::makeRequest 第 3 参原写 totalNow，
//   ORIG 0x0867fce6 读 -0x10/-0xc = 形参 gold ⇒ 改传 gold；
// ③isLoadingHackGold uchar→bool（P292，Main 代落 CUser.h:2695/CUser.cpp:5017）⇒
//   首条件 `xor $1; test; je` 复现 0x0867fbe5；
// ④去 totalNow/cntNow/punish8 具名局部（ORIG 全程 ebx:esi/eax 寄存器直传，P0）⇒
//   帧 0x54→0x30，spill 槽 -0x10/-0xc/-0x18/-0x14 与 ORIG 全等；
// ⑤`gold + totalGold` 交换书写序复现 ORIG 0x0867fc3f 先载 totalGold 入 eax:edx。
// 余 1 条：this=全局指针 g_autoPunishRuleHackTypeMgr 装载 ORIG `mov g,%edx` 直落
// vs 4.4 `mov %eax,%edx; mov g,%eax`（N7 寄存器角色互换）；b28d-W8b 复测去
// `static_cast<unsigned int>`（隐式 int→unsigned 实参旋钮）同 101 ⇒ 封 N。
#include "W8_CU_G/gainMoneyFromHackUser.inc"
                                  unsigned int cnt, unsigned long long totalGold)
{
    if (!isLoadingHackGold())
    {
        setLoadHackGold(true);
        DB_AutoPunishedSecondUserData::makeRequest(get_acc_id(), gold, GetUID(), who);
        return;
    }

    // b28d-solo-W8_CU_G：ORIG 0x0867fc3f 先载 totalGold(-0x18/-0x14) 入 eax:edx、
    // 后载 gold 入 ecx:ebx ⇒ 4.4 先求右操作数，写 `gold + totalGold`（加法交换律）。
    incTotalHackUserTradeGold(gold + totalGold);
    incTotalHackUserTradeCnt(cnt);

    // b28d-solo-W8_CU_G：ORIG 0x0867fc7f/0x0867fc8e 两 getter 结果全程寄存器
    //（ebx:esi / eax，无槽往返，P0）⇒ 内联实参，勿声明 totalNow/cntNow 具名局部。
    if (GlobalData::g_autoPunishRuleHackTypeMgr->checkTotalTradeRuleOver(
            static_cast<unsigned int>(getTotalHackUserTradeCnt()),
            getTotalHackUserTradeGold()))
    {
        SetTradePunishType(8);
    }
    // b28d-solo-W8_CU_G 真语义修正：ORIG 0x0867fce6 末尾 makeRequest 第 3 参读
    // -0x10/-0xc（形参 gold 槽），**非** getTotalHackUserTradeGold() 结果
    //（该值仅用于 checkTotalTradeRuleOver，ebx:esi 之后即弃）。原码传 totalNow
    // 属参数传错，已改正为 gold。
    DB_InsertAutoPunishSecondUser::makeRequest(
        get_acc_id(), who, gold, isTradePunishType(8));
}

// ORIG 0x0867f748：黑名单/惩罚注册入口。
// type==2 lookOuts1；type==3 IP 监视登记（一次性）；type==10 ARS 延迟惩罚定时器；
// 其它走即时 OnPunishUser + 0x81 定时器；尾部对非惩罚状态账号补发首犯入库包。
// b28d-W8(2026-09-09): **AE 276/276**（TU 四数 diff−1）— 本波 6 修：
// ①P227 全表达式临时链 + 删 notifyType 复制槽（直接用形参 type，尾部 `type=10`）；
// ②startIp/rawIp 16 字节（ORIG 各 4×movl，原 17 字节多 2 条 movb）；
// ③声明序 rawIp 先（0x867f832 组）→ GetPeerIP2 → startIp（0x867f86e 组）；
// ④P133 四条独立 `if(type==7/9/12/5)`（合并短路形产 4×je 同落点）；
// ⑤P59 守卫 `if(!m_hackIpLogged){body;return}else return`（正写少 1 条 xor）；
// ⑥ip[16]={0} + GetPeerIP2 返回值直传 makeRequest arg1（P0）。
// ⑦**真语义 bug**：IPCounterLog_InsertIp 第三实参原传 startIp，ORIG 0x867f8ee
//   `lea -0x58(%ebp),%ecx` 传的是 rawIp（GetPeerIP2 目标缓冲）⇒ 记录的是原始 IP。
// p167 CLEAR(31/31)，gap 0，语义完成。ORIG 0x0867f748。
#include "W8_CU_G/setHackUser.inc"
                        unsigned int p4, unsigned short type, unsigned int p6)
{
    // b28d-W8: P227 全表达式临时链 + 直接用形参 type —— ORIG 0x867f753/75a
    // `mov 0x1c(%ebp),%eax; mov %ax,-0x6c(%ebp); movzwl -0x6c(%ebp),%esi`
    //（仅形参一次性截断 spill 槽，无 notifyType 复制槽）；0x867f7be 前右起求值
    // type(%esi)→NumberToString(%ebx)→ctor，idStr 无槽（P0）。尾部改形参本身
    // 0x867fb20 `movw $0xa,-0x6c(%ebp)`（ORIG 直接改 spill 槽）。
    cMyTrace("void CUser::setHackUser(unsigned int, unsigned int, "
             "unsigned int, unsigned int, short unsigned int, unsigned int)",
             0x7e55, 0)(
        "m_id : %s, SetHackUser : %d(%d)\t  %d(%d)   ApplyFlag = %d\n",
        NumberToString(get_acc_id(), 0), p1, p2, p3, p4,
        static_cast<unsigned int>(type));

    if (type == 2)
    {
        m_lookOuts1.push_back(stLookOuts(p1, p3));
    }
    else if (type == 3)
    {
        // b28d-W8: P59/P248档② —— ORIG 0x867f820 `movzbl 0x8e950; xor $1; test;
        // je <return 岛>`（`!flag` 正极性包裹 + else return），正写
        // `if(flag) return; body` 产 `test; jne` 少 1 条 xor。
        if (!m_mcap.m_hackIpLogged)
        {
            // b28d-W8: 声明序照 ORIG —— rawIp 先清零(0x867f832 组 -0x58)+GetPeerIP2，
            // startIp 后清零(0x867f86e 组 -0x68)+Divide 输出（P28 声明序即发射序）。
            // 16 字节各 4×movl（无第 5 条 movb）；GetPeerIP2(rawIp,0x10) 写满 16B
            // 无 NUL（P171 原码遗留照抄）。
            char rawIp[16] = {0};
            m_network.GetPeerIP2(rawIp, 0x10);
            char startIp[16] = {0};
            Divide_C_ClassIP_EndIP(startIp, static_cast<char*>(0), rawIp);

            m_ipCounters.push_back(tyIPCounters(p1, p3));
            m_mcap.m_hackIpLogged = true;
            // b28d-W8 真语义修正：ORIG 0x867f8ee `lea -0x58(%ebp),%ecx` 传的是
            // rawIp（GetPeerIP2 目标缓冲 = local_6c+0x10），非 startIp（Divide 输出
            // = -0x68）；旧码传错缓冲 ⇒ 记的是点分起始段而非原始 IP。
            m_historyLog.IPCounterLog_InsertIp(static_cast<int>(p1),
                                               static_cast<int>(p3), rawIp);
            DB_HackIPSave::makeRequest(startIp, rawIp, p1, p3);
            return;
        }
        else
        {
            return;
        }
    }
    else if (type == 10)
    {
        m_lookOuts2.push_back(stLookOuts(p1, p3));
        if (isTradePunishType(4))
        {
            return;
        }
        // b28d-solo-W8_CU_G 真语义修正：ORIG 0x0867f988 只调 get_rand_int 一次
        //（结果存 -0x14 槽），printf 与 InsertTimer 复用同值；原码 printf 实参又调
        // 一次 get_rand_int（p167 EXTRA +1，且消耗额外随机数）⇒ 合并为单次。
        int waitSec = get_rand_int(0x122) + 10;
        printf("ARS TRADE PUNISH USER - Rand Sec : %d\n", waitSec);
        G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(0),
                                    static_cast<int>(GetUID()),
                                    static_cast<TIMER_MESSAGE>(0x97), waitSec,
                                    get_unique_id(), 0xffffffffu);
    }
    else
    {
        int delaySec = get_rand_int(0x122) + 10;
        m_379.m_field0 = true;
        if (type == 8)
        {
            m_mcap.m_field561 = true;
            delaySec = 1;
        }
        // b28d-W8: P133 —— ORIG 0x867fa2e..0x867fa72 四条独立 `cmpw $X; jne;
        // movb $1,0x8e951` 守卫（非合并短路，合并形产 4×je 同落点）。
        if (type == 7)
        {
            m_mcap.m_field561 = true;
        }
        if (type == 9)
        {
            m_mcap.m_field561 = true;
        }
        if (type == 12)
        {
            m_mcap.m_field561 = true;
        }
        if (type == 5)
        {
            m_mcap.m_field561 = true;
        }
        G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(0),
                                    static_cast<int>(GetUID()),
                                    static_cast<TIMER_MESSAGE>(0x81), delaySec,
                                    get_unique_id(), 0xffffffffu);
        OnPunishUser();
        if (m_field796d8 != 0 && GetTradePunishType() != 0)
        {
            return;
        }
        if (m_field796d8 != 0 && (type == 1 || type == 4 || type == 9))
        {
            type = 10;
        }
    }

    // b28d-W8: ORIG 0x867fb26..0x867fb3b 4×movl 内联清零 16B（无第 5 条 movb）
    // ⇒ `= {0}` 的 16 字节数组，非未初始化 17 字节。ORIG 0x867fb66 `mov %eax,%ebx`
    // = GetPeerIP2 返回值直传 makeRequest arg1（无 ipPtr 具名槽，P0 内联实参）。
    char ip[16] = {0};
    DB_InsertAutoPunishFirstUser::makeRequest(
        get_acc_id(), m_network.GetPeerIP2(ip, 0x10), p1, p2, p3, p4, type, p6);
}

// ORIG 0x084fa298：对角色列表首元素起 size()*0x14d1 字节做 MD5 摘要。
// b28d-solo-W8_CU_G: NEAR/N95 封顶 — ORIG 0x084fa298 md5_context 槽基 -0xe0 vs
// 重建 -0xe8：4.4-O0 局部槽基恒从 ebp-8 起（帧顶留 8B gap），4.1 大局部可顶到 ebp
//（N95 定性；double framePad 反致下移、int pad/声明序调整不可上移）。帧同 0xf4、
// 指令/助记符逐条全等，仅 3 处 lea disp 差（32/32）。语义等价。
#include "W8_CU_G/getHashCode.inc"
{
    md5_context ctx;
    md5_starts(&ctx);
    // P0/P134：ORIG 无 bytes/head 槽（imul 结果 ebx 跨 call 缓存、operator[]
    // 结果 eax 直传），实参嵌套求值（R2L：size()*0x14d1 先物化，[0] 后算）。
    md5_update(&ctx,
               reinterpret_cast<unsigned char*>(&m_characList[0]),
               static_cast<int>(
                   static_cast<unsigned int>(m_characList.size()) * 0x14d1u));
    md5_finish(&ctx, digestOut);
}

// ORIG 0x0867f4c8：CleanPad 人机认证应答比对（验证码头 5×ushort 逐项日志后 memcmp）。
// b28d-solo-W8_CU_G: DIFF/N 封顶（160/160，ndiff 148→37）— 本波修 3 处：
// ①P227 全表达式临时链 `cMyTrace(f,l,v)(...)`：ORIG 0x0867f4d1..0x0867f598 在 ctor
//   前右起求值全部 varargs + NumberToString（具名 trace + 语句形恒 ctor 先）；
// ②CUser.h:2445 匿名 union 双视角 `m_capchaCertifyW[5]`（P235，Main 代落）⇒ 5 处
//   读取由 add+movzwl 三步折回 ORIG 单条 `movzwl 0x8e0fxx(%eax),%eax`；
// ③varargs 中 orig 组实参顺序改为 [0..4]（ORIG 压栈序 0xc/%esi=orig[0] … 0x1c/-0x34
//   =orig[4]，P23/压栈序判读）⇒ 求值序 [4]→[0] 与 ORIG 全等。
// b28d-W8b: **DIFF→AE**（160/160）— 块布局判据（P47/P59）：ORIG 0x0867f62e
// `test %eax,%eax; je +0x203` 把成功块尾置、失败块直落；负极性 `==0` 写法产
// `jne <失败块>` + 成功块直落（指令数相同但 jcc/块位反）。翻转 if 顺序后逐条对齐。
// 余：@0x0867f5e7 字符串地址归一化伪差（§6.3）。15/15 调用点全等，语义完成。
#include "W8_CU_G/checkHumanCertify.inc"
{
    // b21 verdict=DIFF（N 残余）：160/166。15=15 调用点全等（get_acc_id×n/NumberToString/
    //   cMyTraceC1+cMyTracecl/DB_CountOfHumanCertify::makeRequest/memcmp/
    //   DB_ForceSaveSimpleSSO::makeRequest×2/isCleanPadVerifyLimit/setHumanCertified/
    //   SendCmdErrorPacket）。orig/ans 5 组 ushort 物化顺序与 trace 实参求值序差(N82)、
    //   `const unsigned short* orig` 具名局部占槽(ORIG 直读 this+0x8e0ec 偏移族)。语义全等，
    //   byte 层 N 封顶。
    {
        // b28d-solo-W8_CU_G：P227 全表达式临时链 —— ORIG 0x0867f4d1..0x0867f598 在
        // cMyTrace ctor 之前右起求值全部 varargs（answer[4..0]→槽、orig[4..0]→槽/寄存器）
        // 与 NumberToString；具名 trace + 语句形恒 ctor 先。
        // 另：ORIG 每次 `mov 0x8(%ebp),%eax; movzwl 0x8e0f4(%eax),%eax` 重载 this 直读
        // 偏移（P183）⇒ 不设 orig 指针局部；用 CUser.h 匿名 union 双视角
        // `m_capchaCertifyW[5]`（P235，Main 代落）让 4.4 折叠单条 disp。
        cMyTrace(__PRETTY_FUNCTION__, 0x7e07, 0)(
            "checkHumanCertify m_id:%s, orig:%x%x%x%x%x, ans:%x%x%x%x%x",
            NumberToString(get_acc_id(), 0),
            static_cast<unsigned int>(m_capchaCertifyW[0]),
            static_cast<unsigned int>(m_capchaCertifyW[1]),
            static_cast<unsigned int>(m_capchaCertifyW[2]),
            static_cast<unsigned int>(m_capchaCertifyW[3]),
            static_cast<unsigned int>(m_capchaCertifyW[4]),
            static_cast<unsigned int>(answer[0]), static_cast<unsigned int>(answer[1]),
            static_cast<unsigned int>(answer[2]), static_cast<unsigned int>(answer[3]),
            static_cast<unsigned int>(answer[4]));
    }

    DB_CountOfHumanCertify::makeRequest(get_acc_id());

    // b28d-W8b: 块布局判据（P47/P59 族）—— ORIG 0x0867f62e `test %eax,%eax;
    // je +0x203` 把**成功块放尾部**、失败块直落（memcmp!=0 走直落失败路径）；
    // 负极性 `==0` 写法则产 `jne <失败块>` + 成功块直落（块序互换，160/160 但
    // jcc/块位反）。改为正极性 `!=0` + 失败块前置、成功块尾置，逐条复现。
    if (memcmp(answer, m_capchaCertify, static_cast<size_t>(len)) != 0)
    {
        ++m_humanCertifyErrorCnt;
        if (isCleanPadVerifyLimit() && m_field8e3ed)
        {
            DB_ForceSaveSimpleSSO::makeRequest(get_acc_id(), m_humanCertifyErrorCnt);
            SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x91), 8);
        }
        setHumanCertified(true);
        return false;
    }

    if (m_humanCertifyErrorCnt != 0)
    {
        DB_ForceSaveSimpleSSO::makeRequest(get_acc_id(), 0);
    }
    m_humanCertifyErrorCnt = 0;
    setHumanCertified(true);
    return true;
}

// ORIG 0x08649b44：验证三段 CleanPad 种子和是否等于上报校验值。
#include "W8_CU_G/VerifyCleanPadVeriData.inc"
{
    // ORIG 0x08649b44：sum 槽先 `movl $0x0,-0x4(%ebp)` 预清，再 A+B+C 存回。
    // P197③双槽置0：声明=0 + 独立赋值两段式（合并初始化式 4.4 丢预清行）。
    unsigned int sum = 0;
    sum = static_cast<unsigned int>(m_mcap.m_cleanPadVeriA) +
          static_cast<unsigned int>(m_mcap.m_cleanPadVeriB) +
          static_cast<unsigned int>(m_mcap.m_cleanPadVeriC);
    if (value == sum)
        return true;
    return false;
}

// ORIG 0x0868a51a：向客户端下发人机认证图（capcha 队列随机一条 + 新鲜种子三段
// + 错误计数），并重置请求态/失败标志与 60 秒认证超时定时器（TIMER_MESSAGE 9）。
// [b22判词·N] ORIG 155 vs 重建 154：唯一真差异在 memcpy 首参（dst = this+0x8e0ec，
// m_capchaCertify 数组）地址物化——ORIG 4.1.2 `mov 0x8(%ebp),%edx; add $0x8e0ec,%edx`
// （mov+add 两条），4.4.7 折叠为 `lea 0x8e0ec(%eax),%edx`（单条），差 1 条属跨版本
// 寻址伪影（范式库 memcpy/memset 首参 lea-vs-add 族，禁 register asm 强凑）。其余指令/
// 控制流/栈槽/EH landing pad 全对齐；call 目标占位、数据地址 `movl $0x0,(%esp)`
// （&s_capchaDataManager）、字符串立即数为 --no-compile .o 重定位假阳性（§6.3），
// relink 后转 AE。语义等价。
// b28d-W8(2026-09-09): DIFF 155/154（gap 1）— 本波修**真 bug**（p167 重载选错）：
// `put_binary(reinterpret_cast<char*>(m_image), size)` 选中 `EPci` 非 const 重载；
// ORIG 调 `_ZN18InterfacePacketBuf10put_binaryEPKci`（const char*）⇒ 改
// `reinterpret_cast<const char*>`，p167 转 CLEAR。余 1 条 N 族：
// ①memcpy 首参 dst(this+0x8e0ec) ORIG `mov;add` 两步 vs 4.4 `lea` 单条（N107 负例）；
//    b28d-W8b 复测 `&m_capchaCertify[0]` / `reinterpret_cast<char*>(&数组)` 两形
//    仍产 lea（154 不变）⇒ 封 N；
// ②+0x8e0f8 成员读 ORIG `mov mem,%eax; mov %eax,%esi` 两段 vs 4.4 单段（N18a）；
//    b28d-W8b 复测实参 `static_cast<unsigned int>(m_humanCertifyTimerKey)` 形
//    无效（154 不变）⇒ 封 N。
#include "W8_CU_G/reqSendMailCertify.inc"
{
    if (GlobalData::s_capchaDataManager.isEmpty())
    {
        return 0;
    }

    MSG_UDP_CAPCHA_CERTIFY* data = GlobalData::s_capchaDataManager.peekData();
    // b28d-W8b 复测（均无效，保持原形）：ORIG 0x0868a557 `mov 0x8(%ebp),%edx;
    // add $0x8e0ec,%edx` 两步取址 vs 4.4 `lea 0x8e0ec(%eax),%edx` 单条
    //（N107 负例方向）；`&m_capchaCertify[0]` / `reinterpret_cast<char*>(&数组)`
    // 两形实测均仍产 lea（154 不变）⇒ 封 N。
    memcpy(m_capchaCertify, data, 10);

    PacketGuard packet;
    packet.put_header(0, 0x7d);
    packet.put_byte(5);
    packet.put_int(data->m_size);
    // b28d-W8 真 bug（p167 重载选错）：ORIG 调 `put_binary(char const*, int)`
    //（_ZN18InterfacePacketBuf10put_binaryEPKci）；我方 `reinterpret_cast<char*>`
    // 选中非 const 重载 `put_binary(char*, int)`（EPci）⇒ 显式 const 版本。
    packet.put_binary(reinterpret_cast<const char*>(data->m_image), data->m_size);
    packet.put_byte(0);

    unsigned short veriA = 0;
    unsigned char veriB = 0;
    unsigned short veriC = 0;
    GenerateCleanPadVeriData(veriA, veriB, veriC);
    packet.put_short(static_cast<int>(veriA));
    packet.put_byte(static_cast<int>(veriB));
    packet.put_short(static_cast<int>(veriC));

    // ORIG 0x086530xx：`mov 0x8e0fc(%eax),%eax; movzbl %al,%eax` = int 字段
    // **uchar→int 双段截断**形（probe p18.z1：`(int)(unsigned char)` 直落产
    // movzbl %al；`& 0xff` 形产 and——两 lowering 指令名不同）。
    packet.put_byte(static_cast<int>(static_cast<unsigned char>(m_humanCertifyErrorCnt)));
    packet.put_byte(1);
    packet.finalize(true);
    Send(packet);

    setHumanCertified(false);
    m_field8e3ed = false;

    if (static_cast<unsigned int>(m_humanCertifyTimerKey) > 0x7ffffffeu)
    {
        m_humanCertifyTimerKey = 0;
    }
    ++m_humanCertifyTimerKey;

    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(0),
                                static_cast<int>(GetUID()),
                                static_cast<TIMER_MESSAGE>(9), 0x3c,
                                m_humanCertifyTimerKey, 0xffffffffu);
    return 1;
}

// ===========================================================================
// 安全奖励物品发放（sendSecuRewardItem）
// ORIG 0x0868833c：交易中或背包满 → 系统邮件退还；否则入背包并回填落位。
// ===========================================================================
// b28d-W8(2026-09-09): **NEAR 149/149**（DIFF→NEAR）— 本波 5 修：
// ①返回类型 void→bool（CUser.h:2976 同步，P159 尾 `mov $0/%1,%eax`）；
// ②语句序照 ORIG：先拷贝 Inven_Item 局部（0x8688347）再判空（0x86883aa，P171 照抄）；
// ③body 0x100（非 0xff）⇒ 帧/槽位与 ORIG 只差 1B；failed 改 char（`cmpb $0` 直比）；
// ④CheckInTrade 正向 `if(...)` + else 成功路径（`test %al,%al; je` 无 xor）；
// ⑤P47 块序：`if(failed){失败; return false}` + 成功 fall-through；ReqDBSend 内联实参。
// b28d-W8b: **NEAR→AE**（149/149）— title 数组 0x14→0x15（ORIG -0x22..-0xe 共
// 0x15 字节 = strncpy 长度 0x14 + 终止符）⇒ body 起始精确落 -0x15f，1B 槽平移消失。
// p167 CLEAR(9=9)，gap 0。ORIG 0x0868833c。
#include "W8_CU_G/sendSecuRewardItem.inc"
                               int* outSlotVec)
{
    // b28d-21：p167 MISSING strncpy×2/strlen（ORIG 0x086884a6/0x086884c4 两次
    // strncpy 长度立即数 0x14/0xff）⇒ 还原栈缓冲形：body@-0x15f(0xff)、
    // local@-0x5f、title@-0x22(0x14)，ReqDBSendNewSystemMail 传 title/body。
    // b28d-W8: 声明序/语句序照 ORIG —— 0x08688347 先拷贝 Inven_Item 局部、
    // 0x086883aa 才判空（P171 原码遗留照抄）；failed/slot 的初始化在判空之后
    //（0x086883ba/be）。返回类型 void→bool（P159 尾 `mov $0/%1,%eax`）。
    char body[0x100];             // ORIG -0x15f..-0x60（256B，Ghidra local_163[256]）
    Inven_Item local(*item);      // ORIG 将 Inven_Item 原样拷贝入栈局部 -0x5f
    // b28d-W8b: ORIG -0x22..-0xe 共 0x15 字节（strncpy 长度 0x14 + 终止符 1）
    // ⇒ 数组 0x15 使 body 起始精确落 -0x15f（原 0x14 形整帧浅 1B）。
    char title[0x15];             // ORIG -0x22

    if (item == 0)
    {
        return false;
    }

    // b28d-W8: char 而非 bool —— ORIG 0x08688486 `cmpb $0x0,-0xd(%ebp); je`
    // 内存直比（bool 局部 4.4 恒 movzbl;xor $1;test 三段，P158/N122）。
    char failed = 0;
    int slot = -1;

    // b28d-W8: ORIG 0x086883d0 `call; test %al,%al; je <成功路径>`（无 xor）⇒ 正向
    // `if (CheckInTrade())` + else 成功路径（P292 表：bool callee 正向直判 2 条）。
    if (CheckInTrade())
    {
        failed = 1;
    }
    else
    {
        slot = getCurCharacInvenW()->insertItemIntoInventory(
            local, static_cast<eItemAddReason>(0xd), true, true);
        if (slot == -1)
        {
            failed = 1;
        }
    }

    // b28d-W8: P47/P214① —— ORIG 0x08688486 `cmpb $0; je <成功岛>` ⇒ 失败路径
    // fall-through、成功路径在 je 目标（块序与 `if(!failed) 成功` 相反）。
    if (failed != 0)
    {
        // ORIG rodata：DAT_08cf10ec（"아이템 지급" 类标题）/ DAT_08cf10fc（正文），
        // GBK/EUC 多字节原样字节保真转写（objdump -s .rodata 实证）。
        static const char kSenderTitle[] =
            "\260\355\272\355\270\260\300\307\040\274\261\271\260";
        // [S3-W8 2026-09-11 §6.5 真 bug] ORIG 0x8cf10fc 逐字节 = 40B
        //   b0 ed ba ed b8 b0 c0 c7 20 bc b1 b9 b0 c0 cc 20 bf ec c6 ed c0 b8
        //   b7 ce 20 b5 b5 c2 f8 c7 cf bf b4 bd c0 b4 cf b4 d9 2e
        //   （EUC-KR "고블린의 선물이 우편으로 도착하였습니다."）。
        //   旧码为八进制转义且错 4 处：缺 `\272\355`（"블"）、`\304` 多 1 字节、
        //   `\360`→`\370`（f0→f8）、`\231`→`\331`（99→d9）⇒ 二进制串与 ORIG 不符。
        //   改用 \xNN 十六进制逐字节保真，避免八进制歧义。
        static const char kFailMessage[] =
            "\xb0\xed\xba\xed\xb8\xb0\xc0\xc7\x20\xbc\xb1\xb9\xb0\xc0\xcc\x20"
            "\xbf\xec\xc6\xed\xc0\xb8\xb7\xce\x20\xb5\xb5\xc2\xf8\xc7\xcf\xbf"
            "\xb4\xbd\xc0\xb4\xcf\xb4\xd9.";

        strncpy(title, kSenderTitle, 0x14);   // ORIG 0x08688490（长度 0x14）
        strncpy(body, kFailMessage, 0xff);    // ORIG 0x086884ab（长度 0xff）
        // b28d-W8: P52/P134 —— ORIG 0x086884cf/4df/4ec 三值入 %ebx/%esi/%eax 跨
        // 调用保活、装载段直读寄存器；具名局部引入 3 对 store/reload（P0 直传）。
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, local, 0, getCurCharacNo(), body,
            static_cast<int>(strlen(body)), 0,
            static_cast<ENUM_SERVER_GROUP>(GetServerGroup()), false, false);
        outSlotVec[static_cast<int>(type)] = -1;
        return false;
    }

    outSlotVec[static_cast<int>(type)] = slot;
    return true;
}

// ORIG 0x0867d86e：账务仓库 DB 同步转发给内嵌 CAccountCargo。
#include "W8_CU_G/SetAccountCargoDBData.inc"
                                  unsigned int capacity)
{
    m_accountCargo.SetDBData(this, items, money, capacity);
}

// ===========================================================================
// Avenger 计划（复仇者转职前礼物）事件
// ORIG 0x0868b552：事件码 0x8e + 创建时间窗判定，job==4 且未成长一级角色首次
// 登录 → 发放 0x28d3af 物品邮件并置 flag/DB 记录。
// ===========================================================================
// b28d-W8(2026-09-09): **gap 2→0（340/340）** — 本波 5 修：
// ①P0/P183 删 period/period2 具名局部（findGameScript 结果 %eax 直传 receiver，
//   省 2 对 store/reload）；
// ②P28 声明序 body(0x100)先于 subject(0x15)（清零块序照 ORIG 0x867b7e2/82c）；
// ③P52/P134 ReqDBSendNewSystemMail 与 makeRequest 三值内联实参（%ebx/%esi/%eax
//   跨调用保活，删 6 对槽往返）；
// ④P227 trace 全表达式临时链 + itemIdx 去 const（复现 0x868b9ae 槽读 %edi）；
// ⑤getFlagPresentAvengerTitle 返回 uchar→bool（CUser.h:3240/CUser.cpp:11353，
//   SweepCUser2 代落）+ `!f()` ⇒ 0x868b68b `xor $1` 复现。
// p167 CLEAR（仅 C1/C2 别名）；余 isApplied 段 1 条 xor（P168a③b 跨版本 bool ABI）
// + 槽号整体平移。ORIG 0x0868b552。
#include "W8_CU_G/VerifyPresentAvengerTitle.inc"
{
    // ORIG T 0x868b552 全录（报告 docs/class_func_reports/CUser/VerifyPresentAvengerTitle.md
    // 汇编+Ghidra C 双段读完；调用序 Counter 48 项对齐重建 2026-09-05 CUserC8）：
    //  accId 先取 → createTime → 临时 string 两段（allocator C1/string C1/dtor）
    //  → Singleton Get → findGameScript → checkCreateTime(0x8e, createTime, accId)
    //  → 三 guard 短路 → getCurSec → flag!=1 才 **第二次独立** Get/findGameScript
    //    +isApplied（orig 各 2 次，勿复用 period）→ Inven_Item C1 → **先写
    //    item.m_random.m_rand4=0x28d3af 再 create_item**（判据读同一 local_90 槽，
    //    原 const 局部为语义误植）→ findString/strncpy 交替对 → ReqDBSend 系
    //    参数序 group→strlen→characNo → makeRequest 系 characNo→acc→GetUID →
    //    trace 段 idxVal→characNo→acc→NumberToString→C1→operator()。
    // 判据⑤（X 族边界补注 CUserC8）：ORIG 0x868b558-5f accId→%ebx、createTime→
    // %esi 双 callee-saved 跨 string/Get/findScript 四 call 存活（push esi/push ebx
    // prologue 0x868b559-560）。b28d-21：按 P0/N96 清除 `asm("reg")` 绑定，
    // 改纯 register 存储类提示（P231/P248，非绑定形）。
    // b28d-W8b 复测：P248 裸 register 位置旋钮无效 —— 仅 createTime 保留 register
    // 时 accId 被槽化（-0x24）+ createTime→%ebx，341/340 反劣已回退；双 register
    // 形 4.4 反配（accId→%esi/createTime→%ebx）⇒ N7 封顶。
    register unsigned int accId = (unsigned int)get_acc_id();
    register unsigned int createTime =
        static_cast<unsigned int>(getCurCharacCreateTime());
    // b28d-W8: P0/P183 —— ORIG 0x868b5ac findGameScript 结果 %eax 直传 checkCreateTime
    // receiver（无 period 槽往返）⇒ 内联表达式，不设具名局部。
    if (static_cast<ARAD::Arad_EventPeriodDataManager*>(
            ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                std::string("Arad_EventPeriodDataManager")))
            ->checkCreateTime(static_cast<ENUM_REPEAT_EVENT_CODE>(0x8e),
                              createTime, accId) != 1)
    {
        return 0;
    }

    if (get_charac_level() < 0x14 || get_charac_job() != 4 ||
        getCurCharFirstGrowType() != '\0')
    {
        return 0;
    }

    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();

    // b28d-W8: ORIG 0x868b676..0x868b768 —— A 假/B 假同落 L_false（mov $0,%eax），
    // 条件值经 %edi 跨 string+allocator 析构存活后才 test ⇒ 原码是
    // `if (!(A && B)) return 0;`（条件临时无具名槽，P0；eventOn 具名局部是误建模）。
    if (!(!getFlagPresentAvengerTitle() &&
          static_cast<ARAD::Arad_EventPeriodDataManager*>(
              ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                  std::string("Arad_EventPeriodDataManager")))
              ->isApplied(static_cast<ENUM_REPEAT_EVENT_CODE>(0x8e), curSec)))
    {
        return 0;
    }

    Inven_Item item;
    int itemIdx = 0x28d3af;         // [b28d-C08 · C05② 旋钮**两态已试毕**，勿再烧] ORIG
                                    // 0x868b7b0/7ba
                                    // 为 `mov -0x8c(%ebp),%eax; test %eax,%eax; je` +
                                    // `cmp $0xffffffff,%eax`（寄存器比较），曾按 P209/
                                    // P218 档② 改 unsigned 期望翻正；fresh 实测 int/
                                    // unsigned **两态同形**（widthscan cmp_l 恒 our=2）
                                    // —— 本件是「test 零比较 + cmp $imm 常量比较」双比较点，
                                    // itemIdx 在零比较点后仍被复用（日志 `itemidx(%u)`，
                                    // ORIG 0x868b9ae `mov -0x8c(%ebp),%edi`）⇒ 按 A12 口径
                                    // 属「⚠ 可试一次」，但**已试尽**：4.4 恒折
                                    // `cmpl $0x0/-0x1c(%ebp)` 内存直比（同 A01 check_error、
                                    // D01 loadFromString 负例）⇒ **N18b 常量比较子型**
                                    // 封顶，回退 int 保持最小改动。
                                    // ORIG 0x868b77c 独立栈槽 -0x8c（非 Inven_Item
                                    // 成员——0x868b77c 与 item@-0x8e 尺寸不重叠实证；
                                    // ChronicleRandomOptionInfo 亦无 +1 偏移 int 成员）
                                    // b28d-W8: 去 const —— ORIG 0x868b9ae 从槽
                                    // `mov -0x8c(%ebp),%edi` 读回，4.4 对 const 常量
                                    // 传播成 `movl $0x28d3af,0x10(%esp)`（少 1 条 + 无槽）。
    // ORIG 0x868b78b `mov 0xc(%eax),%eax` 直读 m_itemList 成员（getItemList
    // 转发 call orig=0，具名字段直访）。
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(0), item, 1);
    if (itemIdx != 0 && itemIdx != -1)
    {
        setFlagPresentAvengerTitle(true);

        // ORIG 0x868b7e2/82c 两段 peel+rep stos 清零块 = `= {0}` 聚合初始化
        //（makeGuildItemUpgradMessage board 同形，4.4 产 memset 调用 orig=0）。
        // b28d-W8: 声明序照 ORIG —— body(0x100)@0x868b7e2 先清零、subject(0x15)
        // @0x868b82c 后清零（P28 声明序即发射序；原序反了）。
        char body[0x100] = {0};
        char subject[0x15] = {0};
        strncpy(subject,
                g_scriptStringManager_.findString(4, "ready_to_avenger_title", 0),
                0x14);
        strncpy(body,
                g_scriptStringManager_.findString(4, "ready_to_avenger_mail", 0),
                0xff);

        // b28d-W8: P52/P134 —— ORIG 0x868b906/91c/923 三值入 callee-saved
        //（group→%ebx、strlen→%esi、characNo→%eax）跨调用保活、装载段直读寄存器；
        // 具名局部引入 3 对 store/reload（P0 表达式直传）。
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            subject, item, 0, getCurCharacNo(), body,
            static_cast<unsigned int>(strlen(body)), 0,
            static_cast<ENUM_SERVER_GROUP>(GetServerGroup()), false, false);

        // b28d-W8: P52/P134 —— ORIG 0x868b979/986/993 三值 %esi/%ebx/%eax 直传，
        // 无 characNo2/accId2/uid 具名槽。
        DB_SaveGiveAvengerTitleFlag::makeRequest(
            (int)GetUID(), get_acc_id(), getCurCharacNo());

        // b28d-W8: P227 全表达式临时链 —— ORIG 0x868b9ae..0x868ba1b 在 cMyTrace
        // ctor 前右起求值 itemIdx(%edi)→characNo(%esi)→NumberToString(%ebx)，
        // 三值全程寄存器无槽 ⇒ 原码无具名局部（P0）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa6eb, 0)(
            "avenger plan event item post m_id(%s),charac_no(%u), itemidx(%u)",
            NumberToString(get_acc_id(), 0), getCurCharacNo(), itemIdx);
        return 1;
    }
    return 0;
}

// ===========================================================================
// 经验主链
// ===========================================================================
// ORIG 0x0868b20c：结算前后 SP 快照 + gain_exp_sp；升级后按场景广播基础信息包，
// 成长事件触发 decide_growth_power_reward_system（producer=CUserOnlyOrig.cpp）。
#include "W8_CU_G/rewardExp.inc"
                      int& spBefore, int& spAfter,
                      ::eExpAddReason reason, bool triggerGrowth)
{
    PacketGuard packet;

    // P52/P24/P134：`spBefore = skillR->f(index)` 单语句内联实参，索引值先物化
    // callee-saved %ebx 并跨 getCurCharacSkillR 调用存活，不落栈槽（ORIG %ebx 形态）。
    spBefore = reinterpret_cast<SkillSlot*>(const_cast<char*>(getCurCharacSkillR()))
        ->get_remain_sp_at_index(
            static_cast<ENUM_SKILL_TREE_KIND>(GetCurCharacSkillTreeIndex()));

    calcHelpAbuseRatio(static_cast<unsigned int>(expBase), false);
    bool levelUp = gain_exp_sp(expBase, bonusA, bonusB,
                               (eExpAddReason)reason, 0, triggerGrowth);

    spAfter = reinterpret_cast<SkillSlot*>(const_cast<char*>(getCurCharacSkillR()))
        ->get_remain_sp_at_index(
            static_cast<ENUM_SKILL_TREE_KIND>(GetCurCharacSkillTreeIndex()));

    // 极性：`if (levelUp) {...}` 使 4.4 发 `cmpb $0x0,-0xd(%ebp); je`（ORIG 同款）；
    // `if (!levelUp) return;` 早退会发 movzbl/xor $1/test 反形。
    if (levelUp)
    {
        if (m_chState == 3)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(0);
            packet.put_short(1);
            make_basic_info(reinterpret_cast<char*>(&packet), '\0');
            packet.finalize(true);
            G_GameWorld()->send_all(packet, this);
        }
        else if (m_chState == 5 || m_chState == 8)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(1);
            packet.put_short(1);
            make_basic_info(reinterpret_cast<char*>(&packet), '\1');
            packet.finalize(true);
            CParty* party = static_cast<CParty*>(GetParty());
            if (party != 0)
            {
                party->send_to_party(packet);
            }
        }

        if (triggerGrowth)
        {
            decide_growth_power_reward_system();
        }
    }
}

// ORIG 0x0866a3fe：经验结算主流程 —— 入口节流（环境 workmode==2 时 49 级以上不
// 入经验）、累计字段、升级判定（_check_level_up，producer=CUserCheckLevel.cpp）、
// SP/SFP 返还与日志、升级后任务/公会/监控器通知、疲劳奖励 channel 组收尾、
// 专家职业经验结算与 55 级清动作点。
// b28d-W8(2026-09-09): DIFF 388/389（gap 4→1）— 本波 4 修：
// ①P0/P183 去 questR 具名局部（ORIG 0x0866a5xx getCurCharacQuestR 结果 %eax 直传
//   get_quest_info receiver，省 store+load 2 条）；
// ②guild/monitor 两段 getCurCharacGrowType 加 uchar→char 转换复现 ORIG
//   0x0866a67d/0x0866a6fb `movsbl %al,%eax`（具名局部形 396 反劣已弃）；
// ③P227 全表达式临时链复现 VerifyPresentAvengerTitle 失败分支 trace
//   （0x0866a77b..0x0866a7e0 ctor 前右起求值 characNo→NumberToString）；
// ④VerifyPresentAvengerTitle 返回 char→bool（CUser.h:3065 同步）⇒ 调用点
//   `call; xor $1; test; je` 与 ORIG 0x0866a774 全等。
// b28d-W8b: **DIFF→AE**（388/388）— 帧 0x7c→0x6c 精确对齐：①P329 去
// playCount/level/expertExp 三具名局部（ORIG 0x0866a8a0 段全程寄存器/实参位直传）；
// ②P184/P171 补第二个独立 `if (levelUp)`（ORIG 0x0866a96c `je +0x56e` 与检查 A
// 的 `je +0x522` 分离）+ 两条死局部常量 `movl $0x55,-0x20; movl $0x50,-0x1c`。
// 66/66 调用点全等（p167 CLEAR）。ORIG 0x0866a3fe。
#include "W8_CU_G/gain_exp_sp.inc"
                        eExpAddReason reason, int param, bool flag)
{
    // 环境 work-mode==2（G_CEnvironment()+0x1a8 具名化见 CEnvironment.h:m_field1a8）
    // 时，49 级以上角色不入经验。
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 2)
    {
        if (get_charac_level() > 0x31)
        {
            exp = 0;
        }
    }
    if (exp < 0)
    {
        exp = 0;
    }
    // b28d-solo-W8_CU_G：ORIG 0x0866a4xx 对形参直读两次
    // `cmpl $1,0x18(%ebp)…; cmpl $2,0x18(%ebp)`（无槽往返，P0）⇒ 去 reasonVal
    // 具名局部，使用点内联 cast 表达式。
    if (static_cast<int>(reason) == 1 || static_cast<int>(reason) == 2)
    {
        m_character.m_f34 += exp;
    }
    m_character.m_f28 += exp;
    incPlayExpAdd(exp);

    bool levelUp = false;
    if (get_charac_level() < 0x55)
    {
        levelUp = _check_level_up(exp, sp, sfp, reason, param);
        // b28d-solo-W8_CU_G：ORIG 0x0866a5xx getCurCharacSkillR() 结果直接作 receiver
        //（无 skillR 具名局部/槽，P0 表达式直传）。
        int spBefore = reinterpret_cast<SkillSlot*>(
            const_cast<char*>(getCurCharacSkillR()))->
            get_remain_sp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(0));
        gain_sp(sp);
        int sfpBefore = reinterpret_cast<SkillSlot*>(
            const_cast<char*>(getCurCharacSkillR()))->
            get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(2));
        gain_sfp(sfp);
        history_log_sp(spBefore, sp, eSPAddReason_0);
        history_log_sfp(sfpBefore, sfp, eSPAddReason_0);
    }

    PacketGuard questPacket;
    if (levelUp)
    {
        CalcurateUserMaxLevel();

        // 当前等级追平最高装备等级时清除该标记。
        if (GetCurCharacMaxEquipLevel() != 0 &&
            get_charac_level() == GetCurCharacMaxEquipLevel())
        {
            SetCurCharacMaxEquipLevel(0);
        }

        getCurCharacQuestW()->ResetUrgentQuestWaitingList();
        // b28d-W8: P0/P183 —— ORIG 0x0866a5xx getCurCharacQuestR 结果 %eax 直传
        // get_quest_info receiver（ORIG 无 questR 具名槽，我方原码多 store+load 2 条）。
        getCurCharacQuestR()->get_quest_info(reinterpret_cast<char*>(&questPacket));
        m_characterView.enableSaveCharacView();
        Send(questPacket);

        if (get_charac_level() == 0x55)
        {
            resetCharacFatigueGrownUpBuff();
        }

        _UpdateUserInfo4Guild();

        {
            // b28d-W8: ORIG 0x0866a67d `call; movsbl %al,%eax` ⇒ uchar→char 有符号
            // 转换（直接 static_cast<short>(uchar) 产 movzbl）；具名局部形实测
            // 396 条（+6）反劣，维持内联。
            GlobalData::s_guild_proxy_mgr->GetServerProxy(
                static_cast<ENUM_SERVER_GROUP>(GetServerGroup()))->
                SendCharLevelGrowType(get_acc_id(),
                                      static_cast<unsigned int>(get_charac_no(-1)),
                                      static_cast<short>(get_charac_level()),
                                      static_cast<short>(static_cast<char>(
                                          getCurCharacGrowType())));
        }
        {
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                static_cast<ENUM_SERVER_GROUP>(GetServerGroup()))->
                SendCharLevelGrowType(get_acc_id(),
                                      static_cast<unsigned int>(get_charac_no(-1)),
                                      static_cast<short>(get_charac_level()),
                                      static_cast<short>(static_cast<char>(
                                          getCurCharacGrowType())));
        }

        if (!VerifyPresentAvengerTitle())
        {
            // b28d-W8: P227 全表达式临时链 —— ORIG 0x0866a77b..0x0866a7e0 在 ctor
            // 前右起求值 characNo(%esi)→NumberToString(%ebx)，无具名槽（P0）。
            cMyTrace(__PRETTY_FUNCTION__, 0x5046, 0)(
                "CUser::VerifyPresentAvengerTitle() m_id(%s), charac_no(%u)",
                NumberToString(get_acc_id(), 0), getCurCharacNo());
        }

        AddCurCharacMercenaryInfo();
    }

    // 疲劳奖励 channel 组（5/8/7/10/12）。
    if (m_chState == 5 || m_chState == 8 || m_chState == 7 ||
        m_chState == 10 || m_chState == 12)
    {
        if (!flag)
        {
            SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                           static_cast<ENUM_NOTIPACKET>(0x25), 0);
            set_charac_fatigue_buf_bonus_exp(0);
        }
        else if (levelUp)
        {
            SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                           static_cast<ENUM_NOTIPACKET>(0x25), 0);
        }
        if (levelUp)
        {
            decide_growth_power_reward_system();
        }
    }

    if (levelUp)
    {
        // b28d-W8b: P329/P183 —— ORIG 0x0866a8a0 段无具名槽：playCount 先算留 %ebx
        // 跨 call、level 后算留 %eax、receiver s_statisticMgr 载 %edx（0x0866a8c1
        // `mov 0x941f764,%edx`）；expertExp 亦直压 0x4(%esp)（0x0866a8f0）。
        // 具名局部形多 3 槽 + 帧 0x10（389/388）⇒ 内联回实参位。
        GlobalData::s_statisticMgr->m_levelDungeonPlay.IncreaseLevelDungeonPlay(
            get_charac_level(), GetCurCharacDungeonPlayCount());
        ResetCurCharacDungeonPlayCount();

        expert_job::CExpertJob::IncreaseExpertJobExp(this,
                                                     GetCurCharacExpertJobExp());

        ReCalcChattingEmoticon();
        SendChattingEmoticon();
    }

    // b28d-W8b: P184/P171 —— ORIG 0x0866a96c 有**第二个独立** `if (levelUp)`
    //（`cmpb $0x0,-0x29; je +0x56e`，与检查 A 的 `je +0x522` 分离，4.1 不
    // jump-thread 合并）；块首两条死局部常量 0x0866a972/978 `movl $0x55,-0x20;
    // movl $0x50,-0x1c`（全程无读取，原码遗留照抄，P171）。
    if (levelUp)
    {
        int legacyMaxLevel = 0x55;
        int legacyMaxLevel2 = 0x50;
        (void)legacyMaxLevel;
        (void)legacyMaxLevel2;
        if (get_charac_level() == 0x55)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                this, APSystem::_ActionGroupIndex(0x21),
                0, static_cast<ENUM_PACKETCLASS>(0));
        }
    }
    return levelUp;
}

// ===========================================================================
// Inform notice flag 族（m_informNoticeFlags 16 字节 × 8 bit 标志板）
// ===========================================================================
#include "W8_CU_G/getInformNoticeFlag.inc"
{
    return m_informNoticeFlags[idx];   // ORIG 0x0868fa76 单 movzbl 0x8ebb1(%edx,%eax,1)
}

// b28d-solo-W8_CU_G: DIFF/N 封顶 — ORIG 0x0868f956 三失败出口各落独立 nop 岛
//（+0x83 nop;jmp / +0x86 nop;jmp / +0x89 nop，标签序=守卫序）⇒ P223 负条件
// goto 连续尾标签形复现三岛（探针同 CCharacInformNotice::handleInformNoticeFlag
// 同族定案 0x080e4806）。残差 N：①ORIG 读值/和值各多一条寄存器中转
//（`movzbl→%edx; mov %edx,%ebx` / `lea→%edx; mov %edx,%ecx`），4.4 恒折单段
//（N18a 正向，字段/RHS 均已异型仍不可造）；②第三守卫 ORIG `test %al,%al; je`
// vs 4.4 `xor $0x1; test; jne`（P168a①/P292 bool 归一，无绑定不可造）。
// 55/54，语义等价。
#include "W8_CU_G/handleInformNoticeFlag.inc"
{
    // P133：ORIG 三守卫各自 jne/je 直跳统一失败出口（bit>7 → byte 计算 →
    // byte>0xf → check），独立 if 不可合并短路。
    unsigned int bit = bitIndex & 7;
    if (bit > 7)
        goto L1;
    {
        unsigned int byte = bitIndex >> 3;
        if (byte > 0xf)
            goto L2;
        // b28d-W8: ORIG 0x0868f996 `call; test %al,%al; je <岛>`（bool callee 直判、
        // 无 xor）；4.4 对 `!call()`/`call()==0` 恒产 `xor $1; test; jne`（+1 条）⇒
        // P168a③b 跨版本 bool ABI 封 N；正极性 if 包裹实测 51（少 4 条，L1/L2 岛被
        // jump-thread），if/else+goto 形同（b28d-W8b 复测 51）已回退。
        if (checkInformNoticeFlag(bit, byte) == 0)
            goto L3;
        // ORIG lea (%ebx,%edx,1) 加法形 ⇒ 源码 +（位不重叠与 | 等价）。
        m_informNoticeFlags[byte] = m_informNoticeFlags[byte] + (1 << bit);
        enableSaveInformNoticeFlag();
        goto END;
    }
L1:;
L2:;
L3:;
END:;
}

// b28d-solo-W8_CU_G: NEAR/N — P285 空 else{return;} 复现 ORIG 0x0868f900 尾
// `jmp;nop` 岛（ndiff 13→3）。b28d-W8 复验：28/29（ORIG 28），残 1 条 = 循环体
// ORIG 单段 `movzbl (%edx),%ecx`，4.4 恒 `movzbl (%edx),%edx; mov %edx,%ecx` 两段
//（N18a 反向）；b28d-W8b 复测三形（隐式转换 / 显式 (unsigned char) cast /
// P266 `*reinterpret_cast<unsigned char*>(src + i)`）同为 29 ⇒ 封 N，不再试形。
#include "W8_CU_G/loadInformNoticeFlag.inc"
{
    resetInformNoticeFlag();
    if (src != 0)
    {
        for (int i = 0; i < 0x10; ++i)
        {
            // b28d-W8: ORIG 0x0868f929 单段 `movzbl (%edx),%ecx`；4.4 恒
            // `movzbl (%edx),%edx; mov %edx,%ecx` 两段（+1 条，N18a 反向）；
            // P266 uchar 左值读旋钮 / 隐式转换两形实测同为 29 ⇒ 封 N。
            // b28d-W8b 复测：显式 (unsigned char) cast 无效（同 29）⇒ P266 左值读旋钮。
            m_informNoticeFlags[i] = *reinterpret_cast<unsigned char*>(src + i);
        }
        sendInformNoticeFlag();
    }
    else
    {
        return;
    }
}

#include "W8_CU_G/getInformNoticeFlagIndex.inc"
{
    out.clear();
    for (int byteIdx = 0; byteIdx < 0x10; ++byteIdx)
    {
        unsigned char bits = static_cast<unsigned char>(getInformNoticeFlag(byteIdx));
        for (int bitIdx = 0; bitIdx < 8; ++bitIdx)
        {
            if (((bits >> bitIdx) & 1) != 0)
            {
                out.push_back(static_cast<unsigned char>(byteIdx * 8 + bitIdx));
            }
        }
    }
}

#include "W8_CU_G/sendInformNoticeFlag.inc"
{
    std::vector<unsigned char> indexes;
    getInformNoticeFlagIndex(indexes);

    PacketGuard packet;
    packet.put_header(0, 0x1a0);
    packet.put_byte(static_cast<int>(indexes.size()));
    for (size_t i = 0; i < indexes.size(); ++i)
    {
        packet.put_byte(static_cast<int>(indexes[i]));
    }
    packet.finalize(true);
    Send(packet);
}

// ===========================================================================
// 连续效果道具（effect / cooltime）载入与下发
// ===========================================================================
// ORIG 0x08659366：0xae 包头 + 数量 + 逐条目{idx, expire-now}下发。
// b28d-solo-W8_CU_G: IDENTICAL/AE — ①循环体恢复两个具名局部（ORIG 0x086593de/
// 0x086593f5 两次 operator[] 结果落 -0x10/-0xc 槽，P0 槽往返判据），帧/PacketGuard
// 槽随之回到 -0x28；②循环变量 unsigned→int（P218 扩展：4.4 尾测由 `mov i,%edx;
// cmp %eax,%edx; setl` 折回 ORIG `movzbl count,%eax; cmp i,%eax; setg`）；
// ③返回类型 int→bool（P248 档①，Main 代落 CUser.h:2835）⇒ `mov %eax,%ebx` 直传。
// 90/90 逐条 EQ。
#include "W8_CU_G/SendEffectItemInfo.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0xae);
    std::vector<ContinuousItemInfo>* list = GetCurCharacEffectItemList();
    unsigned char count = static_cast<unsigned char>(list->size());
    packet.put_byte(static_cast<int>(count));
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        // b28d-solo-W8_CU_G：ORIG 0x086593de/0x086593f5 两次 operator[] 各存
        // 具名局部（-0x10 idx / -0xc expire，P0 槽往返实证）⇒ 恢复具名局部。
        unsigned int idx = (*list)[i].m_itemIdx;
        unsigned int expire = (*list)[i].m_expireTime;
        packet.put_int(idx);
        packet.put_int(static_cast<int>(expire - itemIdx));
    }
    packet.finalize(true);
    return Send(packet);
}

// ORIG 0x086591fe：0xac 包头 + cooltime 表项迭代下发。
// b28d-solo-W8_CU_G: IDENTICAL/AE — ①`it = list->begin()` 提前到 size()/put_byte
// 之前（ORIG 0x0865923a begin 的 sret 直落 -0x30 it 槽）；②循环体恢复两个具名
// 局部（ORIG 0x0865927d/0x0865928d 两次 operator-> 结果落 -0x10/-0xc 槽）；
// ③返回类型 unsigned int→bool（P248 档①，Main 代落 CUser.h:3574）。帧 0x40、
// 槽位（-0x2c guard/-0x30 it/-0x18 list/-0x11 count）与 ORIG 全等，106/106 EQ
//（ORIG 尾 1 条对齐 nop，§6.1 豁免）。
#include "W8_CU_G/SendCoolTimeItemInfo.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0xac);
    std::vector<ContinuousItemInfo>* list = GetCurCharacCoolTimeItemList();
    // b28d-solo-W8_CU_G：ORIG 0x0865923a begin() 先于 size()/put_byte（P0 槽序
    // 与求值序判据：begin 的 sret 直接落 it 槽 -0x30，无默认 ctor）。
    std::vector<ContinuousItemInfo>::iterator it = list->begin();
    unsigned char count = static_cast<unsigned char>(list->size());
    packet.put_byte(static_cast<int>(count));
    for (; it != list->end(); it++)   // b28d-21：P167 差集 ORIG 为 ppEi（后置，
                                    // P28 增量侧推论），改 it++。
    {
        // b28d-solo-W8_CU_G：ORIG 0x0865927d/0x0865928d 两次 operator-> 结果
        // 各存具名局部（-0x10 idx / -0xc expire，P0 槽往返实证）。
        unsigned int idx = it->m_itemIdx;
        unsigned int expire = it->m_expireTime;
        packet.put_int(idx);
        packet.put_int(static_cast<int>(expire - nowMs));
    }
    packet.finalize(true);
    return Send(packet);
}

// ORIG 0x086590f0：DB 载入 effect 连用表 → 过滤有效条目 → 特效回调 → 清理无效并发送。
// b28d-solo-W8_CU_G: AE — ①P183（删 `const CONT_ENTRY& entry` 缓存引用，每字段重发
// `mov i,%edx; mov info,%eax; mov 0x8(%eax,%edx,8),%eax` 全链）；②守卫「1 直落 + 2 岛」
// 用 P223 多岛形：正向 if 包裹第一守卫（jbe 直跳循环尾）+ 第二/三守卫独立 goto 连续尾
// 标签（L2/L3/NEXT），ORIG 0x8659097/0x865909a 两岛逐条复现；③尾 `!=1` 改 bool 返回
// 旋钮（P248 档①：SendEffectItemInfo int→bool，Main 已代落 CUser.h）⇒ `xor $1; test; je`
// 复现 0x86591e9。81/81，compare_tu 判 AE。
#include "W8_CU_G/LoadEffectItemInfo.inc"
{
    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    std::vector<ContinuousItemInfo>* list = GetCurCharacEffectItemList();
    list->clear();
    for (int i = 0; i < static_cast<int>(static_cast<signed char>(info->m_count)); ++i)
    {
        // b28d-21：ORIG 0x086590f0 每次字段访问都重发
        // `mov i,%edx; mov info,%eax; mov 0x8(%eax,%edx,8),%eax` 全链
        //（P183 地址重物化铁律）⇒ 不得缓存 `const CONT_ENTRY& entry`。
        // 守卫形态：ORIG 第 1 条件 jbe 直跳循环尾（无岛），第 2/3 条件各落独立
        // nop 中继岛（0x8659097/0x865909a）⇒ P223 多岛形：三条独立失败 goto +
        // 连续尾标签（标签序=守卫序）。
        if (info->m_entries[i].m_expireTime > curSec)
        {
            if (static_cast<unsigned int>(info->m_entries[i].m_itemIdx) > 0x7ffffffe)
                goto L2;
            if (info->m_entries[i].m_expireTime > 0x77359400)
                goto L3;
            {
                ContinuousItemInfo ci;
                ci.m_itemIdx = info->m_entries[i].m_itemIdx;
                ci.m_expireTime = info->m_entries[i].m_expireTime;
                _processSpeicalContinuousTimeItem(static_cast<unsigned int>(ci.m_itemIdx));
                list->push_back(ci);
            }
            goto NEXT;
        L2:;
        L3:;
        NEXT:;
        }
    }
    ClearUselessEffectItem(curSec);
    // b28d-21：ORIG 0x086590f0 尾 0x86590da `xor $1,%eax; test; je 0xe8(mov$1)`
    // = 4.1 bool 返回物化；4.4 恒折 `cmp $1; sete`（setcc-fold 族）⇒ N 封顶。
    if (SendEffectItemInfo(curSec) != 1)
        return false;
    return true;
}

// ORIG 0x08658ff6：DB 载入 cooltime 表 → 过滤 → 清理无效 → 下发 0xac 包。
// b28d-solo-W8_CU_G: AE — 同 LoadEffectItemInfo：P183 去缓存引用 + P223 多岛形
//（ORIG 0x08659097/0x0865909a 两岛）+ SendCoolTimeItemInfo bool 返回旋钮
//（P248 档①，Main 代落 CUser.h:3574）⇒ `xor $1; test; je` 复现 0x086590da。
// 76/76 指令（ORIG 尾多 1 条对齐 nop，§6.1 豁免），compare_tu 判 AE。
#include "W8_CU_G/LoadCoolTimeItemInfo.inc"
{
    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    std::vector<ContinuousItemInfo>* list = GetCurCharacCoolTimeItemList();
    list->clear();
    for (int i = 0; i < static_cast<int>(static_cast<signed char>(info->m_count)); ++i)
    {
        // b28d-21：同 LoadEffectItemInfo，ORIG 0x08658ff6 逐字段重发完整
        // imul/lea 寻址（P183）⇒ 不缓存 entry 引用。
        // b28d-solo-W8_CU_G：守卫「1 直落 + 2 岛」（ORIG 0x08659097/0x0865909a）
        // ⇒ 正向 if 包裹（第一守卫 jbe 直跳循环尾）+ 第二/三守卫独立 goto 尾标签。
        if (info->m_entries[i].m_expireTime > curSec)
        {
            if (static_cast<unsigned int>(info->m_entries[i].m_itemIdx) > 0x7ffffffe)
                goto L2;
            if (info->m_entries[i].m_expireTime > 0x77359400)
                goto L3;
            {
                ContinuousItemInfo ci;
                ci.m_itemIdx = info->m_entries[i].m_itemIdx;
                ci.m_expireTime = info->m_entries[i].m_expireTime;
                list->push_back(ci);
            }
            goto NEXT;
        L2:;
        L3:;
        NEXT:;
        }
    }
    ClearUselessCooltimeItem(curSec);
    // b28d-21：ORIG 0x08658ff6 尾 0x86590da `xor $1,%eax; test; je`（setcc-fold）⇒ N。
    if (SendCoolTimeItemInfo(curSec) != 1)
        return false;
    return true;
}

