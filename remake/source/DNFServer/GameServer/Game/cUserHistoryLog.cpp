// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 cUserHistoryLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// 声明以 CUser.h 为准。全部方法按 ORIG 反编译（docs/class_func_reports/cUserHistoryLog/）
// 通过 CUser::LogHistory 输出 uHistory 日志；成员布局见 CUser.h：+0x00 m_user、
// +0x04 m_traderAccount[0x1e]、+0x22 m_traderCharac[0x1e]（首字节当"交易中"标志）、+0x40 m_pad。
// [b28d-solo-cUserHistoryLog 2026-09-09] 本轮 9 件 DIFF→AE：
//  ①8 件 N102（m_traderCharac[0] 数组退化 add-vs-disp）用逗号表达式旋钮
//    `*(0, m_traderCharac)` 翻案（P288 族；探针 /tmp/hlprobe/tb3.cpp h3 逐条同形）；
//  ②SkillSave 拆 num[24] + union pre 复现单条 lea / 4 条 disp movl 清零（探针 y8/y10）；
//  ③ItemDel Compound 段位域 m_lo 复现「and 后截断」形（探针 mask2 w1）。
// 剩余真缺口 6 件（均 1~2 条结构性封顶，已逐件探针穷举）：
//  N107 成员取址 4.4 恒 add+mov vs ORIG lea —— ItemAdd×2 / ItemDel×2 /
//       PrivateStoreItemAdd / PrivateStoreItemDel（tb4/tb5/tb6 共 16 形探针全 add）；
//  N108/P223 Login 两循环 continue 岛 jmp 位置差 1 条（拆 continue/goto 共享尾标签/
//       循环2 改 || continue 三形实测均不闭合）；
//  N18a 反向 ExpAdd 形参二次使用 4.4 恒重载 1 条（register 形参/局部 4 形寄存器角色全变）。
// 另有 N123（_Z41__static_initialization_and_destruction_0ii）为结构性噪音。
// 四数（本 TU）：identical=37 ae=170 near=0 diff=7 missing_in_game=20。
// [G5 2026-09-10 · L12 round2 组5 复核] 5 件：p167（ExpAdd CLEAN、ItemAdd MISSING=EXTRA=2
//   = C1/C2·D1/D2 别名）⇒ 无语义缺口。4 件残余统一为 `lea 0x15(%eax),%ebx` ↔
//   我方 `add $0x15,%eax; mov %eax,%ebx`（N107 成员取址，旧批 16 形探针全 add）；
//   ExpAdd 为形参二次使用重载（N18a 反向）⇒ 全部 N 封顶。
//   复核四数：identical=37 ae=171 near=0 diff=6 missing_in_game=20。
#include "CUser.h"
#include "SkillSlot.h"        // CSkillChanger / SkillSlot / _Mastered_skill（SkillSPInfoSave/SkillSave 依赖）
#include "DNFFunctionLib.h"   // NumberToString / DNFFLib::Binary2Hex
#include <cstring>            // strncpy
#include <cstdio>             // sprintf
#include <vector>
#include <utility>
#include "CEnvironment.h"     // G_CEnvironment / get_server_group（Logout）
#include "Secu_HackLogCheckByParty.h"  // MSG_MONSTER_DIE 完整定义（KillMonster）
#include "CInventory.h"      // GetInvenRef / get_money（Login 实际使用完整类型）
#include "STExpertJobScript.h"  // STExpertJobScript::GetLevel（Login 专家职业等级）
#include "PacketTypes.h"     // MSG_MAILBOX_SEND 完整定义（SendMail 具名访问）


#include "ItemGloballyUniqueIdentifierGenerator.h"  // itemGloballyUniqueIdentifier_t
#include "TimeLog_.h"

// CUser::m_characList @+0x796e8（std::vector<_Charac_info>；ORIG 0x08682a92/
// 0x08682ae0 operator[] 基址实证）。具名成员访问（CUser.h 已建模）。

// Inven_Item +0x25..0x32（ChronicleRandomOptionInfo::m_random 尾区，日志 14 个
// %02X 字节）审计视图（P80）：ORIG 0x08682e84/0x0868340a 逐字段 `mov 0x18(%ebp),
// %eax; movzbl 0x2X(%eax),%eax` 单 disp 折叠形——4.4 对通用指针下标恒发 add+
// movzbl 两段，仅同布局具名字段视图按 ((View*)&item)->m_bNN 每次内联访问可复现
// （P86/P95；视图指针不可命名局部，否则 re-spill）。
struct InvenByteView
{
    char m_pad0[0x25];
    unsigned char m_b25, m_b26, m_b27, m_b28, m_b29, m_b2a;
    unsigned char m_b2b, m_b2c, m_b2d, m_b2e, m_b2f, m_b30;
    unsigned char m_b31, m_b32;
};

// ---- CItemGloballyUniqueIdentifierPrint（ORIG W 0x84ebe58 族实证）----
// ctor：memset(this,0,0x15)；operator()：DNFFLib::Binary2Hex(&guid,0xa,this)，返回缓冲；
// dtor 空。0xa 字节 → 20 hex 字符 + NUL = 0x15 缓冲尺寸吻合。
class CItemGloballyUniqueIdentifierPrint
{
public:
    CItemGloballyUniqueIdentifierPrint()
    {
        memset(m_buf, 0, sizeof(m_buf));
    }
    ~CItemGloballyUniqueIdentifierPrint() {}

    const char* operator()(const itemGloballyUniqueIdentifier_t& guid)
    {
        DNFFLib::Binary2Hex(reinterpret_cast<const unsigned char*>(&guid), 0xa, m_buf);
        return m_buf;
    }

private:
    char m_buf[0x15];
};


// 构造：仅清零 m_user（ORIG 0x8695fe4，C1/C2 同址）
#include "cUserHistoryLog/cUserHistoryLog.inc"
{
    m_user = 0;
}

#include "cUserHistoryLog/SetUser.inc"
{
    m_user = user;
}

// GiveupPvP(reason)：ORIG 0x868646c —— CUser::LogHistory(m_user, "PVP,%d", reason)。
#include "cUserHistoryLog/GiveupPvP.inc"
{
    m_user->LogHistory("PVP,%d", (int)reason);
}

// ORIG 0x8686c36：m_user 非空时 LogHistory("SealItem ,%d,%d,%d", itemIdx, count, reason)。
#include "cUserHistoryLog/seal_Item.inc"
{
    if (m_user != 0)
        m_user->LogHistory("SealItem ,%d,%d,%d", itemIdx, (int)count, (int)reason);
    return;
}

// ORIG 0x08686f1e（DeleteInvaildItem.md）：直接以 m_user 调 LogHistory
// （ORIG 无空指针检查），格式 "DeleteInvaildItem, %s, %d, %d"。
#include "cUserHistoryLog/DeleteInvaildItem.inc"
{
    m_user->LogHistory("DeleteInvaildItem, %s, %d, %d", str.c_str(), a, b);
}

// SetTrader(accName, characName)：ORIG 0x8532036，strncpy 到 m_traderCharac(acc) 与 m_traderAccount(charac)
#include "cUserHistoryLog/SetTrader.inc"
{
    strncpy(m_traderCharac, accName, 0x1e);
    strncpy(m_traderAccount, characName, 0x1e);
}

// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] m_traderCharac[0] 读改逗号表达式旋钮
// `*(0, m_traderCharac)`（判词见 TradeBegin）⇒ 本函数 AE。
#include "cUserHistoryLog/AvatarItemAdd.inc"
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (reason == static_cast<eAvatarItemAddReason>(1) || reason == static_cast<eAvatarItemAddReason>(3)
        || reason == static_cast<eAvatarItemAddReason>(4)) {
        if (*(0, m_traderCharac) != 0)
            m_user->LogHistory("Avatar+,%d,%d,%s,%d,\"%s\",\"%s\"", itemIdx, avatarUid, agency,
                               static_cast<int>(reason), m_traderCharac, m_traderAccount);
    } else {
        m_user->LogHistory("Avatar+,%d,%d,\"%s\",%d", itemIdx, avatarUid, agency,
                           static_cast<int>(reason));
    }
}

