#define _GLIBCXX_IOSTREAM 1
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


#include "cUserHistoryLog/cUserHistoryLog.inc"


#include "cUserHistoryLog/SetUser.inc"


#include "cUserHistoryLog/GiveupPvP.inc"


#include "cUserHistoryLog/seal_Item.inc"


#include "cUserHistoryLog/DeleteInvaildItem.inc"


#include "cUserHistoryLog/SetTrader.inc"


#include "cUserHistoryLog/AvatarItemAdd.inc"


#include "cUserHistoryLog/ItemAdd.inc"


#include "cUserHistoryLog/ItemDel.inc"


#include "cUserHistoryLog/AvatarItemDel.inc"


#include "cUserHistoryLog/CreatureItemDel.inc"


#include "cUserHistoryLog/MoneyAddTrade.inc"


#include "cUserHistoryLog/MoneySubTrade.inc"


#include "cUserHistoryLog/MoneyAdd.inc"


#include "cUserHistoryLog/MoneySub.inc"


#include "cUserHistoryLog/WPSub.inc"
{
    m_user->LogHistory("WP-,%d,%d,%d", cur, sub, static_cast<int>(reason));
}

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

#include "cUserHistoryLog/RequestCleanPad.inc"


#include "cUserHistoryLog/CoinAdd.inc"


#include "cUserHistoryLog/PayCoinAdd.inc"


#include "cUserHistoryLog/EventCoinAdd.inc"


#include "cUserHistoryLog/LevelUp.inc"


#include "cUserHistoryLog/LevelDown.inc"


#include "cUserHistoryLog/CoinSub.inc"


#include "cUserHistoryLog/EventCoinSub.inc"


#include "cUserHistoryLog/PayCoinSub.inc"


#include "cUserHistoryLog/pvpMissionClearReward.inc"


#include "cUserHistoryLog/EnterDungeon.inc"


#include "cUserHistoryLog/LeaveDungeon.inc"


#include "cUserHistoryLog/LeaveDungeon_1.inc"


#include "cUserHistoryLog/LeaveDungeon_2.inc"


#include "cUserHistoryLog/CreatureItemAdd.inc"


#include "cUserHistoryLog/InitSkill.inc"


#include "cUserHistoryLog/SkillSPInfoSave.inc"


#include "cUserHistoryLog/BillingSuccess.inc"


#include "cUserHistoryLog/BillingFail.inc"


#include "cUserHistoryLog/SkillSave.inc"


#include "cUserHistoryLog/SkillAdd.inc"


#include "cUserHistoryLog/SkillDel.inc"


#include "cUserHistoryLog/SPAdd.inc"


#include "cUserHistoryLog/SPSub.inc"


#include "cUserHistoryLog/SFPAdd.inc"


#include "cUserHistoryLog/SFPSub.inc"


#include "cUserHistoryLog/FPAdd.inc"


#include "cUserHistoryLog/FPSub.inc"


#include "cUserHistoryLog/WPAdd.inc"


#include "cUserHistoryLog/ChangeGrowType.inc"


#include "cUserHistoryLog/ClearUsedQP.inc"


#include "cUserHistoryLog/QPAdd.inc"


#include "cUserHistoryLog/SendMail.inc"


#include "cUserHistoryLog/MoveArea.inc"


#include "cUserHistoryLog/Logout.inc"


#include "cUserHistoryLog/RecvMail.inc"


#include "cUserHistoryLog/ChangeItemLimitCount.inc"


#include "cUserHistoryLog/TradeBegin.inc"


#include "cUserHistoryLog/TradeEnd.inc"


#include "cUserHistoryLog/TradeItemAddFail.inc"


#include "cUserHistoryLog/AchievementComplete.inc"


#include "cUserHistoryLog/pvpMissionAdd.inc"


#include "cUserHistoryLog/pvpMissionDel.inc"


#include "cUserHistoryLog/RedeemItemAdd.inc"


#include "cUserHistoryLog/ItemDelCargo.inc"

#include "cUserHistoryLog/DungeonClearInfo.inc"


#include "cUserHistoryLog/DungeonClearInfo_1.inc"


#include "cUserHistoryLog/apply_Item.inc"


#include "cUserHistoryLog/UseSelectBooster.inc"


#include "cUserHistoryLog/UpgradeItem.inc"


#include "cUserHistoryLog/SeparateUpgradeItem.inc"


#include "cUserHistoryLog/ExpandInventory.inc"


#include "cUserHistoryLog/MapClear.inc"


#include "cUserHistoryLog/KillMonster.inc"



// ==================== 私有商店历史（FixPrivateStoreSellerVtable 批次）====================
// 报告 docs/class_func_reports/cUserHistoryLog/{CreatePrivateStore,RemovePrivateStore,
// PrivateStoreItemAdd,PrivateStoreItemDel}.md 全文已读（汇编+反编译 C）。

#include "cUserHistoryLog/CreatePrivateStore.inc"


