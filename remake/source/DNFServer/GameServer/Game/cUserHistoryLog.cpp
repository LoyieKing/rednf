// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 cUserHistoryLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

#include "cUserHistoryLog/SetUser.inc"
}

// GiveupPvP(reason)：ORIG 0x868646c —— CUser::LogHistory(m_user, "PVP,%d", reason)。
#include "cUserHistoryLog/GiveupPvP.inc"
}

// ORIG 0x8686c36：m_user 非空时 LogHistory("SealItem ,%d,%d,%d", itemIdx, count, reason)。
#include "cUserHistoryLog/seal_Item.inc"
}

// ORIG 0x08686f1e（DeleteInvaildItem.md）：直接以 m_user 调 LogHistory
// （ORIG 无空指针检查），格式 "DeleteInvaildItem, %s, %d, %d"。
#include "cUserHistoryLog/DeleteInvaildItem.inc"
}

// SetTrader(accName, characName)：ORIG 0x8532036，strncpy 到 m_traderCharac(acc) 与 m_traderAccount(charac)
#include "cUserHistoryLog/SetTrader.inc"
}

// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] m_traderCharac[0] 读改逗号表达式旋钮
// `*(0, m_traderCharac)`（判词见 TradeBegin）⇒ 本函数 AE。
#include "cUserHistoryLog/AvatarItemAdd.inc"
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
}

// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 AvatarItemAdd ⇒ 本函数 AE。
#include "cUserHistoryLog/AvatarItemDel.inc"
}

// b28d-65 [N4/N102]：同 CreatureItemAdd；ORIG 0x08684f38。
// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
#include "cUserHistoryLog/CreatureItemDel.inc"
}

// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
#include "cUserHistoryLog/MoneyAddTrade.inc"
}

// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
#include "cUserHistoryLog/MoneySubTrade.inc"
}

#include "cUserHistoryLog/MoneyAdd.inc"
}

#include "cUserHistoryLog/MoneySub.inc"
}

// ORIG T 0x8682cc8（0x32 字节）：LogHistory("WP-,%d,%d,%d", cur, sub, reason)。
#include "cUserHistoryLog/WPSub.inc"
}

// ORIG T 0x8685bb8（0x40 字节）：GuildAddExp,"%d/%d/%d",%d,"%s",%d,%d
// （年/月/日/guildKey/guildDBInfo/characNo/exp）。
#include "cUserHistoryLog/GuildAddExp.inc"
}

// ORIG T 0x086853e6（0x32 字节）：LogHistory("CleanPad,%d,%d", a, b)。
#include "cUserHistoryLog/RequestCleanPad.inc"
}

#include "cUserHistoryLog/CoinAdd.inc"
}

#include "cUserHistoryLog/PayCoinAdd.inc"
}

#include "cUserHistoryLog/EventCoinAdd.inc"
}

// LevelUp(int, eLevelUpReason)：ORIG 0x86845b2，"Level+,%d,%d"
#include "cUserHistoryLog/LevelUp.inc"
}

// LevelDown(int, eLevelDownReason)：ORIG 0x86845de，"Level-,%d,%d"
#include "cUserHistoryLog/LevelDown.inc"
}

#include "cUserHistoryLog/CoinSub.inc"
}

#include "cUserHistoryLog/EventCoinSub.inc"
}

#include "cUserHistoryLog/PayCoinSub.inc"
}

#include "cUserHistoryLog/pvpMissionClearReward.inc"
}

#include "cUserHistoryLog/EnterDungeon.inc"
}

#include "cUserHistoryLog/LeaveDungeon.inc"
}

#include "cUserHistoryLog/LeaveDungeon_1.inc"
}

#include "cUserHistoryLog/LeaveDungeon_2.inc"
}

// b28d-65 [N4/N102]：残余仅 m_traderCharac[0] add-vs-disp 1 条（同 TradeBegin 判词）；
//   ORIG 0x08684e3c，78=77+1 封顶。
// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
#include "cUserHistoryLog/CreatureItemAdd.inc"
}

#include "cUserHistoryLog/InitSkill.inc"
}

// ============================================================================
// cUserHistoryLog::SkillSPInfoSave @ 0x08686188
//   SkillSPInfoSave(CUser*, eSkillSPInfoSaveReason)
//   经 CSkillChanger::GetCurCharacTotalSP 计算总 SP；按当前技能树索引（-1 单树 /
//   其它双树）记录 "SkillSPInfo,%d,%d,%d,%d"（reason, remainSP, totalSP, treeKind）。
// ============================================================================
#include "cUserHistoryLog/SkillSPInfoSave.inc"
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
}