// ORIG 0x8682e84：reason∈{1,2} 且交易中（m_traderCharac[0]!=0）→ 交易格式；
// **否则**（reason∉{1,2}）走常规格式（含 getItemXPos/YPos(m_user+0x79700) 网格坐标）。
// 14 个 hex 字节 = item+0x25..0x32（ChronicleRandomOptionInfo 区）。
//
// [P329 + REAL-BUG 本泳道 2026-09-09] ndiff 589 → 9（orig=371 ours=372）。两处改动，
// 改前 / 改后代码对照如下（"提前具名化 ⇒ 整块重排"的因果只有代码对照才能说明）：
//
// ① REAL-BUG：XPos 段漏了一层 `else`。
//    改前（我方）：if (reason==1||reason==2) { if (m_traderCharac[0]) { LogHistory(交易格式); }
//                                     LogHistory(常规格式); }        // ← 顺序执行，无 else
//    改后：      if (reason==1||reason==2) { if (m_traderCharac[0]) { LogHistory(交易格式); } }
//                                     else { LogHistory(常规格式); }   // ORIG 0x08683144 起
//    可观察后果：reason∉{1,2}（绝大多数常规拾取）时**一条日志都不打**；
//    reason∈{1,2} 时**多打一条**带网格坐标的 Item+ 行。p167 全 CLEAR 抓不到
//    （两分支 callee 多重集本就相同）——只有分支树对照能发现。
//
// ② P329：具名局部毁掉变参右→左求值 ⇒ 整块重排。
//    改前：unsigned int reseal = item.GetReSealCount();
//          unsigned int upgrade = item.GetUpgrade();
//          cUserHistoryLog* posSrc = &m_user->m_historyLog;
//          unsigned short xPos = posSrc->getItemXPos();
//          unsigned short yPos = posSrc->getItemYPos();
//          m_user->LogHistory(fmt, ..., xPos & 0xffff, yPos & 0xffff, ..., upgrade, ...,
//                             reseal, Print()(guid));
//    改后：全部表达式就地内联回实参位（见下方函数体）：
//          m_user->LogHistory(fmt, ..., m_user->m_historyLog.getItemXPos() & 0xffff,
//                             m_user->m_historyLog.getItemYPos() & 0xffff, ...,
//                             item.GetUpgrade(), ..., item.GetReSealCount(), Print()(guid));
//    机理：ORIG(GCC 4.1 -O0) 对变参调用**自末实参起**逐个求值入槽（GUID→reseal→off0→
//    b32..b25→fieldb→upgrade→acct→YPos→XPos），随后紧跟一段纯 mov 装载块；提前具名化
//    把这 4 个 getter 顶到语句边界，装载块与求值块整体错位 ⇒ 单块重排即数百行 diff。
//
// [2026-09-09 N102 翻案后] 残差仅 N107 ×2：ORIG 单条 `lea 0x15(%eax),%ebx` vs 4.4
// `add $0x15,%eax; mov %eax,%ebx`（成员常量偏移取址；tb4/tb5/tb6 共 16 形探针全 add）⇒ 封顶。
// [SweepHistoryLog2 2026-09-09 复核] 再试 12 形（/tmp/print_probe{,2,3,4}.cpp，c6root 4.4.7
// 与 -B cc1plus446bin 双编同结论）：数组成员退化/`&arr[0]`/`reinterpret_cast<Guid*>`/引用 cast/
// 解引用 cast/`static_cast<const void*>`/union 具名视图（m_guid）/具名左值/指针算术/三目/
// packed 非对齐偏移——**实参绑定「引用形参」时 4.4 恒发 `add $0x15,%eax; mov %eax,%ebx`**；
// 唯实参绑定「指针形参（const void*）」时 4.4 才发 lea（探针 p1/p2/p9 实证），而 ORIG
// `operator()(itemGloballyUniqueIdentifier_t const&)` 的 mangled 为 `clERK30…`（nm 0x084ebe82
// 实证，引用）⇒ 引用路径无源形可择 ⇒ N107 封顶。
#include "cUserHistoryLog/ItemAdd.inc"
                              const Inven_Item& item, eItemAddReason reason)
{
    if ((int)reason == 1 || (int)reason == 2)
    {
        if (*(0, m_traderCharac) != 0)
        {
            m_user->LogHistory(
                "Item+,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
                "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
                (int)invenType, item.m_addInfo, a3, a4, (int)reason,
                m_traderCharac, m_traderAccount,
                item.GetUpgrade(), (unsigned)item.m_fieldb,
                ((const InvenByteView*)&item)->m_b25, ((const InvenByteView*)&item)->m_b26, ((const InvenByteView*)&item)->m_b27, ((const InvenByteView*)&item)->m_b28, ((const InvenByteView*)&item)->m_b29, ((const InvenByteView*)&item)->m_b2a,
                ((const InvenByteView*)&item)->m_b2b, ((const InvenByteView*)&item)->m_b2c, ((const InvenByteView*)&item)->m_b2d, ((const InvenByteView*)&item)->m_b2e, ((const InvenByteView*)&item)->m_b2f, ((const InvenByteView*)&item)->m_b30,
                ((const InvenByteView*)&item)->m_b31, ((const InvenByteView*)&item)->m_b32,
                item.m_field0,
                item.GetReSealCount(),
                CItemGloballyUniqueIdentifierPrint()(
                    *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
        }
    }
    else
    {
        // ORIG：getItemXPos/YPos(m_user + 0x79700)（history 子对象 +0x40/+0x42 网格坐标）。
        // 全部实参就地内联 ⇒ GCC -O0 右→左求值，与 ORIG 装载序一致
        // （末实参 GUID 最先，XPos 最后）。
        m_user->LogHistory(
            "Item+,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
            (int)invenType, item.m_addInfo, a3, a4, (int)reason,
            m_user->m_historyLog.getItemXPos() & 0xffff,
            m_user->m_historyLog.getItemYPos() & 0xffff,
            item.m_addInfo2, item.m_fieldd,
            (unsigned)item.m_amp.m_abilityType, (unsigned)item.m_amp.m_abilityValue,
            item.GetUpgrade(), (unsigned)item.m_fieldb,
            ((const InvenByteView*)&item)->m_b25, ((const InvenByteView*)&item)->m_b26, ((const InvenByteView*)&item)->m_b27, ((const InvenByteView*)&item)->m_b28, ((const InvenByteView*)&item)->m_b29, ((const InvenByteView*)&item)->m_b2a,
            ((const InvenByteView*)&item)->m_b2b, ((const InvenByteView*)&item)->m_b2c, ((const InvenByteView*)&item)->m_b2d, ((const InvenByteView*)&item)->m_b2e, ((const InvenByteView*)&item)->m_b2f, ((const InvenByteView*)&item)->m_b30,
            ((const InvenByteView*)&item)->m_b31, ((const InvenByteView*)&item)->m_b32,
            item.m_field0,
            item.GetReSealCount(),
            CItemGloballyUniqueIdentifierPrint()(
                *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
    }
}

// ORIG 0x868340a：与 ItemAdd 对应的删除日志（无 XPos/YPos，11 个 %d）；
// 尾部 reason==8 且 ITEM_TYPE==1 时补 "Compound-" 日志。
// [P329 本泳道 2026-09-09] ndiff 584 → 14。分支树原本即正确（`if{if{A}}else{B}` + 独立
// Compound 块），单靠 P329 收敛：
//   改前：unsigned int reseal = item.GetReSealCount(); unsigned int upgrade = item.GetUpgrade();
//         register unsigned int lockV = item.m_amp.GetLock(); ... typeV/fieldV/sepV/addInfoV/upgradeV
//         LogHistory(fmt, ..., upgrade, ..., reseal, Print()(guid));   // Compound 同理 7 个具名
//   改后：上述 getter 全部内联回各自实参位（Compound 块 8 个实参全内联）。
// 机理同 ItemAdd：ORIG 自末实参起求值，具名化使求值块/装载块错位。
// [2026-09-09] N102 已翻案（`*(0, m_traderCharac)`）；Compound 段 `%0x1f` 掩码改位域
// m_bits.m_lo 复现 ORIG 的「and 后截断」形（探针 mask2 w1）⇒ 残差仅 N107 ×2（同 ItemAdd）。
// [SweepHistoryLog2 2026-09-09 复核] 同上 12 形复验（引用形参恒 add、指针形参才 lea）⇒ 维持封顶。
#include "cUserHistoryLog/ItemDel.inc"
                              const Inven_Item& item, eItemDelReason reason)
{
    if ((int)reason == 1 || (int)reason == 2)
    {
        if (*(0, m_traderCharac) != 0)
        {
            m_user->LogHistory(
                "Item-,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
                "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
                (int)invenType, item.m_addInfo, a3, a4, (int)reason,
                m_traderCharac, m_traderAccount,
                item.GetUpgrade(), (unsigned)item.m_fieldb,
                ((const InvenByteView*)&item)->m_b25, ((const InvenByteView*)&item)->m_b26, ((const InvenByteView*)&item)->m_b27, ((const InvenByteView*)&item)->m_b28, ((const InvenByteView*)&item)->m_b29, ((const InvenByteView*)&item)->m_b2a,
                ((const InvenByteView*)&item)->m_b2b, ((const InvenByteView*)&item)->m_b2c, ((const InvenByteView*)&item)->m_b2d, ((const InvenByteView*)&item)->m_b2e, ((const InvenByteView*)&item)->m_b2f, ((const InvenByteView*)&item)->m_b30,
                ((const InvenByteView*)&item)->m_b31, ((const InvenByteView*)&item)->m_b32,
                item.m_field0,
                item.GetReSealCount(),
                CItemGloballyUniqueIdentifierPrint()(
                    *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
        }
    }
    else
    {
        m_user->LogHistory(
            "Item-,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
            (int)invenType, item.m_addInfo, a3, a4, (int)reason,
            item.m_addInfo2, item.m_fieldd,
            (unsigned)item.m_amp.m_abilityType, (unsigned)item.m_amp.m_abilityValue,
            item.GetUpgrade(), (unsigned)item.m_fieldb,
            ((const InvenByteView*)&item)->m_b25, ((const InvenByteView*)&item)->m_b26, ((const InvenByteView*)&item)->m_b27, ((const InvenByteView*)&item)->m_b28, ((const InvenByteView*)&item)->m_b29, ((const InvenByteView*)&item)->m_b2a,
            ((const InvenByteView*)&item)->m_b2b, ((const InvenByteView*)&item)->m_b2c, ((const InvenByteView*)&item)->m_b2d, ((const InvenByteView*)&item)->m_b2e, ((const InvenByteView*)&item)->m_b2f, ((const InvenByteView*)&item)->m_b30,
            ((const InvenByteView*)&item)->m_b31, ((const InvenByteView*)&item)->m_b32,
            item.m_field0,
            item.GetReSealCount(),
            CItemGloballyUniqueIdentifierPrint()(
                *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
    }

    // Compound 补丁块（ORIG +0x543）：reason==8 && m_field1(ITEM_TYPE)==1。
    if ((int)reason == 8 && item.m_field1 == 1)
    {
        // ORIG 0x086839xx：实参位右到左即时求值（P44）——全部实参就地内联。
        m_user->LogHistory(
            "Compound-,%u,%d,%d,%d,%d,%d,%d,%d,",
            (unsigned)item.m_addInfo, item.GetUpgrade(), item.get_add_info(),
            // [b28d-solo-cUserHistoryLog 2026-09-09] ORIG 0x086839xx 读形为
            // `movzbl 0x33(%eax),%eax; and $0x1f,%eax; movzbl %al,%eax`（and 后截断）；
            // 写 `m_field & 0x1f` 时 4.4 把截断提到 and 之前并经 %edx 中转（多 1 条）。
            // 位域 m_lo 读取即 ORIG 形（探针 /tmp/hlprobe/mask2.cpp w1 逐条同形）。
            item.m_upgradeSep.m_bits.m_lo, item.m_fieldd,
            item.m_amp.getAbilityType(), item.m_amp.getAbilityValue(),
            item.m_amp.GetLock());
    }
}

// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 AvatarItemAdd ⇒ 本函数 AE。
#include "cUserHistoryLog/AvatarItemDel.inc"
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (reason == static_cast<eAvatarItemDelReason>(0) || reason == static_cast<eAvatarItemDelReason>(1)) {
        if (*(0, m_traderCharac) != 0)
            m_user->LogHistory("Avatar-,%d,%d,%s,%d,\"%s\",\"%s\"", itemIdx, avatarUid, agency,
                               static_cast<int>(reason), m_traderCharac, m_traderAccount);
    } else {
        m_user->LogHistory("Avatar-,%d,%d,%s,%d", itemIdx, avatarUid, agency, static_cast<int>(reason));
    }
}

// b28d-65 [N4/N102]：同 CreatureItemAdd；ORIG 0x08684f38。
// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
#include "cUserHistoryLog/CreatureItemDel.inc"
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (type == 5) {
        if (reason == static_cast<eItemDelReason>(0x14) || reason == static_cast<eItemDelReason>(0x15)) {
            if (*(0, m_traderCharac) != 0)
                m_user->LogHistory("CTItem-,%d,%d,%d,%d,\"%s\",\"%s\"", itemIdx, count, addInfo,
                                   static_cast<int>(reason), m_traderCharac, m_traderAccount);
        } else {
            m_user->LogHistory("CTItem-,%d,%d,%d,%d", itemIdx, count, addInfo, static_cast<int>(reason));
        }
    } else {
        Inven_Item item;
        item.reset();
        item.m_addInfo = itemIdx;   // ORIG local_47 = param_3（m_addInfo +0x02）
        ItemDel(invenType, count, addInfo, item, reason);
    }
}

// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
#include "cUserHistoryLog/MoneyAddTrade.inc"
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (*(0, m_traderCharac) != 0)
        m_user->LogHistory("Money+,%d,%d,%d,%s(%s)", money, add, static_cast<int>(reason),
                           m_traderCharac, m_traderAccount);
}

// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
#include "cUserHistoryLog/MoneySubTrade.inc"
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (*(0, m_traderCharac) != 0)
        m_user->LogHistory("Money-,%d,%d,%d,%s(%s)", money, sub, static_cast<int>(reason),
                           m_traderCharac, m_traderAccount);
}

#include "cUserHistoryLog/MoneyAdd.inc"
{
    if (reason == static_cast<eMoneyAddReason>(1) || reason == static_cast<eMoneyAddReason>(2)) {
        MoneyAddTrade(money, add, reason);
    } else {
        m_user->LogHistory("Money+,%d,%d,%d", money, add, static_cast<int>(reason));
    }
}

#include "cUserHistoryLog/MoneySub.inc"
{
    if (reason == static_cast<eMoneySubReason>(2) || reason == static_cast<eMoneySubReason>(3)) {
        MoneySubTrade(money, sub, reason);
    } else {
        m_user->LogHistory("Money-,%d,%d,%d", money, sub, static_cast<int>(reason));
    }
}

// ORIG T 0x8682cc8（0x32 字节）：LogHistory("WP-,%d,%d,%d", cur, sub, reason)。
#include "cUserHistoryLog/WPSub.inc"
{
    m_user->LogHistory("WP-,%d,%d,%d", cur, sub, static_cast<int>(reason));
}

// ORIG T 0x8685bb8（0x40 字节）：GuildAddExp,"%d/%d/%d",%d,"%s",%d,%d
// （年/月/日/guildKey/guildDBInfo/characNo/exp）。
#include "cUserHistoryLog/GuildAddExp.inc"
                                  const char* guildDBInfo,
                                  unsigned int characNo, int exp)
{
    const time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    struct tm* r = localtime_r(&now, &local);
    // b28d-50：ORIG 0x08685be2 段把 tm_year/tm_mon+1/tm_mday 各落具名槽
    // （-0x14/-0x10/-0xc，P0 槽往返⇒原码有具名局部），调用点再 `lea 0x76c(%eax)`；
    // 直接内联读 r-> 字段会丢槽并改帧（0x78→0x70 + push esi/ebx）。
    int year = r->tm_year;
    int month = r->tm_mon + 1;
    int day = r->tm_mday;
    m_user->LogHistory(
        "GuildAddExp,\"%d/%d/%d\",%d,\"%s\",%d,%d",
        year + 1900, month, day,
        guildKey, guildDBInfo, characNo, exp);
}

// ORIG T 0x086853e6（0x32 字节）：LogHistory("CleanPad,%d,%d", a, b)。
#include "cUserHistoryLog/RequestCleanPad.inc"
{
    m_user->LogHistory("CleanPad,%d,%d", a, static_cast<int>(b));
}

#include "cUserHistoryLog/CoinAdd.inc"
{
    m_user->LogHistory("Coin+,%d,%d,%d", coin, amount, static_cast<int>(reason));
}

#include "cUserHistoryLog/PayCoinAdd.inc"
{
    m_user->LogHistory("PCoin+,%d,%d,%d", coin, amount, static_cast<int>(reason));
}

#include "cUserHistoryLog/EventCoinAdd.inc"
{
    m_user->LogHistory("ECoin+,%d,%d,%d", coin, amount, static_cast<int>(reason));
}

// LevelUp(int, eLevelUpReason)：ORIG 0x86845b2，"Level+,%d,%d"
#include "cUserHistoryLog/LevelUp.inc"
{
    m_user->LogHistory("Level+,%d,%d", level, static_cast<int>(reason));
}

// LevelDown(int, eLevelDownReason)：ORIG 0x86845de，"Level-,%d,%d"
#include "cUserHistoryLog/LevelDown.inc"
{
    m_user->LogHistory("Level-,%d,%d", level, static_cast<int>(reason));
}

#include "cUserHistoryLog/CoinSub.inc"
{
    m_user->LogHistory("Coin-,%d,%d,%d", coin, sub, static_cast<int>(reason));
}

#include "cUserHistoryLog/EventCoinSub.inc"
{
    m_user->LogHistory("ECoin-,%d,%d,%d", coin, sub, static_cast<int>(reason));
}

#include "cUserHistoryLog/PayCoinSub.inc"
{
    m_user->LogHistory("PCoin-,%d,%d,%d", coin, sub, static_cast<int>(reason));
}

#include "cUserHistoryLog/pvpMissionClearReward.inc"
{
    m_user->LogHistory("PvPMissionReward+,%d,%d,%d,%d", expPoint, exp, missionKind, missionIndex);
}

#include "cUserHistoryLog/EnterDungeon.inc"
{
    m_user->LogHistory("DungeonEnter,\"%s\",%d", dungeonName, level);
}

#include "cUserHistoryLog/LeaveDungeon.inc"
{
    m_user->LogHistory("DungeonLeave,\"%s\",%d", dungeonName, level);
}

#include "cUserHistoryLog/LeaveDungeon_1.inc"
{
    m_user->LogHistory("DungeonLeave,\"%s\",%d,%d,%s", dungeonName, unk, state, memberNames);
}

#include "cUserHistoryLog/LeaveDungeon_2.inc"
{
    m_user->LogHistory("DungeonLeave,%d,%d,%d,%s", dungeonIdx, unk, state, memberNames);
}

// b28d-65 [N4/N102]：残余仅 m_traderCharac[0] add-vs-disp 1 条（同 TradeBegin 判词）；
//   ORIG 0x08684e3c，78=77+1 封顶。
// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
#include "cUserHistoryLog/CreatureItemAdd.inc"
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (type == 5) {
        if (reason == static_cast<eItemAddReason>(0x1a) || reason == static_cast<eItemAddReason>(0x1c)
            || reason == static_cast<eItemAddReason>(0x1d)) {
            if (*(0, m_traderCharac) != 0)
                m_user->LogHistory("CTItem+,%d,%d,%d,%d,\"%s\",\"%s\")", itemIdx, count, addInfo,
                                   static_cast<int>(reason), m_traderCharac, m_traderAccount);
        } else {
            m_user->LogHistory("CTItem+,%d,%d,%d,%d", itemIdx, count, addInfo, static_cast<int>(reason));
        }
    } else {
        Inven_Item item;
        item.reset();
        item.m_addInfo = itemIdx;   // ORIG local_47 = param_3（m_addInfo +0x02）
        ItemAdd(invenType, count, addInfo, item, reason);
    }
}

#include "cUserHistoryLog/InitSkill.inc"
{
    m_user->LogHistory("SkillInit,%d,%d,%d,%d,%d", treeKind, level, sp, sfp, static_cast<int>(reason));
}

// ============================================================================
// cUserHistoryLog::SkillSPInfoSave @ 0x08686188
//   SkillSPInfoSave(CUser*, eSkillSPInfoSaveReason)
//   经 CSkillChanger::GetCurCharacTotalSP 计算总 SP；按当前技能树索引（-1 单树 /
//   其它双树）记录 "SkillSPInfo,%d,%d,%d,%d"（reason, remainSP, totalSP, treeKind）。
// ============================================================================
#include "cUserHistoryLog/SkillSPInfoSave.inc"
{
    // ORIG 语句顺序：changer 构造 → totalSP → buf 清零 → isSingleTree 判定。
    WongWork::CSkillChanger changer;
    int totalSP = changer.GetCurCharacTotalSP(user, false);

    // ORIG 0x08686188：buf1/buf2 各 0x80（rep stos $0x20 dwords，P116 计数口径）；
    // 单/双树判定不物化 bool（ORIG test/je+cmp/je 直落两分支体，P133 独立守卫形）。
    char buf1[0x80] = {0};
    char buf2[0x80] = {0};

    if (user != 0 && user->GetCurCharacSkillTreeIndex() == -1)
    {
        // ORIG 0x08686210：链式 receiver（getR 结果 eax 直转 this 实参，不落
        // slot——P52；命名 slot 局部会 spill+reload 各多 1 条）。
        int remain0 = reinterpret_cast<const SkillSlot*>(user->getCurCharacSkillR())
                          ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0);
        if (totalSP >= 0 && remain0 >= 0)
        {
            sprintf(buf1, "SkillSPInfo,%d,%d,%d,%d", (int)reason, remain0, totalSP, 0xffffffff);
            user->LogHistory(buf1);
        }
    }
    else
    {
        int remain0 = reinterpret_cast<const SkillSlot*>(user->getCurCharacSkillR())
                          ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0);
        int remain1 = reinterpret_cast<const SkillSlot*>(user->getCurCharacSkillR())
                          ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1);
        if (totalSP >= 0 && remain0 >= 0 && remain1 == 0)
        {
            sprintf(buf1, "SkillSPInfo,%d,%d,%d,%d", (int)reason, remain0, totalSP, 0);
            user->LogHistory(buf1);
            sprintf(buf2, "SkillSPInfo,%d,%d,%d,%d", (int)reason, remain1, totalSP, 1);
            user->LogHistory(buf2);
        }
    }
}

