// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_AvatarRoulette/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// Arad_AvatarRoulette — 原始工程 localjapan/Arad_AvatarRoulette.cpp 的复刻 TU。
// 收录 AvatarCoin 的「使用侧」方法（ORIG 参照 docs/class_func_reports/AvatarCoin/）：
//   Use（前批次）/ SaveToDB / SendSyncPacket / GetPackage / HistoryLog::AddLog
//   以及 CUser::_GetSaveInvenBase（SaveToDB 的内联依赖，ORIG 独立 T 符号）。
// 另含 AvatarRouletteServer::isAvatarPottery（0x817fb0a）。
// 注意：Use 调用 IsUsable 必须跨 TU（PLT 调用 → ORIG 帧 sub $0x18），
// 与 AvatarCoin.cpp 保持分 TU。

#include "AvatarCoin.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"               // m_avatarCoin 真实成员访问（_GetSaveInvenBase）
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "LogManager.h"
#include "PacketGuard.h"
#include "CDataManager.h"             // DataManagerInitDepends.h 前置依赖
#include "DataManagerInitDepends.h"   // AvatarRoulette 权威布局（成员声明）
#include <cstring>

// isAvatarPottery 为 AvatarRouletteServer 成员（ORIG T 0x0817fb0a，定义
// CtorProducers_1.cpp；本 TU 经 DataManagerInitDepends.h 类声明调用）。

// ---- AvatarRoulette::getAvatarList ORIG T 0x08911db0 ----
// 汇编形状（range 0x4d）：*isOk = 0；job<0 或 job>10 直接返回；
// 否则 out = m_avatars[job]（vector::operator=，PLT call）后 *isOk = 1。
// 地址计算 = this + job*0xc + 0x14（P30 两步寻址：*3 shl$2 后 +0x10 再 +0x4）。
#include "Arad_AvatarRoulette/getAvatarList.inc"
    bool& isOk, ENUM_CHARACTERJOB job,
    std::vector<std::vector<std::pair<unsigned long, int> > >& out)
{
    isOk = false;
    if (job >= 0 && job <= 10)
    {
        out = m_avatars[job];
        isOk = true;
    }
    return;
}

// ---- AvatarRoulette::getStikerList ORIG T 0x08911dfe ----
// 同 getAvatarList 结构；out = m_stikers[job]（+0x90 再 +0x8 → +0x98）。
#include "Arad_AvatarRoulette/getStikerList.inc"
                                   std::vector<int>& out)
{
    isOk = false;
    if (job >= 0 && job <= 10)
    {
        out = m_stikers[job];
        isOk = true;
    }
    return;
}

#include "Arad_AvatarRoulette/Use.inc"
{
    // [b28c-lane17] P0：清除历史 register asm("al") 绑定，改自然条件形。
    if (IsUsable())
    {
        m_coin = m_coin + -1;
        return true;
    }
    return false;
}

// ---- AvatarCoin 对象位于角色背包基址 +0x658（ORIG 全局约定） ----
// [b28c-lane17] 删除 coinOf static inline 助手（P49：-O0 下仍产真实 call 符号；
// ORIG 各调用点均为内联 `getCurCharacInvenW(); add $0x658` 直落 receiver）。