// ============================================================================
// cUserHistoryLog::BillingFail @ 0x08685838
//   BillingFail(ENUM_BILLING_TYPE, unsigned long long, int, int, char*)
//   "BILLING, Fail,%d,%s,%d,%d,%s"（type, tranNo 串, amount, code, reason）。
// ============================================================================
#include "cUserHistoryLog/BillingFail.inc"
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
}

#include "cUserHistoryLog/SkillAdd.inc"
}

#include "cUserHistoryLog/SkillDel.inc"
}

#include "cUserHistoryLog/SPAdd.inc"
}

#include "cUserHistoryLog/SPSub.inc"
}

#include "cUserHistoryLog/SFPAdd.inc"
}

#include "cUserHistoryLog/SFPSub.inc"
}

#include "cUserHistoryLog/FPAdd.inc"
}

#include "cUserHistoryLog/FPSub.inc"
}

// WPAdd(int, int, eWPAddReason)：ORIG _ZN15cUserHistoryLog5WPAddEii12eWPAddReason @0x08682c96。
// 记录 "WP+,%d,%d,%d"（winPoint 当前值, 增加量, reason）。
#include "cUserHistoryLog/WPAdd.inc"
}

#include "cUserHistoryLog/ChangeGrowType.inc"
}

#include "cUserHistoryLog/ClearUsedQP.inc"
}

// QPAdd(int, int, int)：ORIG _ZN15cUserHistoryLog5QPAddEiii @0x0868888c。
// 记录 "QP+,%d,%d,%d"（before, questIdx, add）。
#include "cUserHistoryLog/QPAdd.inc"
}

// SendMail：ORIG 0x86847ea。字段已具名（PacketTypes.h MSG_MAILBOX_SEND，
// m_pad 即 MSG_BASE 后的 +0x0d 起原字节区）。条件为 mail 非空 且 收件人名非空。
#include "cUserHistoryLog/SendMail.inc"
}

#include "cUserHistoryLog/MoveArea.inc"
}

// Logout：ORIG 0x86840c4。"IP-,%s,%d,%d,%d,%d,%d,%p"，
// 参数顺序：characName, a3, a4, (int)a6(uchar), server_group(CEnvironment), a5, this指针。
#include "cUserHistoryLog/Logout.inc"
}

// RecvMail：ORIG 0x86848a8。"MailR,%d,\"%s\",\"%s\""
#include "cUserHistoryLog/RecvMail.inc"
}

// ChangeItemLimitCount：ORIG 0x8686d54。m_user 非空才记录，
// "ChangeItemTradeLimitCount,%d,%d,%d,%d,%d"。
#include "cUserHistoryLog/ChangeItemLimitCount.inc"
}

// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] m_traderCharac[0] 数组退化 add-vs-disp
// 单条已复现 ⇒ 本函数 AE。本文件交易中判族共同判词见下。
#include "cUserHistoryLog/TradeBegin.inc"
}

// b28d-65 [N4/N102]：残余仅 m_traderCharac[0] 数组退化 add-vs-disp 1 条（同 TradeBegin
//   判词，七形探针封顶）；ORIG 0x0868507c，53=53。
// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
#include "cUserHistoryLog/TradeEnd.inc"
}

#include "cUserHistoryLog/TradeItemAddFail.inc"
}

#include "cUserHistoryLog/AchievementComplete.inc"
}

#include "cUserHistoryLog/pvpMissionAdd.inc"
}

#include "cUserHistoryLog/pvpMissionDel.inc"
}

#include "cUserHistoryLog/RedeemItemAdd.inc"
}

#include "cUserHistoryLog/ItemDelCargo.inc"
}
// DungeonClearInfo(int, long)：ORIG 0x8684ac4，经 CUser::LogHistory 记录
#include "cUserHistoryLog/DungeonClearInfo.inc"
}

// DungeonClearInfo(int,long,int,int,int,ENUM_DUNGEON_MODE,char const*,int)：ORIG 0x8684a6e
#include "cUserHistoryLog/DungeonClearInfo_1.inc"
}