// ============================================================================
// cUserHistoryLog::BillingSuccess @ 0x086858a0
//   BillingSuccess(ENUM_BILLING_TYPE, unsigned long long, int)
//   type==3 走 "BILLING, Success,%d,%s"；否则 "BILLING, Success,%d,%d,%d"
//  （ORIG 第二分支对 NumberToString 结果用 %d 打印——忠实转写）。
// [B24D判词 DONE_AE] 语义正确：两分支各独立 NumberToString(tranNo,0)（P48）+LogHistory
//   "BILLING, Success,%d,%s"（type==3）/ "%d,%d,%d"（type,str,amount）。已修第一分支
//   字面量 3→(int)type（ORIG 读参数槽 0xc(%ebp)），45/45 指令对齐；仅剩 call/字符串
//   地址 AE 占位差异。
// ============================================================================
#include "cUserHistoryLog/BillingSuccess.inc"
                                     unsigned long long tranNo, int amount)
{
    // ORIG 0x086858a0：两分支各独立调 NumberToString（P48 去跨臂缓存，
    // `push %ebx`（第二臂 %ebx=amount）正由此双调用引入）。
    if (type == 3)
        m_user->LogHistory("BILLING, Success,%d,%s", (int)type,
                           NumberToString(tranNo, 0));
    else
        m_user->LogHistory("BILLING, Success,%d,%d,%d", (int)type,
                           NumberToString(tranNo, 0), amount);
}

// ============================================================================
// cUserHistoryLog::BillingFail @ 0x08685838
//   BillingFail(ENUM_BILLING_TYPE, unsigned long long, int, int, char*)
//   "BILLING, Fail,%d,%s,%d,%d,%s"（type, tranNo 串, amount, code, reason）。
// ============================================================================
#include "cUserHistoryLog/BillingFail.inc"
                                  unsigned long long tranNo, int amount,
                                  int code, char* reason)
{
    // b28d-50：ORIG 0x08685838 的 NumberToString 结果直留 %eax 落 0xc(%esp)（无
    // str 槽）；具名局部多一槽且使 ull spill 由 -0x10/-0xc 漂到 -0x20/-0x1c、
    // 帧 0x34→0x48、丢 push ebx。内联实参即复现（P0 表达式直传）。
    m_user->LogHistory("BILLING, Fail,%d,%s,%d,%d,%s",
                       (int)type, NumberToString(tranNo, 0), amount, code, reason);
}

// ============================================================================
// cUserHistoryLog::SkillSave @ 0x08685dce
//   SkillSave(_Mastered_skill const*, unsigned int, int, eSkillSaveReason)
//   收集非空技能（跳过 0x8a..0x95 保留区），按每 15 个一批写入
//   "SaveLethe,%d, %d/%d/%d<,或/>%d/%d…" 的 Lethe 保存日志。
// ============================================================================
// [b28d-solo-cUserHistoryLog] DIFF 257/260：修真 bug——buf 清零 0x100→整块 0x400 字节
// （ORIG rep stos ecx=0x100 dwords，P116/P253）、chunk 0x80→0x200；make_pair 去 (int) cast
// 以匹配 ORIG 实例 make_pair<uchar const&,char const&>（0x0869c165）；lineCount/end 预清（P198①）。
// 残差 N78（tmp memset 恒 call）+ N107（&tmp[0x10] lea+add）+ 字符串地址 AE 伪差。
#include "cUserHistoryLog/SkillSave.inc"
                                eSkillSaveReason reason)
{
    // ORIG 0x08685e02 段首 `movl $0xf,-0x44(%ebp)`（具名 15 局部，P0 槽往返）。
    int perLine = 15;
    char header[16];
    sprintf(header, ",%d, ", (int)reason);

    // ORIG 0x08685dfb 段：lineCount(-0x40) 预清先于 vec 构造（P198① 预清两句式）。
    int lineCount = 0;
    std::vector<std::pair<int, int> > vec;
    int count = 0;
    for (int i = 0; i < 0xcc; i++)
    {
        if (skills[i].m_index != 0 && (i < 0x8a || 0x95 < i))
        {
            // ORIG 0x0869c165 实例 = make_pair<unsigned char const&, char const&>
            // （C++0x 完美转发重载，实参为无 cast 的左值）；(int) cast 会选错实例。
            vec.push_back(std::make_pair(skills[i].m_index, skills[i].m_level));
            count++;
        }
    }

    lineCount = (int)((float)count / 15.0f) + 1;
    for (int line = 0; line < lineCount; line++)
    {
        // ORIG 0x08685ef2 段：rep stos ecx=0x100 dwords = 0x400 字节（整块 buf 清零，P116/P253）。
        // 旧 memset(buf,0,0x100) 只清 0x100 字节（少清 0x300）——真语义缺口。
        char buf[1024] = {0};
        memcpy(buf, "SaveLethe", 10);
        strcat(buf, header);

        int start = line * 0xf;
        // ORIG 0x08685f48 段：`movl $0x0,-0x2c`（end 预清 0，P198① 两句式）。
        int end = 0;
        if (line < lineCount - 1)
        {
            end = start + 0xe;
        }
        else
        {
            end = (count % 0xf) - 1 + start;
        }
        int n = end - start + 1;
        if (n == 0)
            break;

        // [b28d-solo-cUserHistoryLog 2026-09-09 翻案] 原 `char tmp[40]` 两处残差：
        // ①`&tmp[0x10]` 4.4 恒发 lea -0x94 + add $0x10（ORIG 单条 lea -0x84）；
        // ②循环内 memset 恒发 call（ORIG 内联 4 条 movl disp）。
        // 拆成「先声明 char num[24]（浅槽 -0x84，ORIG tmp+0x10 区）+ 后声明 16 字节
        // 结构体 pre（深槽 -0x94，4.4 后声明占深槽且与 num 紧邻——探针
        // /tmp/hlprobe/y8.cpp y8 逐条同形）」：num 数组名退化 = 单条 lea -0x84；
        // pre.m_zero[i]=0 = 4 条 movl disp；sprintf((char*)&pre,…) = 单条 lea -0x94。
        // union 双视角：m_chars 供 sprintf/strcat 数组名退化（单条 lea，探针 y10），
        // m_zero 供 4 条 disp movl 清零（探针 y8）。
        union SkillSavePre { char m_chars[16]; unsigned int m_zero[4]; };
        char num[24];          // ORIG tmp+0x10（-0x84）区
        SkillSavePre pre;      // ORIG tmp+0x00（-0x94）区
        // ORIG 循环外无 tmp 清零（sprintf 自写终止符）；清零只在 k 循环内（4.1 内联 4 条 movl）。
        sprintf(num, "%d/%d/%d", a, n, b);
        strcat(buf, num);

        // ORIG 0x08685ff3 段：rep stos ecx=0x80 dwords = 0x200 字节（整块 chunk 清零）。
        char chunk[512] = {0};
        for (int k = 0; k < n; k++)
        {
            int idx = vec[start + k].first;
            int lvl = vec[start + k].second;
            // ORIG 0x08686009 段：4 条 `movl $0,-0x94/-0x90/-0x8c/-0x88(%ebp)`（disp 形）——
            // 结构体成员赋值复现（探针 y8；memset 恒 call，指针 cast 写恒 lea+add）。
            pre.m_zero[0] = 0;
            pre.m_zero[1] = 0;
            pre.m_zero[2] = 0;
            pre.m_zero[3] = 0;
            if (k % 3 == 0)
                sprintf(pre.m_chars, ",%d/%d", idx, lvl);
            else
                sprintf(pre.m_chars, "/%d/%d", idx, lvl);
            strcat(chunk, pre.m_chars);
        }
        strcat(buf, chunk);

        m_user->LogHistory(buf);
    }
}

