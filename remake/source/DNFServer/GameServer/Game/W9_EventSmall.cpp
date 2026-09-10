// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W9_EventSmall/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <ctime>
#include "W9_EventSmall.h"
#include "GameTypes.h"       // cMyTrace
#include "GlobalData.h"
#include "CSystemTime.h"
#include "StreamPool.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "CUser.h"

// getDailyInitializeTime：定义 GameUtils.cpp（DB_LoadEtc.cpp 已声明）。
#include "W9_EventSmall/getDailyInitializeTime.inc"

// [b28d-PoolMisc2 GI-q06 纯移位] TU 首符号需为 CTrcOnOffEvent 构造体外联定义
// （ORIG 0x08285a44）⇒ 本定义块上移至本 TU 第一个发射位置。对象声明序一字未动，
// 仅换定义块位置（构造序只由 file-scope 对象声明顺序决定）。
// ---- CTrcOnOffEvent ----
#include "W9_EventSmall/CTrcOnOffEvent.inc"

// ============================ DB 依赖 ============================
// [b28d-lane6] DIFF·N93/N82：槽号与块序（60 vs 63 条）。
#include "W9_EventSmall/makeRequest.inc"
{
    // ORIG T 0x08433ada _ZN34DB_LoadAutoMarketConditionsControl11makeRequestEb
    // [M1S3EmblemLink 2026-09-11 P0] ORIG 0x08433b00 `call Acquire` 结果留在 %eax，
    //   紧接 `movl $0x1,0x8(%esp); mov %eax,0x4(%esp); lea -0x10(%ebp),%eax; …
    //   call CStreamGuard::CStreamGuard(Stream*, bool)` —— 无 `Stream*` 具名槽
    //   （我方原多 -0xc 槽 + 2 条槽往返）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7c43), true);
    guard.operator*()->operator<<(0x15a);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(flag);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    // [M1S3EmblemLink 2026-09-11 P0] ORIG 0x08433b89 正常路径 `jmp 0x08433ba6`
    //   直接进 epilogue（`lea -0x10; call CStreamGuard::~CStreamGuard; add $0x30;
    //   pop ebx/esi/ebp; ret`），**无** `mov $0x1,%ebx` / `mov %ebx,%eax` 返回值
    //   物化 ⇒ ORIG 源码无 return 语句（返回寄存器内容未定义）。删发明 `return true;`。
}

// ============================ 事件基类模板 ============================
// 四个小类的 Start/End 全部为「SetEventFlag + cMyTrace(function 名, len, 0) +
// trace(msg)」同构（各报告反编译一致）。构造/析构只置 vptr（编译器生成），
// IsEventing 为基类透传。

#include "W9_EventSmall/dtor_CTrcOnOffEvent.inc"

#include "W9_EventSmall/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x13, 0);
    trace("Trc On Off Event Start!");
}

#include "W9_EventSmall/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1f, 0);
    trace("Trc On Off Event Event End!");
}

#include "W9_EventSmall/StartEvent_1.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x19, 0);
    trace("Trc On Off Event Event Start!");
}

#include "W9_EventSmall/IsEventing.inc"
{
    // ORIG 0x08285ba4：第二参数恒传 0（NULL），不转发本函数形参。返回仍等于
    // 基类结果，4.1 走「取反+反极性岛」。b28d-lane6 P0 清绑（N96）：删历史
    // `register bool t __asm__("al")` 绑定，改 P168a 裸形 `if(!boolcall())`。
    if (!CEventBase::IsEventing(static_cast<CUser*>(0)))
        return false;
    return true;
}

// ---- CFatigueBuffEvent ----
#include "W9_EventSmall/CFatigueBuffEvent.inc"
#include "W9_EventSmall/dtor_CFatigueBuffEvent.inc"

#include "W9_EventSmall/StartEvent_2.inc"
{
    SetEventFlag(true);
    // [N3 2026-09-11 §6.5 真错] ORIG prett 实读 = "virtual void CFatigueBuffEvent::StartEvent()"
    //   （0x08b51820 size 0x2d，GCC 对虚函数加 virtual 前缀）；旧形 __FUNCTION__ 只展开裸
    //   函数名 "StartEvent" ⇒ 运行期日志文本不同（verdict 恒不变不可见）⇒ 改 __PRETTY_FUNCTION__。
    cMyTrace trace(__PRETTY_FUNCTION__, 0x11, 0);
    trace("CFatigueBuffEvent Event Start!");
}

#include "W9_EventSmall/EndEvent_1.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1b, 0);
    trace("CFatigueBuffEvent Event End!");
}

#include "W9_EventSmall/StartEvent_3.inc"
    // [b28c-27] AE（fresh 复验）：共享头改动后本件 .o 与 ORIG AE 全等
    // （虚调用目标寄存器形在 W9_EventSmall 路由下与 ORIG 一致）。
{
    StartEvent();
}