// apply_Item(int,int,eApplyItemReason)：ORIG 0x8686c7c。
// m_user 非空才记录，"ApplyItem ,%d,%d,%d"（注意逗号后有空格，与 ORIG 字符串一致）。
#include "cUserHistoryLog/apply_Item.inc"
}

// UseSelectBooster(unsigned int,int,int,std::vector<Inven_Item>&)：ORIG 0x8686b52。
// 把 items 中每个元素 +0x02（Inven_Item::m_addInfo）拼成 "%d," 串（上限 0x400 字节缓冲，
// 每项至少占 0xd 字节），经 CUser::LogHistory 记录 "UseSelectBooster,%u,%d,%d,%s"。
// b28d-65：while(i<size)+体内 `if(strlen+0xd>0x3fe) break` + sprintf 目标内联（去 p 槽）
//   ⇒ 与 ORIG 0x08686b52 逐条全等（60=60）。
#include "cUserHistoryLog/UseSelectBooster.inc"
}

// ============================================================================
// cUserHistoryLog::UpgradeItem @ 0x8684ca0
//   记录强化日志 "Upgrade+,%d,%d,%d" / "Upgrade-,%d,%d,%d"。
// ============================================================================
#include "cUserHistoryLog/UpgradeItem.inc"
}

// ============================================================================
// cUserHistoryLog::SeparateUpgradeItem @ 0x8684d0a
//   记录分离强化日志 "SeparateUpgrade+,%d,%d,%d" / "-..."。
// ============================================================================
#include "cUserHistoryLog/SeparateUpgradeItem.inc"
}

// ============================================================================
// cUserHistoryLog::ExpandInventory @ 0x08683cbc
//   记录背包扩容日志 "ExpandInventory+,%d,%d,%d"（cap, newCap, a）。
//   经 CUser::LogHistory 输出（与其余日志方法同构）。
// ============================================================================
#include "cUserHistoryLog/ExpandInventory.inc"
}

// ============================================================================
// cUserHistoryLog::MapClear @ 0x08684b22（_ZN15cUserHistoryLog8MapClearEtii）
//   记录副本通关物品日志 "MapClear,%d,%d,%d"。
//   调用方：WongWork::CBossStage::handleMoveMap（逐有效成员）。
// ============================================================================
#include "cUserHistoryLog/MapClear.inc"
}

// ============================================================================
// cUserHistoryLog::KillMonster @ 0x086851ac
//   (_ZN15cUserHistoryLog11KillMonsterEijjttRK15MSG_MONSTER_DIEcbii13eExpAddReason)
//   记录击杀日志 "KillMob,%d,%d,%d, (%d:%d),%d, (%d:%d),%d,%d,%d, (%d:%d),%d,
//   %d,%d,%d,%d,%d,%d,%d"（20 变参，含 MSG_MONSTER_DIE +0xa22..+0xa3d 字段）。
//   调用方：WongWork::CBossStage::handleKillMonster。
// ============================================================================
#include "cUserHistoryLog/KillMonster.inc"
}


// ==================== 私有商店历史（FixPrivateStoreSellerVtable 批次）====================
// 报告 docs/class_func_reports/cUserHistoryLog/{CreatePrivateStore,RemovePrivateStore,
// PrivateStoreItemAdd,PrivateStoreItemDel}.md 全文已读（汇编+反编译 C）。

// ORIG T 0x868455a（报告全文）："Private Store+,%d,%d"
#include "cUserHistoryLog/CreatePrivateStore.inc"
}

// ORIG T 0x8684586（报告全文）："Private Store-,%d,%d"
#include "cUserHistoryLog/RemovePrivateStore.inc"
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
}

// ============================================================================
// 账号金库 / 副本进入 / 角色死亡历史日志（DBHistoryDeps 批次；各报告汇编+C 全文已读）
// ============================================================================

// ORIG T 0x08683cee（ItemAddCargo.md）："Inven+,%d,%d,%d"（itemIdx, count, slotType）。
#include "cUserHistoryLog/ItemAddCargo.inc"
}

// ORIG T 0x086868b4（AccountCargoAddItem.md）：'AccountCargo+,%d,%d,%d,%d'
// ——ORIG 变参首参是 reason（param_5），其后才是 itemIdx/count/slotType。
#include "cUserHistoryLog/AccountCargoAddItem.inc"
}