#include "cUserHistoryLog/SkillAdd.inc"
{
    m_user->LogHistory("Skill+,%d,%d,%d,%d,%d", job, skillIdx, count, static_cast<int>(reason), treeKind);
}

#include "cUserHistoryLog/SkillDel.inc"
{
    m_user->LogHistory("Skill-,%d,%d,%d,%d,%d", job, skillIdx, count, static_cast<int>(reason), treeKind);
}

#include "cUserHistoryLog/SPAdd.inc"
{
    m_user->LogHistory("SP+,%d,%d,%d,%d", remainSP, sp, static_cast<int>(reason), treeKind);
}

#include "cUserHistoryLog/SPSub.inc"
{
    m_user->LogHistory("SP-,%d,%d,%d,%d", remainSP, sp, static_cast<int>(reason), treeKind);
}

#include "cUserHistoryLog/SFPAdd.inc"
{
    m_user->LogHistory("SFP+,%d,%d,%d,%d", remainSFP, sfp, static_cast<int>(reason), treeKind);
}

#include "cUserHistoryLog/SFPSub.inc"
{
    m_user->LogHistory("SFP-,%d,%d,%d,%d", remainSFP, sfp, static_cast<int>(reason), treeKind);
}

#include "cUserHistoryLog/FPAdd.inc"
{
    m_user->LogHistory("FP+,%d,%d,%d,%d", a, b, c, static_cast<int>(reason));
}

#include "cUserHistoryLog/FPSub.inc"
{
    m_user->LogHistory("FP-,%d,%d,%d,%d", a, b, c, static_cast<int>(reason));
}

// WPAdd(int, int, eWPAddReason)：ORIG _ZN15cUserHistoryLog5WPAddEii12eWPAddReason @0x08682c96。
// 记录 "WP+,%d,%d,%d"（winPoint 当前值, 增加量, reason）。
#include "cUserHistoryLog/WPAdd.inc"
{
    m_user->LogHistory("WP+,%d,%d,%d", winPoint, add, static_cast<int>(reason));
}

#include "cUserHistoryLog/ChangeGrowType.inc"
{
    m_user->LogHistory("CP,%d,%d,%d,%d,%d,%d", a, b, c, d, e, static_cast<int>(reason));
}

#include "cUserHistoryLog/ClearUsedQP.inc"
{
    m_user->LogHistory("ClearQP,%d,%d,%d,%d", before, after, initCount, unused);
}

// QPAdd(int, int, int)：ORIG _ZN15cUserHistoryLog5QPAddEiii @0x0868888c。
// 记录 "QP+,%d,%d,%d"（before, questIdx, add）。
#include "cUserHistoryLog/QPAdd.inc"
{
    m_user->LogHistory("QP+,%d,%d,%d", before, questIdx, add);
}

// SendMail：ORIG 0x86847ea。字段已具名（PacketTypes.h MSG_MAILBOX_SEND，
// m_pad 即 MSG_BASE 后的 +0x0d 起原字节区）。条件为 mail 非空 且 收件人名非空。
#include "cUserHistoryLog/SendMail.inc"
{
    // b28d-65：ORIG 0x086847ea 两条独立早退（je 分落两个 jmp/nop 中继，P63）：
    // 第二守卫 `add $0x11; test %eax,%eax` 是数组退化比较（恒真、无 movzbl）
    // ⇒ 原码写 `mail->m_name11 == 0`（P171 照抄原码恒真形）；nstr/f2e/f36/f37
    // 无槽往返 ⇒ 内联实参（P0），mailBody/f3d/f39 有槽 ⇒ 具名局部。
    if (mail == 0)
        return;
    if (mail->m_name11 == 0)
        return;
    m_user->LogHistory("MailS,%s,%d,%d,%d,%d,%d,\"%s\",%s",
                       mail->m_name11, mail->m_gold2e, mail->m_space36,
                       mail->m_slot37, mail->m_itemId39, mail->m_extra3d,
                       mail->m_text45, NumberToString(a, 0));
}

#include "cUserHistoryLog/MoveArea.inc"
{
    m_user->LogHistory("MoveArea,%d,%d,%d,%d", a, b, c, d);
}

// Logout：ORIG 0x86840c4。"IP-,%s,%d,%d,%d,%d,%d,%p"，
// 参数顺序：characName, a3, a4, (int)a6(uchar), server_group(CEnvironment), a5, this指针。
#include "cUserHistoryLog/Logout.inc"
                             unsigned char f)
{
    // ORIG 0x08685bxx：get_server_group 结果留 callee-saved（%ebx）直转实参；
    // 4.4 命名局部必 spill 槽（frame +0x14）。取链式内联形。
    m_user->LogHistory("IP-,%s,%d,%d,%d,%d,%d,%p",
                       b, c, d, static_cast<int>(f),
                       G_CEnvironment()->get_server_group(), e,
                       (const void*)m_user);
}

// RecvMail：ORIG 0x86848a8。"MailR,%d,\"%s\",\"%s\""
#include "cUserHistoryLog/RecvMail.inc"
                               const char* accName)
{
    m_user->LogHistory("MailR,%d,\"%s\",\"%s\"", mailIdx, senderName, accName);
}

// ChangeItemLimitCount：ORIG 0x8686d54。m_user 非空才记录，
// "ChangeItemTradeLimitCount,%d,%d,%d,%d,%d"。
#include "cUserHistoryLog/ChangeItemLimitCount.inc"
{
    if (m_user != 0)
        m_user->LogHistory("ChangeItemTradeLimitCount,%d,%d,%d,%d,%d", a, b, itemIdx, d, e);
    return;
}

// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] m_traderCharac[0] 数组退化 add-vs-disp
// 单条已复现 ⇒ 本函数 AE。本文件交易中判族共同判词见下。
#include "cUserHistoryLog/TradeBegin.inc"
{
    // [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] ORIG（0x08685047 TradeBegin /
    // 0x08682xxx MoneyAdd·MoneySub·Avatar±·TradeEnd·Creature± 同族）对数组退化读发
    // `mov 0x8(%ebp),%eax; add $0x22,%eax; movzbl (%eax),%eax`（4.1 两步寻址）；
    // 4.4 对直接下标 `m_traderCharac[0]` 恒折单条 disp。把退化指针经逗号表达式变成
    // 右值再解引用（`*(0, m_traderCharac)`，语义同 `m_traderCharac[0]`）即复现两步
    // 寻址——探针 /tmp/hlprobe/tb3.cpp h3 与 ORIG 逐条同形（sub $0x18 / add $0x22 /
    // movzbl (%eax)），无槽、无新增符号。P288 数组退化族旋钮。
    if (*(0, m_traderCharac) != 0)
        m_user->LogHistory("Trade+,\"%s\",\"%s\"", m_traderCharac, m_traderAccount);
}

// b28d-65 [N4/N102]：残余仅 m_traderCharac[0] 数组退化 add-vs-disp 1 条（同 TradeBegin
//   判词，七形探针封顶）；ORIG 0x0868507c，53=53。
// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
#include "cUserHistoryLog/TradeEnd.inc"
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (*(0, m_traderCharac) != 0) {
        static const int ILLEGAL_FACTOR = 10;   // ORIG _ZZN15cUserHistoryLog8TradeEndE15eTradeEndReasoniiiiE14ILLEGAL_FACTOR @0x8cf2690（=10）
        int sum1 = a + b;
        int sum2 = c + d;
        char flag = 0;
        if (sum1 * ILLEGAL_FACTOR < sum2)
            flag = 1;
        else if (sum2 * ILLEGAL_FACTOR < sum1)
            flag = 2;
        m_user->LogHistory("Trade-,\"%s\",\"%s\",%d, %d, %d, %d",
                           m_traderCharac, m_traderAccount, static_cast<int>(reason), sum1, sum2,
                           static_cast<int>(flag));
    }
}

#include "cUserHistoryLog/TradeItemAddFail.inc"
{
    m_user->LogHistory("Item!,%d,%d", a, b);
}

#include "cUserHistoryLog/AchievementComplete.inc"
{
    m_user->LogHistory("AchieveComplete,%d", idx);
}

#include "cUserHistoryLog/pvpMissionAdd.inc"
{
    m_user->LogHistory("PvPMission+,%d,%d,%d,%d", a, b, c, d);
}

#include "cUserHistoryLog/pvpMissionDel.inc"
{
    m_user->LogHistory("PvPMission-,%d,%d", a, b);
}

#include "cUserHistoryLog/RedeemItemAdd.inc"
{
    m_user->LogHistory("RedeemItem+,%d,%d", a, b);
}

#include "cUserHistoryLog/ItemDelCargo.inc"
{
    m_user->LogHistory("Inven-,%d,%d,%d,%d", a, b, c, static_cast<int>(reason));
}
// DungeonClearInfo(int, long)：ORIG 0x8684ac4，经 CUser::LogHistory 记录
#include "cUserHistoryLog/DungeonClearInfo.inc"
{
    m_user->LogHistory("DungeonClearInfo,%d,%d", isLast, playTimeSec);
}

// DungeonClearInfo(int,long,int,int,int,ENUM_DUNGEON_MODE,char const*,int)：ORIG 0x8684a6e
#include "cUserHistoryLog/DungeonClearInfo_1.inc"
                                       int val2, ENUM_DUNGEON_MODE dungeonMode,
                                       const char* name, int val3)
{
    m_user->LogHistory("AdvanceDungeonClearInfo,%d,%d,%d,%d,%d,%d,%s,%d",
                       idx, mode2, isLast, playTimeSec, val2, static_cast<int>(dungeonMode),
                       name, val3);
}

// apply_Item(int,int,eApplyItemReason)：ORIG 0x8686c7c。
// m_user 非空才记录，"ApplyItem ,%d,%d,%d"（注意逗号后有空格，与 ORIG 字符串一致）。
#include "cUserHistoryLog/apply_Item.inc"
{
    if (m_user)
        m_user->LogHistory("ApplyItem ,%d,%d,%d", a, b, static_cast<int>(reason));
    return;
}

// UseSelectBooster(unsigned int,int,int,std::vector<Inven_Item>&)：ORIG 0x8686b52。
// 把 items 中每个元素 +0x02（Inven_Item::m_addInfo）拼成 "%d," 串（上限 0x400 字节缓冲，
// 每项至少占 0xd 字节），经 CUser::LogHistory 记录 "UseSelectBooster,%u,%d,%d,%s"。
// b28d-65：while(i<size)+体内 `if(strlen+0xd>0x3fe) break` + sprintf 目标内联（去 p 槽）
//   ⇒ 与 ORIG 0x08686b52 逐条全等（60=60）。
#include "cUserHistoryLog/UseSelectBooster.inc"
                                       std::vector<Inven_Item>& items)
{
    char buffer[0x400];
    memset(buffer, 0, sizeof(buffer));
    unsigned int i = 0;
    while (i < items.size())
    {
        if (strlen(buffer) + 0xd > 0x3fe)
            break;
        sprintf(buffer + strlen(buffer), "%d,", items[i].m_addInfo);
        ++i;
    }
    m_user->LogHistory("UseSelectBooster,%u,%d,%d,%s", a, b, c, buffer);
}