// ---- CWeekendBonusEvent ----
#include "W9_EventSmall/CWeekendBonusEvent.inc"
#include "W9_EventSmall/dtor_CWeekendBonusEvent.inc"

#include "W9_EventSmall/StartEvent_4.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x12, 0);
    trace("CWeekendBonusEvent Event Start!");
}

#include "W9_EventSmall/EndEvent_2.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1c, 0);
    trace("CWeekendBonusEvent Event End!");
}

#include "W9_EventSmall/StartEvent_5.inc"
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
{
    StartEvent();
}

// ---- CAutoMarketConditionsControlEvent ----
#include "W9_EventSmall/CAutoMarketConditionsControlEvent.inc"
#include "W9_EventSmall/dtor_CAutoMarketConditionsControlEvent.inc"

#include "W9_EventSmall/StartEvent_6.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x13, 0);
    trace("CAutoMarketConditionsControlEvent Start!");
}

#include "W9_EventSmall/EndEvent_3.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1f, 0);
    trace("CAutoMarketConditionsControlEvent End!");
}

#include "W9_EventSmall/StartEvent_7.inc"
{
    SetEventFlag(true);
    DB_LoadAutoMarketConditionsControl::makeRequest(true);
}

// ============================ CCharacBloodDungeon ============================
#include "W9_EventSmall/CCharacBloodDungeon.inc"
{
    // ORIG 0x832af6e：仅调基类 ctor 0x80c8cba + 置 vptr；帧内无字段清零
    // （ORIG 汇编无 memset/逐字段赋值），故与 ORIG 一致不做显式初始化。
}

#include "W9_EventSmall/_reset.inc"
{
    m_point = 0;
    m_weeklyEnterCount = 0;
    m_useGold = 0;
    m_field14 = 0;
    m_enterCount = 0;
    m_field1c = 0;
    m_flag20 = 0;
    m_field24 = 0;
    m_field28 = 0;
}

#include "W9_EventSmall/_saveData.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("CharacBloodDungeon.cpp", 0x1e),
                       true);
    guard.operator*()->operator<<(0x197);       // op*(guard)→op<<(int)
    guard.operator*()->operator<<((int)pUser->GetUID());

    SIG_BLOOD_DUNGEON* sig = guard.operator->()->GetInBuffer<SIG_BLOOD_DUNGEON>();
    memset(sig, 0, sizeof(SIG_BLOOD_DUNGEON));   // ORIG memset 0x28
    // [P211 档③ 试形记录（已回滚）] 删 `(int)` cast 试同型化（ORIG 0x0832a3e7 为
    //   LHS-first 直存）实测 DIFF 数不变（3 件）⇒ 判 N18b/装载序族，保持直证形。
    // [M1S3EmblemLink 2026-09-11 复核封顶] 唯一残余 = 1 条：ORIG 0x0832a3e7
    //   `mov -0xc(%ebp),%edx; mov %eax,(%edx)`（RHS 留 %eax，指针装 %edx 后直存）
    //   vs 4.4 `mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`（多一条
    //   值搬运）⇒ 我方 113 vs ORIG 112。§6.5 串已核（.o 内
    //   "CharacBloodDungeon.cpp" 与 ORIG 0x8c26d80 字节一致）、§6.5.1 不适用
    //   （无 EUC-KR 字面量）；槽位与 ORIG 全等（sig -0xc、guard -0x14）⇒ 非 P339/
    //   P331 域，属 4.4 赋值 lowering（N18b 族）⇒ 封顶。
    // [P6S6 2026-09-11 复核·仍 N] 本波再删 `(int)` cast 实测：`.o` 反汇编
    //   仍是 `mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`（113 vs 112，
    //   blocks=1）⇒ 4.4 恒「地址入 %eax、值入 %edx」，ORIG 4.1 形
    //   `mov -0xc(%ebp),%edx; mov %eax,(%edx)`（值留 %eax 直存）源码不可择 ⇒ 维持 N。
    sig->m_characNo = (int)pUser->getCurCharacNo();
    sig->m_point = m_point;                      // this+0x08 → sig+0x04
    sig->m_weeklyEnterCount = m_weeklyEnterCount;
    sig->m_useGold = m_useGold;
    sig->m_dailyInitTime = getDailyInitializeTime(6, 0, 0);  // 覆写 +0x10
    // ORIG 跳过 this+0x14：sig+0x14←this+0x18、sig+0x18←this+0x1c（与 loadData
    // 镜像一致；原写 this+0x14/0x18 系真错位 bug）。
    sig->m_field14 = m_enterCount;
    sig->m_enterCount = m_field1c;
    sig->m_flag1c = m_flag20;                    // byte 镜像
    sig->m_field20 = m_field24;
    sig->m_field24 = m_field28;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_flag20 = 0;                                // ORIG 发送后清标志
    return true;                                 // 返回 1
}