// ORIG T 0x086868ee（AccountCargoDelItem.md）：与 Add 对应的 "AccountCargo-,..."
// （变参顺序同上：reason 在前）。
#include "cUserHistoryLog/AccountCargoDelItem.inc"
}

// ORIG T 0x08684dfa（CharacDie.md）：'Die,"%s",%d,%d'
// （两个 ushort 零扩展后按 %d 打印，ORIG movzbl/movzwl 口径一致）。
#include "cUserHistoryLog/CharacDie.inc"
}

// ORIG T 0x086849a2（EnterDungeon_086849a2.md）：'DungeonEnter,"%s",%d,%d,%s'
// ——第 3 个 %d 是形参 4（state）、%s 是形参 3（memberNames），按报告交错转写。
#include "cUserHistoryLog/EnterDungeon_1.inc"
}

// ORIG T 0x0868491a（EnterDungeon.md 主报告）：
// 'AdvanceDungeonEnter,%d,%d,%d,%s,%d,%d'——char 形参经 movsbl 符号扩展后打印。
#include "cUserHistoryLog/EnterDungeon_2.inc"
}

// ============================================================================
// ORIG T 0x08685452（Auction.md 全量转写）：按 m_type 0/1/2/3 分派
// LogHistory 格式串（SIG_AUCTION_LOG_MESSAGE_AG 布局见 SigTypes.h）。
// ============================================================================
#include "cUserHistoryLog/Auction.inc"
}

// ============================================================================
// ORIG T 0x08685654（AuctionPoint.md 全量转写）：与 Auction 同型，POINT_* 串。
// ============================================================================
#include "cUserHistoryLog/AuctionPoint.inc"
}

// ============================================================================
// Wave5 仅ORIG 清零批（TaiwanLog）——54 个 ORIG-only 方法补齐。
// 逐函数依据 docs/class_func_reports/cUserHistoryLog/ 报告（汇编+反编译C）
// 与 df_game_r objdump（无报告者已注明地址）转写。
// ============================================================================

// ORIG _ZN15cUserHistoryLog16AntibotPacketLogEN20AntibotPacketLogType1TE @0x08686dd0
#include "cUserHistoryLog/AntibotPacketLog.inc"
}

// ORIG _ZN15cUserHistoryLog15BuyCashShopItemEjmjPKccN19CeraShopPaymentType1TEy
//      @0x08686ea0：价格 u64 经 NumberToString 渲染后入参。
#include "cUserHistoryLog/BuyCashShopItem.inc"
}

// ORIG _ZN15cUserHistoryLog12CashInvenAddE19eCashInvenAddReasonjjm @0x0868683a
#include "cUserHistoryLog/CashInvenAdd.inc"
}

// ORIG _ZN15cUserHistoryLog12CashInvenDelE19eCashInvenDelReasonjjmj @0x08686874
#include "cUserHistoryLog/CashInvenDel.inc"
}

// ORIG _ZN15cUserHistoryLog9CharacAddEi @0x08682a92：
//   栈上 TimeLog_ 计时后读 m_charac[idx].m_field26(+0x26 growtype) 日志 "Charac+,%d"。
//   【推断】TimeLog_ 为纯计时局部对象，无可观察副作用，重建中省略。
#include "cUserHistoryLog/CharacAdd.inc"
}

// ORIG _ZN15cUserHistoryLog9CharacDelEi @0x08682ae0（与 CharacAdd 同型，"Charac-,"）
#include "cUserHistoryLog/CharacDel.inc"
}

// ORIG _ZN15cUserHistoryLog6DiePvPE23ENUM_PVP_HISTORY_REASONjjiiiiPKc @0x08686490
#include "cUserHistoryLog/DiePvP.inc"
}

// ORIG _ZN15cUserHistoryLog23Enchant3rdChronicleItemEbiiii @0x08684c28
#include "cUserHistoryLog/Enchant3rdChronicleItem.inc"
}

// ORIG _ZN15cUserHistoryLog11EnchantItemEiiibPKcS1_ @0x08684d74
#include "cUserHistoryLog/EnchantItem.inc"
}

// ORIG _ZN15cUserHistoryLog6EndPvPE23ENUM_PVP_HISTORY_REASONbj @0x0868642e
#include "cUserHistoryLog/EndPvP.inc"
}

// ORIG _ZN15cUserHistoryLog10EndRidableENS_12eRidableTypeEi @0x0868695c：注意参数换位
#include "cUserHistoryLog/EndRidable.inc"
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
}