// ============================================================================
// cUserHistoryLog::UpgradeItem @ 0x8684ca0
//   记录强化日志 "Upgrade+,%d,%d,%d" / "Upgrade-,%d,%d,%d"。
// ============================================================================
#include "cUserHistoryLog/UpgradeItem.inc"
{
    if (isUpgrade)
        m_user->LogHistory("Upgrade+,%d,%d,%d", itemIdx, from, to);
    else
        m_user->LogHistory("Upgrade-,%d,%d,%d", itemIdx, from, to);
}

// ============================================================================
// cUserHistoryLog::SeparateUpgradeItem @ 0x8684d0a
//   记录分离强化日志 "SeparateUpgrade+,%d,%d,%d" / "-..."。
// ============================================================================
#include "cUserHistoryLog/SeparateUpgradeItem.inc"
                                          bool isUpgrade)
{
    if (isUpgrade)
        m_user->LogHistory("SeparateUpgrade+,%d,%d,%d", itemIdx, from, to);
    else
        m_user->LogHistory("SeparateUpgrade-,%d,%d,%d", itemIdx, from, to);
}

// ============================================================================
// cUserHistoryLog::ExpandInventory @ 0x08683cbc
//   记录背包扩容日志 "ExpandInventory+,%d,%d,%d"（cap, newCap, a）。
//   经 CUser::LogHistory 输出（与其余日志方法同构）。
// ============================================================================
#include "cUserHistoryLog/ExpandInventory.inc"
{
    m_user->LogHistory("ExpandInventory+,%d,%d,%d", cap, newCap, a);
}

// ============================================================================
// cUserHistoryLog::MapClear @ 0x08684b22（_ZN15cUserHistoryLog8MapClearEtii）
//   记录副本通关物品日志 "MapClear,%d,%d,%d"。
//   调用方：WongWork::CBossStage::handleMoveMap（逐有效成员）。
// ============================================================================
#include "cUserHistoryLog/MapClear.inc"
{
    m_user->LogHistory("MapClear,%d,%d,%d", (unsigned)itemIdx, playSec, a);
}

// ============================================================================
// cUserHistoryLog::KillMonster @ 0x086851ac
//   (_ZN15cUserHistoryLog11KillMonsterEijjttRK15MSG_MONSTER_DIEcbii13eExpAddReason)
//   记录击杀日志 "KillMob,%d,%d,%d, (%d:%d),%d, (%d:%d),%d,%d,%d, (%d:%d),%d,
//   %d,%d,%d,%d,%d,%d,%d"（20 变参，含 MSG_MONSTER_DIE +0xa22..+0xa3d 字段）。
//   调用方：WongWork::CBossStage::handleKillMonster。
// ============================================================================
#include "cUserHistoryLog/KillMonster.inc"
                                  unsigned int killValue2, unsigned short a,
                                  unsigned short b, const MSG_MONSTER_DIE& msg,
                                  char roleType, bool isBoss, int curExp,
                                  int exp, eExpAddReason reason)
{
    m_user->LogHistory(
        "KillMob,%d,%d,%d, (%d:%d),%d, (%d:%d),%d,%d,%d, (%d:%d),%d,%d,%d,%d,%d,%d,%d",
        mobIndex, killValue1, killValue2,
        (unsigned)a & 0xffff, (unsigned)b & 0xffff,
        (unsigned)msg.m_fieldA22,
        (int)msg.m_fieldA29, (int)msg.m_fieldA2b,
        msg.m_fieldA31, msg.m_fieldA2d,
        (int)roleType,
        (int)msg.m_fieldA35, (int)msg.m_fieldA37,
        (int)msg.m_fieldA39, (int)msg.m_fieldA3b, (int)msg.m_fieldA3d,
        (unsigned)(isBoss ? 1 : 0),
        curExp, exp, (int)reason);
}


// ==================== 私有商店历史（FixPrivateStoreSellerVtable 批次）====================
// 报告 docs/class_func_reports/cUserHistoryLog/{CreatePrivateStore,RemovePrivateStore,
// PrivateStoreItemAdd,PrivateStoreItemDel}.md 全文已读（汇编+反编译 C）。

// ORIG T 0x868455a（报告全文）："Private Store+,%d,%d"
#include "cUserHistoryLog/CreatePrivateStore.inc"
{
    // ORIG vararg 只压 zoneA+zoneB 两个实参（0xc/0x10(%esp)），format "%d,%d"，
    // 不传 name；多余 name 实参是 ORIG 未有的（移除以匹配 0x18 帧）。
    m_user->LogHistory("Private Store+,%d,%d", zoneA, zoneB);
}

// ORIG T 0x8684586（报告全文）："Private Store-,%d,%d"
#include "cUserHistoryLog/RemovePrivateStore.inc"
{
    m_user->LogHistory("Private Store-,%d,%d", zoneA, zoneB);
}

// ORIG T 0x868433e（报告全文）："Item+,..."
// GUID 串：ORIG CItemGloballyUniqueIdentifierPrint 对 item+0x15 的渲染
// （W 0x84ebe58/0x84ebe7c，本文件真实建模）。
// b21 NEAR：语义与 ORIG 全等。修：vararg 原缺失 item+0x00 字节（IVar1=*param_3），
//   加在 b32 与 GetReSealCount 之间使 %02X×14+%d+%d 与 26 实参对齐（原 guid 入%02X 移位已消除）。
// b28c-L2 fresh 复验 ndiff 13（orig 165/our 166），残差全部为两族结构性封顶：
//   [N80] ORIG 0x0868434x 在 `call CItemGloballyUniqueIdentifierPrintC1Ev` **之前**
//     就把 op() 实参 `&item+0x15` 物化到 callee-saved %ebx 并跨 ctor 保活
//     （`mov 0x10(%ebp),%eax; lea 0x15(%eax),%ebx; lea -0x2d(%ebp),%eax; mov %eax,(%esp);
//      call C1; mov %ebx,0x4(%esp)`）；4.4 恒 ctor 后求值。本件 print 为**具名对象**
//     （ORIG D1 在 LogHistory 之后、leave 之前的正常路径 0x08684xxx，非语句末临时），
//     故 P227 全表达式临时链翻案不可用（改临时会把 D1 提前、破坏 ORIG 尾段与
//     landing pad 0x0868452x 的 `lea -0x2d; call D1; _Unwind_Resume` 形）⇒ 维持 N80。
//   [N82附] 帧内槽向：ORIG print 聚合对象 @-0x2d、guid 标量 @-0x80（插在 vararg
//     值槽 -0x7c..-0x30 之下），4.4 聚合优先占低槽 + 标量分区 ⇒ print @-0x31、
//     guid @-0x1c（4 处 lea + 2 处 mov 共 6 行）。声明序/pad/临时链均不可翻（N82附）。
//   其余 2 行 = `mov 0x2(%eax),%ebx` 单段直落 vs 4.4 两段中转（N18a）。
//   帧尺寸 sub $0xec、26 个 vararg 装载序与全部 call 目标/宽度与 ORIG 逐条一致。
// b28d-65 复核：去 (int)m_addInfo 冗余 cast 消 N18a 两段中转 ⇒ 165=165；
//   残余仅 N80（ctor 前实参保活）+ N82附（print@-0x2d/guid@-0x80 槽向）+ N107
//   （取址 add-vs-lea）三族，P226 语句表达式实测零收益 ⇒ 维持封顶。
// [b28d-solo-cUserHistoryLog] DIFF 166/165：P227 全表达式临时链（Print()(arg) 作末实参）
// 翻案 N80+N82附——4.4 复现 ORIG 的 ctor 前实参保活与 print/guid 槽向；残差仅 N107：
// 4.4 `add $0x15,%eax; mov %eax,%ebx` vs ORIG `lea 0x15(%eax),%ebx`（数组名退化/
// 语句表达式/reinterpret_cast 三形实测同形；N107 六形探针封顶）。
// [2026-09-09 复核] 上面 N80/N82附/N18a 历史族已全部消解，当前仅剩 N107 ×1；
// 补测 union 数组视角/引用实参/`(char*)&`/`static_cast<const void*>` 等共 16 形
// （/tmp/hlprobe/tb4/tb5/tb6）全部产 add+mov ⇒ 成员取址 N107 封顶。
// [SweepHistoryLog2 2026-09-09 复核] 再试 12 形（/tmp/print_probe{,2,3,4}.cpp 双后端）：
// 引用形参恒 add、唯指针形参才 lea（ORIG operator() 为引用，nm 0x084ebe82 实证）⇒ 维持封顶。
#include "cUserHistoryLog/PrivateStoreItemAdd.inc"
                                          int addNew, int addOld, eItemAddReason reason,
                                          char const* name1, char const* name2)
{
    m_user->LogHistory(
        "Item+,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
        (int)invenType, item.m_addInfo, addNew, addOld, (int)reason,
        name1, name2,
        item.GetUpgrade(),
        (unsigned)item.m_fieldb,
        ((const InvenByteView*)&item)->m_b25, ((const InvenByteView*)&item)->m_b26,
        ((const InvenByteView*)&item)->m_b27, ((const InvenByteView*)&item)->m_b28,
        ((const InvenByteView*)&item)->m_b29, ((const InvenByteView*)&item)->m_b2a,
        ((const InvenByteView*)&item)->m_b2b, ((const InvenByteView*)&item)->m_b2c,
        ((const InvenByteView*)&item)->m_b2d, ((const InvenByteView*)&item)->m_b2e,
        ((const InvenByteView*)&item)->m_b2f, ((const InvenByteView*)&item)->m_b30,
        ((const InvenByteView*)&item)->m_b31, ((const InvenByteView*)&item)->m_b32,
        item.m_field0,            // ORIG IVar1 = *param_3（item+0x00 字节）
        item.GetReSealCount(),
        CItemGloballyUniqueIdentifierPrint()(
            *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
}

// ORIG T 0x868412a（报告全文）："Item-,..."——注意 %s 只有一个名字参数（param_8）。
// b21 NEAR：语义与 ORIG 全等（%s 仅 name2=param_8 正确）。修：补齐 item+0x00 字节 vararg。
// b28c-L2 fresh 复验 ndiff 13（orig 163/our 164）：封顶族与 PrivateStoreItemAdd 同
//   （N80 具名 print ctor 前实参保活 + N82附 聚合/标量槽向 + N18a 单段直落），判词见上。
// b28d-65 复核：去 (int)m_addInfo 冗余 cast ⇒ 163=163；残余同 Add（N80+N82附+N107）。
// [b28d-solo-cUserHistoryLog] DIFF 164/163：P227 同 PrivateStoreItemAdd；残差仅 N107。
// [2026-09-09 复核] 同上：仅剩 N107 ×1（16 形探针封顶）。
// [SweepHistoryLog2 2026-09-09 复核] 同上 12 形复验（引用形参恒 add、指针形参才 lea）⇒ 维持封顶。
#include "cUserHistoryLog/PrivateStoreItemDel.inc"
                                          int delNew, int delOld, eItemDelReason reason,
                                          char const* name1, char const* name2)
{
    m_user->LogHistory(
        "Item-,%d,%d,%d,%d,%d,\"%s\",%d,%d,"
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
        (int)invenType, item.m_addInfo, delNew, delOld, (int)reason,
        name2,
        item.GetUpgrade(),
        (unsigned)item.m_fieldb,
        ((const InvenByteView*)&item)->m_b25, ((const InvenByteView*)&item)->m_b26,
        ((const InvenByteView*)&item)->m_b27, ((const InvenByteView*)&item)->m_b28,
        ((const InvenByteView*)&item)->m_b29, ((const InvenByteView*)&item)->m_b2a,
        ((const InvenByteView*)&item)->m_b2b, ((const InvenByteView*)&item)->m_b2c,
        ((const InvenByteView*)&item)->m_b2d, ((const InvenByteView*)&item)->m_b2e,
        ((const InvenByteView*)&item)->m_b2f, ((const InvenByteView*)&item)->m_b30,
        ((const InvenByteView*)&item)->m_b31, ((const InvenByteView*)&item)->m_b32,
        item.m_field0,            // ORIG IVar1 = *param_3（item+0x00 字节）
        item.GetReSealCount(),
        CItemGloballyUniqueIdentifierPrint()(
            *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
}

// ============================================================================
// 账号金库 / 副本进入 / 角色死亡历史日志（DBHistoryDeps 批次；各报告汇编+C 全文已读）
// ============================================================================

// ORIG T 0x08683cee（ItemAddCargo.md）："Inven+,%d,%d,%d"（itemIdx, count, slotType）。
#include "cUserHistoryLog/ItemAddCargo.inc"
{
    m_user->LogHistory("Inven+,%d,%d,%d", itemIdx, count, slotType);
}

// ORIG T 0x086868b4（AccountCargoAddItem.md）：'AccountCargo+,%d,%d,%d,%d'
// ——ORIG 变参首参是 reason（param_5），其后才是 itemIdx/count/slotType。
#include "cUserHistoryLog/AccountCargoAddItem.inc"
                                          eItemAddReason reason)
{
    m_user->LogHistory("AccountCargo+,%d,%d,%d,%d",
                       static_cast<int>(reason), itemIdx, count, slotType);
}

// ORIG T 0x086868ee（AccountCargoDelItem.md）：与 Add 对应的 "AccountCargo-,..."
// （变参顺序同上：reason 在前）。
#include "cUserHistoryLog/AccountCargoDelItem.inc"
                                          eItemDelReason reason)
{
    m_user->LogHistory("AccountCargo-,%d,%d,%d,%d",
                       static_cast<int>(reason), itemIdx, count, slotType);
}

// ORIG T 0x08684dfa（CharacDie.md）：'Die,"%s",%d,%d'
// （两个 ushort 零扩展后按 %d 打印，ORIG movzbl/movzwl 口径一致）。
#include "cUserHistoryLog/CharacDie.inc"
                                unsigned short unkF)
{
    m_user->LogHistory("Die,\"%s\",%d,%d", name,
                       static_cast<unsigned>(unkD), static_cast<unsigned>(unkF));
}