#include "W9_EventSmall/loadData.inc"
{
    // pData 即 _saveData 落库的 SIG_BLOOD_DUNGEON 记录（ORIG 0x832a4c6 汇编：
    // 8 条 movl，逐条 sig+0x04/08/0c/10/14/18/20/24 → this+08/0c/10/14/18/1c/24/28）。
    // sig+0x00 characNo 不读取；sig+0x1c（m_flag1c byte）不读取，故 this+0x20
    // m_flag20 保持原值 —— 与 getData 非严格互逆，ORIG 本身即如此。
    const SIG_BLOOD_DUNGEON* sig = reinterpret_cast<const SIG_BLOOD_DUNGEON*>(pData);
    m_point             = sig->m_point;              // sig+0x04（int）
    m_weeklyEnterCount  = sig->m_weeklyEnterCount;   // sig+0x08（int）
    m_useGold           = sig->m_useGold;            // sig+0x0c（int）
    m_field14           = sig->m_dailyInitTime;      // sig+0x10（int）
    m_enterCount        = sig->m_field14;            // sig+0x14（int）
    m_field1c           = sig->m_enterCount;         // sig+0x18（int）
    m_field24           = sig->m_field20;            // sig+0x20（int）
    m_field28           = sig->m_field24;            // sig+0x24（int）
    return true;
}

#include "W9_EventSmall/getData.inc"
{
    // loadData 的反向写出（ORIG 0x832a53a：8 条 movl + 1 条 movzbl/movb）。
    SIG_BLOOD_DUNGEON* sig = reinterpret_cast<SIG_BLOOD_DUNGEON*>(pData);
    sig->m_point             = m_point;                      // this+0x08 → sig+0x04
    sig->m_weeklyEnterCount  = m_weeklyEnterCount;           // this+0x0c → sig+0x08
    sig->m_useGold           = m_useGold;                    // this+0x10 → sig+0x0c
    sig->m_dailyInitTime     = m_field14;                    // this+0x14 → sig+0x10
    sig->m_field14           = m_enterCount;                 // this+0x18 → sig+0x14
    sig->m_enterCount        = m_field1c;                    // this+0x1c → sig+0x18
    sig->m_flag1c            = m_flag20;                     // this+0x20 → sig+0x1c（byte）
    sig->m_field20           = m_field24;                    // this+0x24 → sig+0x20
    sig->m_field24           = m_field28;                    // this+0x28 → sig+0x24
}

#include "W9_EventSmall/ResetDaily.inc"
{
    m_enterCount = 0;

    time_t curSec =
        static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    struct tm tmBuf;
    // P43：返回指针须经返回值访问（ORIG lret 槽 -0xc，tm_wday 经其读取）。
    struct tm* t = localtime_r(&curSec, &tmBuf);
    if (t->tm_wday == 1)                     // 周一：weekly 区清零
    {
        m_point = 0;
        m_weeklyEnterCount = 0;
        m_useGold = 0;
        m_flag20 = 0;
    }
    // P184 死块照抄：ORIG 尾部对 tm_hour/min/sec 的归零死写（-O0 逐条发射）。
    t->tm_hour = 0;
    t->tm_min = 0;
    t->tm_sec = 0;
}

// [b28d-lane6] AE（已修）：字段 m_point unsigned 化（P211）复现 ORIG 0x85bfee6 add 段。
#include "W9_EventSmall/IncPoint.inc"
{
    // b28d-lane6 修复（AE）：ORIG 0x85bfee6 为「mov 0x8(%eax),%edx; mov 0xc(%ebp),%eax;
    // add %eax,%edx」= 字段 unsigned + 形参 int 的 4.4 形态（P211 类型旋钮）。
    // 旧 b26「N18a 反向封顶」判词作废：探针 /tmp/l6p/p1.cpp B 形实证。
    charac_expand::CData::alter();
    m_point += delta;
}

#include "W9_EventSmall/IncUseGold.inc"
{
    // b28d-lane6 修复（AE）：同 IncPoint，ORIG 0x85bfeff 段（字段 unsigned 旋钮）。
    charac_expand::CData::alter();
    m_useGold += gold;
}

#include "W9_EventSmall/IncEnterCount.inc"
{
    charac_expand::CData::alter();
    ++m_enterCount;
    ++m_weeklyEnterCount;
}

#include "W9_EventSmall/GetEnterCount.inc"
{
    return m_enterCount;                             // 读 +0x18
}

#include "W9_EventSmall/GetWeeklyEnterCount.inc"
{
    return m_weeklyEnterCount;                       // 读 +0x0c
}