// ---- CUser::_GetSaveInvenBase ORIG @ 0x0864fd80 ----
// [b28d-solo-Arad_AvatarRoulette AE] 61=61 逐条对齐，p167 11/11 CLEAR。
// P211 档③字段侧同型化：ORIG 0x0864fda1 段为 LHS-first 直存
// `mov 0xc(%ebp),%edx; mov %eax,0x4(%edx)`；SIG_SAVE_INVENTORY::m_characNo 为 int 时
// 与 getCurCharacNo()(unsigned int) 异型 ⇒ 4.4 恒产 RHS-first+`mov %eax,%edx` 中转。
// 字段改 unsigned int（SigTypes_E.h:104，宽度/偏移/sizeof 0xad99 不变）后逐条全等；
// 源侧 static_cast<int> 形实测无效（P211 明证，勿再试）。
// 其余 store（+0x00/+0x09/+0x0d/+0x11/+0x15）本就与各自 callee 同型直存。
#include "Arad_AvatarRoulette/_GetSaveInvenBase.inc"
{
    out->m_accId = get_acc_id();
    out->m_characNo = getCurCharacNo();
    out->m_money = getCurCharacMoney();
    out->m_coin = getCurCharacCoin();
    out->m_payCoin = getCurCharacPayCoin();
    out->m_eventCoin = getCurCharacEventCoin();
    if (getCurCharacR() == 0)
    {
        out->m_avatarCoin = 0;
    }
    else
    {
        // ORIG 0x0864fd80：getCurCharacInvenR 结果 add $0x658 直落 Get receiver
        //（无 coinOf call；真实成员 CInventory::m_avatarCoin @+0x658）。
        out->m_avatarCoin = getCurCharacInvenR()->m_avatarCoin.Get();
    }
    // ORIG 0x0864fd80 尾：getCurSec 结果落 -0xc 槽再装载（P0 具名局部）。
    // P211 双向开关：形参 long 与实参同型（long 局部）⇒ this(LHS) 先载 =
    // ORIG `mov 0x8(%ebp),%eax; mov -0xc(%ebp),%edx` 序；unsigned 异型产 RHS 先。
    long curSec = GlobalData::s_systemTime_.getCurSec();
    setCurCharacLastPlayTick(curSec);
}

// ---- AvatarCoin::SaveToDB ORIG @ 0x081800d6 ----
#include "Arad_AvatarRoulette/SaveToDB.inc"
{
    // P0：ORIG Acquire 返回值 `mov %eax,0x4(%esp)` 直传 ctor 实参，无具名 stream 局部。
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire(
        "localjapan/Arad_AvatarRoulette.cpp", 0x184), true);
    // [b23判词·AE假阳性] 69/69 归一化逐条相等（含 guard@-0x14 / sig@-0xc 槽位、
    // 两次 operator* + 一次 operator-> + GetInBuffer<SIG_SAVE_INVENTORY> 的双 call 形、
    // `movb $0x1,0x8(%eax)` 保存标志、put(2) 与清理岛的 %ebx/%esi 交换序）；残余 diff
    // 全为未 relink .o 的 call 重定位占位 + s_big_stream_pool/s_msgq_mgr/.rodata 文件名
    // 字符串显示 0x0（§6.3 假阳性），relink 后转 IDENTICAL_AE。
    guard.operator*()->operator<<(0x23);              // SIG_SAVE_INVENTORY
    guard.operator*()->operator<<((int)user->GetUID());

    // ORIG 经 guard.operator->()（0x080c8c4e）再调 GetInBuffer，两层 call。
    SIG_SAVE_INVENTORY* sig = guard.operator->()->GetInBuffer<SIG_SAVE_INVENTORY>();
    user->_GetSaveInvenBase(sig);
    sig->m_saveFlag = 1;  // ORIG local_10[8]=1（仅 AvatarCoin 保存标志）

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

// ---- AvatarCoin::SendSyncPacket ORIG @ 0x0817ffe4 ----
// bool 返回：ORIG 调用点 0x0819f1cd `xor $0x1,%eax` 判据（Dispatchers 多处）。
#include "Arad_AvatarRoulette/SendSyncPacket.inc"
{
    if (user == 0)
        return false;
    if (user->getCurCharacR() == 0)
        return false;

    PacketGuard packet;
    packet.put_header(1, 0x24b);
    packet.put_byte(1);   // ORIG 0x0817ffe4 头部后追加 1 字节
    // ORIG 0x818005f：inven 基址 add $0x658 → Get()（无 coinOf 调用、无 coin 槽）
    packet.put_int(reinterpret_cast<AvatarCoin*>(
                       const_cast<char*>(reinterpret_cast<const char*>(
                           user->getCurCharacInvenR())) + 0x658)->Get());
    packet.finalize(true);
    user->Send(packet);
    return true;
}