// ORIG T 0x086849a2（EnterDungeon_086849a2.md）：'DungeonEnter,"%s",%d,%d,%s'
// ——第 3 个 %d 是形参 4（state）、%s 是形参 3（memberNames），按报告交错转写。
#include "cUserHistoryLog/EnterDungeon_1.inc"
                                   const char* memberNames, int state)
{
    m_user->LogHistory("DungeonEnter,\"%s\",%d,%d,%s",
                       dungeonName, unk, state, memberNames);
}

// ORIG T 0x0868491a（EnterDungeon.md 主报告）：
// 'AdvanceDungeonEnter,%d,%d,%d,%s,%d,%d'——char 形参经 movsbl 符号扩展后打印。
#include "cUserHistoryLog/EnterDungeon_2.inc"
                                   const char* memberNames, char flag, int state)
{
    m_user->LogHistory("AdvanceDungeonEnter,%d,%d,%d,%s,%d,%d",
                       dungeonIdx, a, b, memberNames, static_cast<int>(flag), state);
}

// ============================================================================
// ORIG T 0x08685452（Auction.md 全量转写）：按 m_type 0/1/2/3 分派
// LogHistory 格式串（SIG_AUCTION_LOG_MESSAGE_AG 布局见 SigTypes.h）。
// ============================================================================
#include "cUserHistoryLog/Auction.inc"
{
    switch (msg->m_type)
    {
        case 0:
            m_user->LogHistory("Auction_reg_s,%llu,%d,%u,%hhu,%d,%hu,%u",
                msg->m_key, msg->m_arg09, msg->m_arg11, msg->m_arg15,
                msg->m_arg16, msg->m_arg1a, msg->m_arg1c);
            break;
        case 1:
            m_user->LogHistory("Auction_reg_f,%llu,%d,%u,%hhu,%d,%hu,%u,%hhu",
                msg->m_key, msg->m_arg09, msg->m_arg11, msg->m_arg15,
                msg->m_arg16, msg->m_arg1a, msg->m_arg1c, msg->m_arg20);
            break;
        case 2:
            m_user->LogHistory("Auction_bid_s,%llu,%d,%d,%d",
                msg->m_key, msg->m_arg09, msg->m_arg11, msg->m_arg0d);
            break;
        case 3:
            m_user->LogHistory("Auction_bid_f,%llu,%d,%d",
                msg->m_key, msg->m_arg09, msg->m_arg0d);
            break;
        default:
            m_user->LogHistory("Auction_unknown,%d", (int)msg->m_type);
            break;
    }
}

// ============================================================================
// ORIG T 0x08685654（AuctionPoint.md 全量转写）：与 Auction 同型，POINT_* 串。
// ============================================================================
#include "cUserHistoryLog/AuctionPoint.inc"
{
    switch (msg->m_type)
    {
        case 0:
            m_user->LogHistory("POINT_reg_s,%llu,%d,%u,%hhu,%d,%hu,%u",
                msg->m_key, msg->m_arg09, msg->m_arg11, msg->m_arg15,
                msg->m_arg16, msg->m_arg1a, msg->m_arg1c);
            break;
        case 1:
            m_user->LogHistory("POINT_reg_f,%llu,%d,%u,%hhu,%d,%hu,%u",
                msg->m_key, msg->m_arg09, msg->m_arg11, msg->m_arg15,
                msg->m_arg16, msg->m_arg1a, msg->m_arg1c);
            break;
        case 2:
            m_user->LogHistory("POINT_bid_s,%llu,%d,%d",
                msg->m_key, msg->m_arg09, msg->m_arg0d);
            break;
        case 3:
            m_user->LogHistory("POINT_bid_f,%llu,%d,%d",
                msg->m_key, msg->m_arg09, msg->m_arg0d);
            break;
        default:
            m_user->LogHistory("POINT_unknown,%d", (int)msg->m_type);
            break;
    }
}

// ============================================================================
// Wave5 仅ORIG 清零批（TaiwanLog）——54 个 ORIG-only 方法补齐。
// 逐函数依据 docs/class_func_reports/cUserHistoryLog/ 报告（汇编+反编译C）
// 与 df_game_r objdump（无报告者已注明地址）转写。
// ============================================================================

// ORIG _ZN15cUserHistoryLog16AntibotPacketLogEN20AntibotPacketLogType1TE @0x08686dd0
#include "cUserHistoryLog/AntibotPacketLog.inc"
{
    m_user->LogHistory("AntibotPacketLog,%d", (int)type);
}

// ORIG _ZN15cUserHistoryLog15BuyCashShopItemEjmjPKccN19CeraShopPaymentType1TEy
//      @0x08686ea0：价格 u64 经 NumberToString 渲染后入参。
#include "cUserHistoryLog/BuyCashShopItem.inc"
                                      unsigned int count, const char* buyer, char flag,
                                      CeraShopPaymentType::T payType,
                                      unsigned long long price)
{
    m_user->LogHistory("BuyCashItem, %u, %u, %u, %s, %d, %d, %s",
                       itemIdx, (unsigned int)uid, count, buyer, (int)flag,
                       (int)payType, NumberToString(price, 0));
}

// ORIG _ZN15cUserHistoryLog12CashInvenAddE19eCashInvenAddReasonjjm @0x0868683a
#include "cUserHistoryLog/CashInvenAdd.inc"
                                   unsigned int b, unsigned long c)
{
    m_user->LogHistory("CashInven+,%u,%u,%u,%u",
                       (unsigned)reason, a, b, (unsigned)c);
}

// ORIG _ZN15cUserHistoryLog12CashInvenDelE19eCashInvenDelReasonjjmj @0x08686874
#include "cUserHistoryLog/CashInvenDel.inc"
                                   unsigned int b, unsigned long c, unsigned int d)
{
    m_user->LogHistory("CashInven-,%u,%u,%u,%u,%u",
                       (unsigned)reason, a, b, (unsigned)c, d);
}

// ORIG _ZN15cUserHistoryLog9CharacAddEi @0x08682a92：
//   栈上 TimeLog_ 计时后读 m_charac[idx].m_field26(+0x26 growtype) 日志 "Charac+,%d"。
//   【推断】TimeLog_ 为纯计时局部对象，无可观察副作用，重建中省略。
#include "cUserHistoryLog/CharacAdd.inc"
{
    // ORIG 栈上 TimeLog_ 局部对象调 setTime()（结果未用，保留以对齐字节）。
    TimeLog_ tl;
    tl.setTime();
    // CUser::m_characList @+0x796e8（ORIG 0x08682a92 operator[] 基址），
    // 表达式内联传递（P134——引用局部会挤占 -0xc 槽并使实参装载序漂移）。
    m_user->LogHistory("Charac+,%d",
        (int)m_user->m_characList[idx].m_field26);
}

// ORIG _ZN15cUserHistoryLog9CharacDelEi @0x08682ae0（与 CharacAdd 同型，"Charac-,"）
#include "cUserHistoryLog/CharacDel.inc"
{
    TimeLog_ tl;
    tl.setTime();
    m_user->LogHistory("Charac-,%d",
        (int)m_user->m_characList[idx].m_field26);
}

// ORIG _ZN15cUserHistoryLog6DiePvPE23ENUM_PVP_HISTORY_REASONjjiiiiPKc @0x08686490
#include "cUserHistoryLog/DiePvP.inc"
                             int c, int d, int e, int f, const char* name)
{
    m_user->LogHistory("PVP,%d,%u,%u,%d,%d,%d,%d,\"%s\"",
                       (int)reason, a, b, c, d, e, f, name);
}

// ORIG _ZN15cUserHistoryLog23Enchant3rdChronicleItemEbiiii @0x08684c28
#include "cUserHistoryLog/Enchant3rdChronicleItem.inc"
                                              int equipType, int count)
{
    if (isEnchant)
        m_user->LogHistory("3rdEnchant+,%d,%d,%d,%d", itemIdx, materialIdx, equipType, count);
    else
        m_user->LogHistory("3rdEnchant-,%d,%d,%d,%d", itemIdx, materialIdx, equipType, count);
}

// ORIG _ZN15cUserHistoryLog11EnchantItemEiiibPKcS1_ @0x08684d74
#include "cUserHistoryLog/EnchantItem.inc"
                                  const char* name1, const char* name2)
{
    if (up)
        m_user->LogHistory("Enchant+,%d,%d,%d,\"%s\",\"%s\"", a, b, c, name1, name2);
    else
        m_user->LogHistory("Enchant-,%d,%d,%d,\"%s\",\"%s\"", a, b, c, name1, name2);
}

// ORIG _ZN15cUserHistoryLog6EndPvPE23ENUM_PVP_HISTORY_REASONbj @0x0868642e
#include "cUserHistoryLog/EndPvP.inc"
{
    // ORIG 0x08687f1x：bool 形参 spill 槽 movzbl 直接进 %d 实参（无三目岛）。
    m_user->LogHistory("PVP,%d,%d,%u", (int)reason, flag, v);
}

// ORIG _ZN15cUserHistoryLog10EndRidableENS_12eRidableTypeEi @0x0868695c：注意参数换位
#include "cUserHistoryLog/EndRidable.inc"
{
    m_user->LogHistory("Transform,%d,%d,%d", 0, a, (int)type);
}