// ORIG _ZN15cUserHistoryLog16FullIPCounterLogEiiiiiPc @0x08685d54
#include "cUserHistoryLog/FullIPCounterLog.inc"
}

// ---- GuildCargo 族（全部为 "GuildCargo,%d..." 前缀透传；ORIG 地址见各注释） ----
#include "cUserHistoryLog/GuildCargo.inc"
}
#include "cUserHistoryLog/GuildCargoResult.inc"
}
#include "cUserHistoryLog/GuildCargoHistory.inc"
}
#include "cUserHistoryLog/GuildCargoHistoryResult.inc"
}
#include "cUserHistoryLog/GuildCargoCheckPushItem.inc"
}
#include "cUserHistoryLog/GuildCargoCheckPushItemResult.inc"
}
#include "cUserHistoryLog/GuildCargoPushItem.inc"
}
#include "cUserHistoryLog/GuildCargoItemRecovery.inc"
}
#include "cUserHistoryLog/GuildCargoPopItem.inc"
}
#include "cUserHistoryLog/GuildCargoPopItemResult.inc"
}
#include "cUserHistoryLog/GuildCargoPushItemResult.inc"
}
#include "cUserHistoryLog/GuildCargoMoveItem.inc"
}
#include "cUserHistoryLog/GuildCargoMoveItemResult.inc"
}

// ---- 公会日期族：ORIG 取 GlobalData::s_systemTime_ 当前秒 + localtime_r，
//      "%d/%d/%d" = tm_year+1900(tm_year+0x76c)/tm_mon+1/tm_mday ----
// ORIG _ZN15cUserHistoryLog11GuildCreateEjPKcj @0x08685978
#include "cUserHistoryLog/GuildCreate.inc"
}

// ORIG _ZN15cUserHistoryLog9GuildJoinEjPKcjtc @0x08685a48
// b28d-65：三具名日期局部（year/month/day，P0 槽往返）⇒ 53=53 逐条全等（ORIG 0x08685a48）。
#include "cUserHistoryLog/GuildJoin.inc"
}

// ORIG _ZN15cUserHistoryLog8GuildOutEjPKcjct @0x08685b00
// b28d-65：同 GuildJoin（三具名日期局部）⇒ 53=53（ORIG 0x08685b00）。
#include "cUserHistoryLog/GuildOut.inc"
}

// ORIG _ZN15cUserHistoryLog16GuildChangeGradeEjPKcjjhh @0x08685c4e
// b28d-65：同 GuildJoin（三具名日期局部）⇒ 55=55（ORIG 0x08685c4e）。
#include "cUserHistoryLog/GuildChangeGrade.inc"
}

#include "cUserHistoryLog/useEmblem.inc"
}

// ORIG _ZN15cUserHistoryLog12StartRidableENS_12eRidableTypeEi @0x08686928：参数换位同 EndRidable
#include "cUserHistoryLog/StartRidable.inc"
}

// ORIG _ZN15cUserHistoryLog11HackDetectedEjjjjjj @0x08685320：按尾参是否为零递增格式长度
#include "cUserHistoryLog/HackDetected.inc"
}

// ORIG _ZN15cUserHistoryLog15HadesHistoryLogEjjjj @0x08685418
#include "cUserHistoryLog/HadesHistoryLog.inc"
}

// ORIG _ZN15cUserHistoryLog12IPCounterLogEiiiiiPc @0x08685d0c
#include "cUserHistoryLog/IPCounterLog.inc"
}

// ORIG _ZN15cUserHistoryLog21IPCounterLog_InsertIpEiiPc @0x08685d9c
#include "cUserHistoryLog/IPCounterLog_InsertIp.inc"
}

// ORIG _ZN15cUserHistoryLog18ItemMakingSkillAddE21ENUM_ITEM_MAKING_TYPEiis @0x08686a88
#include "cUserHistoryLog/ItemMakingSkillAdd.inc"
}

// ORIG _ZN15cUserHistoryLog20MercenaryCompetitionEjcc @0x08686aca
#include "cUserHistoryLog/MercenaryCompetition.inc"
}

// ORIG _ZN15cUserHistoryLog15MercenaryReturnEjiiiii @0x08686b0a（注意 "MecenaryReturn" 拼写照抄 ORIG）
#include "cUserHistoryLog/MercenaryReturn.inc"
}