// ---- AvatarCoin::HistoryLog::AddLog ORIG @ 0x817ff9c ----
#include "Arad_AvatarRoulette/AddLog.inc"
{
    if (user == 0)
        return;
    // P0：ORIG 内联 `call getCurCharacInvenRefR; add $0x658; call AvatarCoin::Get`
    // （无 coinOf 调用、无 coin 局部槽），同 GetPackage L129 内联形。
    user->LogHistory("AvatarCoin+,%d,%d", amount,
        reinterpret_cast<AvatarCoin*>(
            const_cast<char*>(reinterpret_cast<const char*>(
                user->getCurCharacInvenRefR())) + 0x658)->Get());
}

// ---- AvatarCoin::GetPackage ORIG @ 0x0818028a ----
#include "Arad_AvatarRoulette/GetPackage.inc"
{
    // [b28c-lane17] ORIG 0x0818029b：`cmpb $0x0,-0xc; je body` ⇒ 源码是
    // `if (bReset) return 1;`（真值提前返回，body 直落），非 `if(!bReset){body}`。
    if (bReset)
        return 1;
    {
        // ORIG 0x081802a4：avatarUid 先入 ebx，再 call getCurCharacInvenW
        // + add $0x658 直落 Add receiver（无 coinOf call）。
        reinterpret_cast<AvatarCoin*>(
            reinterpret_cast<char*>(user->getCurCharacInvenW()) + 0x658)
            ->Add(static_cast<unsigned int>(avatarUid));
        if (!SaveToDB(user))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_AvatarRoulette.cpp",
                __PRETTY_FUNCTION__, 0x1b0,
                "SaveAvatarCoin failed");
        }
        HistoryLog::AddLog(user, static_cast<unsigned int>(avatarUid));
        SendSyncPacket(user);
    }
    return 1;
}

// ---- AvatarCoin::BuyProcess ORIG @ 0x081801d0 ----
// mangled _ZN10AvatarCoin10BuyProcessEP5CUserR10Inven_Item
// 陶罐（avatar pottery）商品购买：isAvatarPottery(item.m_addInfo) 为假直接
// 返 0；否则取 m_addInfo2（+0x07）为币值，加到背包基址 +0x658 的 AvatarCoin，
// SaveToDB 失败打 ORIG 同文日志（__PRETTY_FUNCTION__ 文本），随后
// HistoryLog::AddLog + SendSyncPacket，返 1。
#include "Arad_AvatarRoulette/BuyProcess.inc"
{
    // [b28c-lane17] P0：清除历史 register asm("al") 绑定，改自然条件形。
    // ORIG 0x081801ee `test %al,%al; je 失败`。
    if (reinterpret_cast<AvatarRouletteServer*>(GlobalData::s_pAvatarRoulette)
            ->isAvatarPottery(static_cast<unsigned long>(item.m_addInfo)))
    {
        // ORIG 0x081801f6：item.m_addInfo2 先入 ebx 跨 getCurCharacInvenW 保活，
        // +0x658 直落 Add receiver（P226 内联实参形）。
        reinterpret_cast<AvatarCoin*>(
            reinterpret_cast<char*>(user->getCurCharacInvenW()) + 0x658)
            ->Add(static_cast<unsigned int>(item.m_addInfo2));
        if (!SaveToDB(user))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_AvatarRoulette.cpp",
                __PRETTY_FUNCTION__,
                0x19d, "SaveAvatarCoin failed");
        }
        HistoryLog::AddLog(user,
                           static_cast<unsigned int>(item.m_addInfo2));
        SendSyncPacket(user);
        return true;
    }
    return false;
}