// ORIG _ZN15cUserHistoryLog6ExpAddEii13eExpAddReasoni @0x08684bd0：
//   仅当 reason>0xc 或 (1<<reason)&0x1892==0 时记录（排除常规来源）；
//   日志第三参数与判断均用 reason（reasonIdx 未参与）。
// [b28d-solo-cUserHistoryLog] DIFF（orig 32 / our 33）：尾岛已按 P223 负条件 goto 尾空标签复现
// （ORIG 0x08684c1f `jmp +0x52; nop` 空 else 占位岛）；残差仅 N18a 反向
// （ORIG 复用 cmp 的 %eax 作 shl 源，4.4 对参数表达式二次使用恒重载
// `mov 0x14(%ebp),%eax`；e7/e9/e10/语句表达式/一元加四形探针实测均重载）。
// [2026-09-09 复核] 补测 register 形参/register 局部/语句表达式/逗号表达式/有符号移位
// 等 6 形（/tmp/hlprobe/exp2/exp3/exp4）：register 形参可消重载但寄存器角色全变
// （%eax=reason / %ebx=常量 / %esi=结果 + push esi），其余仍重载 ⇒ N18a 反向封顶。
// [SweepHistoryLog2 2026-09-09 三度复核] 再试 10 形（/tmp/exp_probe.cpp f1..f14，c6root 4.4.7
// 与 -B cc1plus446bin 双编同结论）：①(unsigned)reason 两侧同写；②三目 A?true:C；
// ③反向 !(A&&B)；④正向 A>=..&&..；⑤1u→1（折 sar/and $1）；⑥去 cast（折 jg，证 enum 底层
// signed，须保留 (unsigned)）；⑦register unsigned 局部（落 %ebx +2 条）；⑧register enum 局部
// （+2 条）；⑨语句表达式 ({reason;})；⑩逗号 (0,reason)——**全部仍多 1 条
// `mov 0x14(%ebp),%eax`**。机理：4.1 把 || 左操作数装载的 %eax 保留给右操作数 shl 源；
// 4.4 -O0 无 CSE，第二次使用恒重读形参槽 ⇒ 无合法源形可造 ⇒ N18a 反向封顶（ORIG 31/我们 32）。
#include "cUserHistoryLog/ExpAdd.inc"
{
    // [b28d-solo-cUserHistoryLog] N6 翻案：`(int)` cast 阻断 4.4 前端的 bool 位测试折叠
    // （探针 /tmp/hlprobe/Game/probe4.cpp k1：`(int)((1u<<r)&0x1892u)==0` 产 ORIG 的
    // shl;and $0x1892;test，无 cast 恒折 shr;and $1）。
    // 证据（ORIG 0x08684bd7..0x08684c27）：`mov 0x14(%ebp),%eax; cmp $0xc,%eax; ja body;
    // shl/and/test; jne <nop 岛>; body: ... call; jmp +0x52; nop`。P223 尾空标签复现
    // （探针 probe4.cpp e3 与 ORIG 同形，仅 reason 重载一条差）。
    if (!((0xc < (unsigned)reason) ||
          (((int)((1u << reason) & 0x1892U)) == 0)))
        goto end;
    m_user->LogHistory("Exp+,%d,%d,%d", exp, sp, reason);
end:;
}

// ORIG _ZN15cUserHistoryLog16FullIPCounterLogEiiiiiPc @0x08685d54
#include "cUserHistoryLog/FullIPCounterLog.inc"
{
    m_user->LogHistory("D_IPCount,%d,%d,%d,%d,%d,%s", a, b, c, d, e, ip);
}

// ---- GuildCargo 族（全部为 "GuildCargo,%d..." 前缀透传；ORIG 地址见各注释） ----
#include "cUserHistoryLog/GuildCargo.inc"
{
    m_user->LogHistory("GuildCargo,%d,%d", (int)reason, a);
}
#include "cUserHistoryLog/GuildCargoResult.inc"
                                       int a, int b)                              // 0x08686512
{
    m_user->LogHistory("GuildCargo,%d,%d,%d", (int)reason, a, b);
}
#include "cUserHistoryLog/GuildCargoHistory.inc"
{
    m_user->LogHistory("GuildCargo,%d,%d", (int)reason, a);
}
#include "cUserHistoryLog/GuildCargoHistoryResult.inc"
{
    m_user->LogHistory("GuildCargo,%d,%d", (int)reason, a);
}
#include "cUserHistoryLog/GuildCargoCheckPushItem.inc"
                                              int a, int b, int c, int d, int e, int f)  // 0x0868659c
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e, f);
}
#include "cUserHistoryLog/GuildCargoCheckPushItemResult.inc"
                                                    int a, int b, int c, int d, int e, int f) // 0x086865ea
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e, f);
}
#include "cUserHistoryLog/GuildCargoPushItem.inc"
                                         int a, int b, int c, int d, int e, int f,
                                         unsigned char g)                         // 0x08686638
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d,%d",
                       (int)reason, a, b, c, d, e, f, g & 0xff);
}
#include "cUserHistoryLog/GuildCargoItemRecovery.inc"
                                             int a, int b, int c, int d, int e,
                                             unsigned char f)                     // 0x08686694
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d",
                       (int)reason, a, b, c, d, e, f & 0xff);
}
#include "cUserHistoryLog/GuildCargoPopItem.inc"
                                        int a, int b, int c, int d, int e)        // 0x086866ea
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e);
}
#include "cUserHistoryLog/GuildCargoPopItemResult.inc"
                                              int a, int b, int c, int d, int e, int f) // 0x08686780
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e, f);
}
#include "cUserHistoryLog/GuildCargoPushItemResult.inc"
                                               int a, int b, int c, int d, int e, int f) // 0x08686732
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e, f);
}
#include "cUserHistoryLog/GuildCargoMoveItem.inc"
                                         int a, int b)                            // 0x086867ce
{
    m_user->LogHistory("GuildCargo,%d,%d,%d", (int)reason, a, b);
}
#include "cUserHistoryLog/GuildCargoMoveItemResult.inc"
                                               int a, int b, int c)               // 0x08686800
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d", (int)reason, a, b, c);
}

// ---- 公会日期族：ORIG 取 GlobalData::s_systemTime_ 当前秒 + localtime_r，
//      "%d/%d/%d" = tm_year+1900(tm_year+0x76c)/tm_mon+1/tm_mday ----
// ORIG _ZN15cUserHistoryLog11GuildCreateEjPKcj @0x08685978
#include "cUserHistoryLog/GuildCreate.inc"
                                  unsigned int characNo)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    struct tm* r = localtime_r(&now, &broken);
    // b28d-50：同 GuildAddExp——ORIG 0x086859a2 段三具名日期槽 -0x14/-0x10/-0xc。
    int year = r->tm_year;
    int month = r->tm_mon + 1;
    int day = r->tm_mday;
    m_user->LogHistory("GuildCreate,\"%d/%d/%d\",%d,\"%s\",%d",
                       year + 0x76c, month, day,
                       guildKey, guildName, characNo);
}

// ORIG _ZN15cUserHistoryLog9GuildJoinEjPKcjtc @0x08685a48
// b28d-65：三具名日期局部（year/month/day，P0 槽往返）⇒ 53=53 逐条全等（ORIG 0x08685a48）。
#include "cUserHistoryLog/GuildJoin.inc"
                                unsigned int characNo, unsigned short a, char b)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    struct tm* r = localtime_r(&now, &broken);
    int year = r->tm_year;
    int month = r->tm_mon + 1;
    int day = r->tm_mday;
    m_user->LogHistory("GuildJoin,\"%d/%d/%d\",%d,\"%s\",%d,%d,%d",
                       year + 0x76c, month, day,
                       guildKey, guildName, characNo, (unsigned)a, (int)b);
}

// ORIG _ZN15cUserHistoryLog8GuildOutEjPKcjct @0x08685b00
// b28d-65：同 GuildJoin（三具名日期局部）⇒ 53=53（ORIG 0x08685b00）。
#include "cUserHistoryLog/GuildOut.inc"
                               unsigned int characNo, char a, unsigned short b)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    struct tm* r = localtime_r(&now, &broken);
    int year = r->tm_year;
    int month = r->tm_mon + 1;
    int day = r->tm_mday;
    m_user->LogHistory("GuildOut,\"%d/%d/%d\",%d,\"%s\",%d,%d,%d",
                       year + 0x76c, month, day,
                       guildKey, guildName, characNo, (int)a, (unsigned)b);
}

// ORIG _ZN15cUserHistoryLog16GuildChangeGradeEjPKcjjhh @0x08685c4e
// b28d-65：同 GuildJoin（三具名日期局部）⇒ 55=55（ORIG 0x08685c4e）。
#include "cUserHistoryLog/GuildChangeGrade.inc"
                                       unsigned int characNo, unsigned int grade,
                                       unsigned char oldGrade, unsigned char newGrade)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    struct tm* r = localtime_r(&now, &broken);
    int year = r->tm_year;
    int month = r->tm_mon + 1;
    int day = r->tm_mday;
    m_user->LogHistory("GuildChangeGrade,\"%d/%d/%d\",%d,\"%s\",%d,%d,%d,%d",
                       year + 0x76c, month, day,
                       guildKey, guildName, characNo, grade,
                       (unsigned)(oldGrade & 0xff), (unsigned)(newGrade & 0xff));
}

#include "cUserHistoryLog/useEmblem.inc"
                                const char* d, unsigned int e)
{
    // b28c-L2: 删 (int) 冗余 cast——-m32 下 unsigned long 即 32 位，cast 不改变
    // 压栈宽度但诱发 4.4 callee-saved %ebx 缓存 + 帧 0x28→0x24（P41 变体/BM13
    // charge 同族），删后 4.4 回到 ORIG 0x08685a08 的 edx 逐段即用即弃形。
    m_user->LogHistory("UseEmblem,%d,%d,%d,%s,%d", a, b, c, d, e);
}

// ORIG _ZN15cUserHistoryLog12StartRidableENS_12eRidableTypeEi @0x08686928：参数换位同 EndRidable
#include "cUserHistoryLog/StartRidable.inc"
{
    m_user->LogHistory("Transform,%d,%d,%d", 1, a, (int)type);
}

// ORIG _ZN15cUserHistoryLog11HackDetectedEjjjjjj @0x08685320：按尾参是否为零递增格式长度
#include "cUserHistoryLog/HackDetected.inc"
                                   unsigned int d, unsigned int e, unsigned int f)
{
    if (e == 0 && f == 0)
        m_user->LogHistory("Hack,%d,%d,%d,%d", a, b, c, d);
    else if (f == 0)
        m_user->LogHistory("Hack,%d,%d,%d,%d,%d", a, b, c, d, e);
    else
        m_user->LogHistory("Hack,%d,%d,%d,%d,%d,%d", a, b, c, d, e, f);
}

// ORIG _ZN15cUserHistoryLog15HadesHistoryLogEjjjj @0x08685418
#include "cUserHistoryLog/HadesHistoryLog.inc"
                                      unsigned int d)
{
    m_user->LogHistory("Hades,%d,%d,%d,%d", a, b, c, d);
}

// ORIG _ZN15cUserHistoryLog12IPCounterLogEiiiiiPc @0x08685d0c
#include "cUserHistoryLog/IPCounterLog.inc"
{
    m_user->LogHistory("IPCount,%d,%d,%d,%d,%d,%s", a, b, c, d, e, ip);
}

// ORIG _ZN15cUserHistoryLog21IPCounterLog_InsertIpEiiPc @0x08685d9c
#include "cUserHistoryLog/IPCounterLog_InsertIp.inc"
{
    m_user->LogHistory("IPCountInt,%d,%d,%s", a, b, ip);
}

// ORIG _ZN15cUserHistoryLog18ItemMakingSkillAddE21ENUM_ITEM_MAKING_TYPEiis @0x08686a88
#include "cUserHistoryLog/ItemMakingSkillAdd.inc"
{
    m_user->LogHistory("ItemMakingSkill+,%d,%d,%d,%d",
                       (int)type, a, b, (int)c);
}

// ORIG _ZN15cUserHistoryLog20MercenaryCompetitionEjcc @0x08686aca
#include "cUserHistoryLog/MercenaryCompetition.inc"
{
    m_user->LogHistory("MercenaryCompetition,%u,%d,%d", a, (int)b, (int)c);
}

// ORIG _ZN15cUserHistoryLog15MercenaryReturnEjiiiii @0x08686b0a（注意 "MecenaryReturn" 拼写照抄 ORIG）
#include "cUserHistoryLog/MercenaryReturn.inc"
{
    m_user->LogHistory("MecenaryReturn,%u,%d,%d,%d,%d,%d", mercNo, a, b, c, d, e);
}

// ORIG _ZN15cUserHistoryLog5QPSubEiii @0x0868885a
#include "cUserHistoryLog/QPSub.inc"
{
    m_user->LogHistory("QP-,%d,%d,%d", before, questIdx, sub);
}

// ORIG _ZN15cUserHistoryLog11SecuServiceE22ENUM_SECU_SERVICE_TYPE24ENUM_SECU_SERVICE_ACTION
//      @0x0868479c：仅 type 0/1 有日志串。
#include "cUserHistoryLog/SecuService.inc"
{
    if (type == ENUM_SECU_SERVICE_TYPE_GOBLIN)
        m_user->LogHistory("s_gob,%d", (int)action);
    else if (type == ENUM_SECU_SERVICE_TYPE_CARD)
        m_user->LogHistory("s_card,%d", (int)action);
}

// ORIG _ZN15cUserHistoryLog27SelectCubeForPremiumServiceEi @0x08686da0
#include "cUserHistoryLog/SelectCubeForPremiumService.inc"
{
    if (m_user != 0)
        m_user->LogHistory("SelectCubeForPremiumService,%d", a);
    return;
}