#include "cUserHistoryLog/RemovePrivateStore.inc"


#include "cUserHistoryLog/PrivateStoreItemAdd.inc"


#include "cUserHistoryLog/PrivateStoreItemDel.inc"


// ============================================================================
// 账号金库 / 副本进入 / 角色死亡历史日志（DBHistoryDeps 批次；各报告汇编+C 全文已读）
// ============================================================================

#include "cUserHistoryLog/ItemAddCargo.inc"


#include "cUserHistoryLog/AccountCargoAddItem.inc"


#include "cUserHistoryLog/AccountCargoDelItem.inc"


#include "cUserHistoryLog/CharacDie.inc"


#include "cUserHistoryLog/EnterDungeon_1.inc"


#include "cUserHistoryLog/EnterDungeon_2.inc"


#include "cUserHistoryLog/Auction.inc"


#include "cUserHistoryLog/AuctionPoint.inc"


// ============================================================================
// Wave5 仅ORIG 清零批（TaiwanLog）——54 个 ORIG-only 方法补齐。
// 逐函数依据 docs/class_func_reports/cUserHistoryLog/ 报告（汇编+反编译C）
// 与 df_game_r objdump（无报告者已注明地址）转写。
// ============================================================================

#include "cUserHistoryLog/AntibotPacketLog.inc"


#include "cUserHistoryLog/BuyCashShopItem.inc"


#include "cUserHistoryLog/CashInvenAdd.inc"


#include "cUserHistoryLog/CashInvenDel.inc"


#include "cUserHistoryLog/CharacAdd.inc"


#include "cUserHistoryLog/CharacDel.inc"


#include "cUserHistoryLog/DiePvP.inc"


#include "cUserHistoryLog/Enchant3rdChronicleItem.inc"


#include "cUserHistoryLog/EnchantItem.inc"


#include "cUserHistoryLog/EndPvP.inc"


#include "cUserHistoryLog/EndRidable.inc"


#include "cUserHistoryLog/ExpAdd.inc"


#include "cUserHistoryLog/FullIPCounterLog.inc"


#include "cUserHistoryLog/GuildCargo.inc"

#include "cUserHistoryLog/GuildCargoResult.inc"

#include "cUserHistoryLog/GuildCargoHistory.inc"

#include "cUserHistoryLog/GuildCargoHistoryResult.inc"

#include "cUserHistoryLog/GuildCargoCheckPushItem.inc"

#include "cUserHistoryLog/GuildCargoCheckPushItemResult.inc"

#include "cUserHistoryLog/GuildCargoPushItem.inc"

#include "cUserHistoryLog/GuildCargoItemRecovery.inc"

#include "cUserHistoryLog/GuildCargoPopItem.inc"

#include "cUserHistoryLog/GuildCargoPopItemResult.inc"

#include "cUserHistoryLog/GuildCargoPushItemResult.inc"

#include "cUserHistoryLog/GuildCargoMoveItem.inc"

#include "cUserHistoryLog/GuildCargoMoveItemResult.inc"
                                               int a, int b, int c)               // 0x08686800
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d", (int)reason, a, b, c);
}

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


#include "cUserHistoryLog/StartRidable.inc"


#include "cUserHistoryLog/HackDetected.inc"


#include "cUserHistoryLog/HadesHistoryLog.inc"


#include "cUserHistoryLog/IPCounterLog.inc"


#include "cUserHistoryLog/IPCounterLog_InsertIp.inc"


#include "cUserHistoryLog/ItemMakingSkillAdd.inc"


#include "cUserHistoryLog/MercenaryCompetition.inc"


#include "cUserHistoryLog/MercenaryReturn.inc"


#include "cUserHistoryLog/QPSub.inc"


#include "cUserHistoryLog/SecuService.inc"


#include "cUserHistoryLog/SelectCubeForPremiumService.inc"


#include "cUserHistoryLog/SendBuffertoClientLog.inc"


#include "cUserHistoryLog/SendMailForCeraShopGift.inc"


#include "cUserHistoryLog/StartPvP.inc"


#include "cUserHistoryLog/UseAvatarOptionChange.inc"


#include "cUserHistoryLog/UseDye.inc"


// ---- 物品网格坐标视图：ORIG 存于 this+0x40/+0x42（类尾部 m_pad 区）， ----
// ---- 此前 static helper 直读偏移，现改用具名成员 m_itemX/m_itemY。     ----

#include "cUserHistoryLog/getItemXPos.inc"


#include "cUserHistoryLog/getItemYPos.inc"


#include "cUserHistoryLog/SetItemXYPos.inc"


#include "cUserHistoryLog/QuestComplete.inc"


#include "cUserHistoryLog/RedeemItemDel.inc"


#include "cUserHistoryLog/ItemLockUsingLog.inc"


#include "cUserHistoryLog/CeraPoint.inc"


#include "cUserHistoryLog/Login.inc"