// ORIG _ZN15cUserHistoryLog5QPSubEiii @0x0868885a
#include "cUserHistoryLog/QPSub.inc"
}

// ORIG _ZN15cUserHistoryLog11SecuServiceE22ENUM_SECU_SERVICE_TYPE24ENUM_SECU_SERVICE_ACTION
//      @0x0868479c：仅 type 0/1 有日志串。
#include "cUserHistoryLog/SecuService.inc"
}

// ORIG _ZN15cUserHistoryLog27SelectCubeForPremiumServiceEi @0x08686da0
#include "cUserHistoryLog/SelectCubeForPremiumService.inc"
}

// ORIG _ZN15cUserHistoryLog21SendBuffertoClientLogEishb @0x08686e3a
#include "cUserHistoryLog/SendBuffertoClientLog.inc"
}

// ORIG _ZN15cUserHistoryLog23SendMailForCeraShopGiftEjcjj @0x086848da
#include "cUserHistoryLog/SendMailForCeraShopGift.inc"
}

// ORIG _ZN15cUserHistoryLog8StartPvPE23ENUM_PVP_HISTORY_REASONPKcS2_S2_S2_S2_S2_S2_S2_
//      @0x086863b6：两条 PVP 行（前 4 名/后 4 名角色名）。
#include "cUserHistoryLog/StartPvP.inc"
}

// ORIG _ZN15cUserHistoryLog21UseAvatarOptionChangeEiiii @0x08686d0e
#include "cUserHistoryLog/UseAvatarOptionChange.inc"
}

// ORIG _ZN15cUserHistoryLog6UseDyeEiiiRKSsi @0x08686cba
#include "cUserHistoryLog/UseDye.inc"
}

// ---- 物品网格坐标视图：ORIG 存于 this+0x40/+0x42（类尾部 m_pad 区）， ----
// ---- 此前 static helper 直读偏移，现改用具名成员 m_itemX/m_itemY。     ----

// ORIG W _ZN15cUserHistoryLog11getItemXPosEv @0x08696000：
//   mov 0x8(%ebp),%eax; movzwl 0x40(%eax),%eax（单 disp 折叠）——具名成员
//   m_itemX（CUser.h:2153 +0x40）直读。
#include "cUserHistoryLog/getItemXPos.inc"
}

// ORIG W _ZN15cUserHistoryLog11getItemYPosEv @0x0869600c：movzwl 0x42(%eax)。
#include "cUserHistoryLog/getItemYPos.inc"
}

// ORIG W _ZN15cUserHistoryLog12SetItemXYPosEtt @0x0822f084：short 形参 spill
// 槽（-0x4/-0x8）+ movzwl 回读 + `mov %dx,0x40(%eax)` disp 折叠写。
#include "cUserHistoryLog/SetItemXYPos.inc"
}

// ORIG _ZN15cUserHistoryLog13QuestCompleteEi @0x0868514e（格式串 ORIG .rodata 0x8cf0957）
#include "cUserHistoryLog/QuestComplete.inc"
}

// ORIG _ZN15cUserHistoryLog13RedeemItemDelEii @0x086869bc（格式串 ORIG 0x8cf0e9d）
#include "cUserHistoryLog/RedeemItemDel.inc"
}

// ORIG _ZN15cUserHistoryLog16ItemLockUsingLogEhiiii @0x08689808（格式串 ORIG 0x8cf1153）
#include "cUserHistoryLog/ItemLockUsingLog.inc"
}

// ORIG _ZN15cUserHistoryLog9CeraPointE23ENUM_CERAPOINT_LOG_TYPEj @0x0868593c：
//   尾参为当前余额 CUser::GetCeraPoint()。
// [b28d-solo-cUserHistoryLog] IDENTICAL 19/19：第三实参**去 (int) cast** 直传 unsigned
// （GetCeraPoint 返回 unsigned int，vararg %d 位模式不变、语义等价）⇒ 4.4 复现 ORIG
// 「balance 留 %eax / type 先入 %ecx 跨 receiver 保活 / receiver 用 %edx」装载序
// （探针 /tmp/hlprobe/Game/probe3.cpp fB 与 ORIG 逐条同形；旧 fA 带 cast 多 mov %eax,%ecx）。
#include "cUserHistoryLog/CeraPoint.inc"
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
}