// ORIG _ZN15cUserHistoryLog21SendBuffertoClientLogEishb @0x08686e3a
#include "cUserHistoryLog/SendBuffertoClientLog.inc"
{
    // b28d-50：ORIG 0x08686e3a 在右起 varargs（c→%esi, b→%ebx, a→%ecx）之后才求
    // %s 三元并直留 %eax（无槽，push esi/ebx + sub 0x30）；具名 `name` 局部会提前
    // 物化到 -0xc 槽并改帧（0x44）。内联三元即复现。
    m_user->LogHistory("SendBuffertoClient [%s],%d,%d,%d",
                       (kind == 0) ? "ANTIBOT" : "DPROTO",
                       (int)a, (unsigned)b, (unsigned)c);
}

// ORIG _ZN15cUserHistoryLog23SendMailForCeraShopGiftEjcjj @0x086848da
#include "cUserHistoryLog/SendMailForCeraShopGift.inc"
                                              unsigned int a, unsigned int b)
{
    m_user->LogHistory("MailSG,%u,%d,%u,%u", mailIdx, (int)flag, a, b);
}

// ORIG _ZN15cUserHistoryLog8StartPvPE23ENUM_PVP_HISTORY_REASONPKcS2_S2_S2_S2_S2_S2_S2_
//      @0x086863b6：两条 PVP 行（前 4 名/后 4 名角色名）。
#include "cUserHistoryLog/StartPvP.inc"
                               const char* n3, const char* n4, const char* n5,
                               const char* n6, const char* n7, const char* n8)
{
    m_user->LogHistory("PVP,%d,\"%s\",\"%s\",\"%s\",\"%s\"", (int)reason, n1, n2, n3, n4);
    m_user->LogHistory("PVP,%d,\"%s\",\"%s\",\"%s\",\"%s\"", (int)reason, n5, n6, n7, n8);
}

// ORIG _ZN15cUserHistoryLog21UseAvatarOptionChangeEiiii @0x08686d0e
#include "cUserHistoryLog/UseAvatarOptionChange.inc"
{
    if (m_user != 0)
        m_user->LogHistory("UseAvatarOptionChange,%d,%d,%d,%d", a, b, c, d);
    return;
}

// ORIG _ZN15cUserHistoryLog6UseDyeEiiiRKSsi @0x08686cba
#include "cUserHistoryLog/UseDye.inc"
{
    if (m_user != 0)
        m_user->LogHistory("UseDye ,%d,%d,%d,%s,%d", a, b, c, dyeName.c_str(), d);
    return;
}

// ---- 物品网格坐标视图：ORIG 存于 this+0x40/+0x42（类尾部 m_pad 区）， ----
// ---- 此前 static helper 直读偏移，现改用具名成员 m_itemX/m_itemY。     ----

// ORIG W _ZN15cUserHistoryLog11getItemXPosEv @0x08696000：
//   mov 0x8(%ebp),%eax; movzwl 0x40(%eax),%eax（单 disp 折叠）——具名成员
//   m_itemX（CUser.h:2153 +0x40）直读。
#include "cUserHistoryLog/getItemXPos.inc"
{
    return m_itemX;
}

// ORIG W _ZN15cUserHistoryLog11getItemYPosEv @0x0869600c：movzwl 0x42(%eax)。
#include "cUserHistoryLog/getItemYPos.inc"
{
    return m_itemY;
}

// ORIG W _ZN15cUserHistoryLog12SetItemXYPosEtt @0x0822f084：short 形参 spill
// 槽（-0x4/-0x8）+ movzwl 回读 + `mov %dx,0x40(%eax)` disp 折叠写。
#include "cUserHistoryLog/SetItemXYPos.inc"
{
    m_itemX = x;
    m_itemY = y;
}

// ORIG _ZN15cUserHistoryLog13QuestCompleteEi @0x0868514e（格式串 ORIG .rodata 0x8cf0957）
#include "cUserHistoryLog/QuestComplete.inc"
{
    m_user->LogHistory("QuestComplete,%d", questIdx);
}

// ORIG _ZN15cUserHistoryLog13RedeemItemDelEii @0x086869bc（格式串 ORIG 0x8cf0e9d）
#include "cUserHistoryLog/RedeemItemDel.inc"
{
    m_user->LogHistory("RedeemItem-,%d,%d", a, b);
}

// ORIG _ZN15cUserHistoryLog16ItemLockUsingLogEhiiii @0x08689808（格式串 ORIG 0x8cf1153）
#include "cUserHistoryLog/ItemLockUsingLog.inc"
{
    m_user->LogHistory("ItemLock,%d,%d,%d,%d,%d", lockType & 0xff, a, b, c, d);
}

// ORIG _ZN15cUserHistoryLog9CeraPointE23ENUM_CERAPOINT_LOG_TYPEj @0x0868593c：
//   尾参为当前余额 CUser::GetCeraPoint()。
// [b28d-solo-cUserHistoryLog] IDENTICAL 19/19：第三实参**去 (int) cast** 直传 unsigned
// （GetCeraPoint 返回 unsigned int，vararg %d 位模式不变、语义等价）⇒ 4.4 复现 ORIG
// 「balance 留 %eax / type 先入 %ecx 跨 receiver 保活 / receiver 用 %edx」装载序
// （探针 /tmp/hlprobe/Game/probe3.cpp fB 与 ORIG 逐条同形；旧 fA 带 cast 多 mov %eax,%ecx）。
#include "cUserHistoryLog/CeraPoint.inc"
{
    // 证据（ORIG 0x0868594f..0x08685971）：call GetCeraPoint 后
    //   mov 0xc(%ebp),%ecx（type 先入 ecx）→ mov 0x8(%ebp),%edx; mov (%edx),%edx
    //   → mov %eax,0x10(%esp)（balance 留 eax）→ mov 0x10(%ebp),%eax（amount）
    //   → mov %eax,0xc(%esp) → mov %ecx,0x8(%esp) → call。
    // 带 (int) 的第三实参（unsigned→int 转换节点）会使 4.4 把 balance 提前转存 %ecx
    // （fA 20 条）；去掉转换节点即得 ORIG 19 条全等（P41 变体：冗余 cast 诱发寄存器改派）。
    m_user->LogHistory("CeraPoint,%d,%d,%d", (int)type, amount,
                       m_user->GetCeraPoint());
}

// ORIG _ZN15cUserHistoryLog5LoginEPKcS1_iihi @0x08683d86（Login.md 全量转写）：
//   扫描背包 invenType=0 槽 10..0x13 统计强化/稀有件、invenType=2 全槽计数、
//   回归判定 local_60、专家职业等级，输出 "IP+,..." 18 字段行。
// [b28d-solo-cUserHistoryLog] 裸偏移 *(int*)(charac+0x7b) → 具名
// _Charac_info::m_field7b（CUser.h:546）复现 ORIG 单条 disp。
// [SweepHistoryLog2 2026-09-09 翻案 N108/P223] 两循环守卫统一改「单 continue + 正向 if 包裹
// body」形后 ndiff=0（orig=239/our=239，p167 CLEAR）：
//   ① `if (it == 0) continue;` 保留 ORIG 的 `je <nop 岛>`（4.4 对该单 continue 恰发 1 条）；
//   ② 第二守卫 `it->m_addInfo` 不写 continue，改 `if (it->m_addInfo != 0) { body }` 正向包裹
//      ⇒ 复现 ORIG 体尾唯一 `jmp <增量>`（实测：`||` 合并 continue 多 2 条 jmp、
//      两个独立 continue 多 2 条、goto 双标签多 2 条、纯嵌套 if 少 1 条；唯有「1 continue +
//      1 正向 if」两处各差 0）。
//   探针 /tmp/hlprobe 复用与 check_tu_routed fresh 双证；旧判词「正向包裹各劣 4 条」已作废。
#include "cUserHistoryLog/Login.inc"
                            unsigned char loginType, int param6)
{
    // 声明序=栈槽序（P28）：region/money 先声明得 -0x5c/-0x58 槽，余者顺延。
    int region = 0;                              // local_60 回归分类（ORIG 顶部零初始化）
    int money = 0;                               // ORIG 亦先零初始化后覆写（+0x019/+0x05d）
    int upgradeFirst = 0;                        // local_58（首件强化）
    int firstItemId = 0;                         // local_54（10 号槽 itemId）
    int upgradeSum = 0;                          // local_50（其余件强化和）
    int rareCount = 0;                           // local_4c（rarity 0/1 计数）
    int petCount = 0;                            // local_48（invenType=2 计数）

    money = reinterpret_cast<CInventory*>(
                reinterpret_cast<char*>(const_cast<_Charac_info*>(m_user->getCurCharacR()))
                + 0xda)->get_money();

    for (int slot = 10; slot < 0x14; ++slot)
    {
        // ORIG 每循环迭代重复 getCurCharacInvenR（P48 去缓存，禁循环外缓存）
        const Inven_Item* it = m_user->getCurCharacInvenR()->GetInvenRef(0, slot);
        // [b28d-solo-cUserHistoryLog 2026-09-09] ORIG 0x08683e19/0x08683e27 两守卫分落
        // +0x118 nop 岛 / +0x119 增量段，++rareCount 后 `jmp +0x119`；4.4 对 continue
        // 恒生成 nop+jmp 岛（多 1 条 jmp）。实测：拆两 continue（+2）、goto 多源共享尾标签
        // （ndiff 同 2）、循环2 改 || continue（+2）均不闭合 ⇒ N108/P223 封顶。
        if (it == 0)
            continue;
        if (it->m_addInfo != 0)
        {
            if (slot == 10)
            {
                upgradeFirst = it->GetUpgrade() & 0xff;
                firstItemId = it->m_addInfo;
            }
            else
            {
                upgradeSum += it->GetUpgrade() & 0xff;
            }
            CItem* item = G_CDataManager()->find_item(it->m_addInfo);
            if (item != 0)
            {
                // ORIG 单次 get_rarity 缓存后判 0/1（P183 反向警告：二次访问须具名缓存）
                int rarity = item->get_rarity();
                if (rarity == 0 || rarity == 1)
                    ++rareCount;
            }
        }
    }
    for (int slot = 0; slot < 0x74; ++slot)
    {
        // ORIG 每循环迭代重复 getCurCharacInvenR（P48 去缓存）
        const Inven_Item* it = m_user->getCurCharacInvenR()->GetInvenRef(2, slot);
        if (it == 0)
            continue;
        if (it->m_addInfo != 0)
            ++petCount;
    }

    // +0x7b 处回归用户标记 = _Charac_info::m_field7b（CUser.h:546，int）。
    // ORIG 0x08683dxx 单条 `mov 0x7b(%eax),%eax`（具名成员 disp 折叠）；
    // 裸偏移 `*(int*)(char*+0x7b)` 使 4.4 发 `add $0x7b; mov (%eax),%eax` 两步（§4.3 禁裸偏移）。
    int returnUserFlag = m_user->getCurCharacR()->m_field7b;
    // ORIG 先取 GetFirstLogin 落字节槽(-0x39)再判（勿内联 bool 物化）
    unsigned char isFirstLogin = m_user->GetFirstLogin();
    if (returnUserFlag == 0 && isFirstLogin != 0)
        region = 0;
    else if (loginType == 0 && returnUserFlag != 0)
        region = 1;
    else if (loginType == 1 && returnUserFlag != 0)
        region = 2;
    else if (loginType == 2 && returnUserFlag != 0)
        region = 3;
    // else：ORIG 无 else 写，region 保持初值 0（勿补 -1）

    int expertJobLevel = 0;                      // local_3c
    STExpertJobScript* script =
        reinterpret_cast<STExpertJobScript*>(
            G_CDataManager()->GetExpertJobScript(m_user->GetCurCharacExpertJobType()));
    if (script != 0)
        expertJobLevel = script->GetLevel((unsigned)m_user->GetCurCharacExpertJobExp());

    m_user->LogHistory(
        "IP+,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%p",
        param2, param3, param4, (int)loginType, param6,
        m_user->get_charac_job(), m_user->get_charac_level(),
        (int)m_user->getCurCharFirstGrowType(), (int)m_user->getCurCharSecondGrowType(),
        region, money, upgradeFirst, firstItemId, upgradeSum, rareCount, petCount,
        m_user->GetCurCharacExpertJobType(), expertJobLevel,
        reinterpret_cast<void*>(m_user));
}
