// 声明以 CUser.h 为准。全部方法按 ORIG 反编译（docs/class_func_reports/cUserHistoryLog/）
// 通过 CUser::LogHistory 输出 uHistory 日志；成员布局见 CUser.h：+0x00 m_user、
// +0x04 m_traderAccount[0x1e]、+0x22 m_traderCharac[0x1e]（首字节当"交易中"标志）、+0x40 m_pad。
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
cUserHistoryLog::cUserHistoryLog()
{
    m_user = 0;
}

void cUserHistoryLog::SetUser(CUser* user)
{
    m_user = user;
}

// GiveupPvP(reason)：ORIG 0x868646c —— CUser::LogHistory(m_user, "PVP,%d", reason)。
void cUserHistoryLog::GiveupPvP(ENUM_PVP_HISTORY_REASON reason)
{
    m_user->LogHistory("PVP,%d", (int)reason);
}

// ORIG 0x8686c36：m_user 非空时 LogHistory("SealItem ,%d,%d,%d", itemIdx, count, reason)。
void cUserHistoryLog::seal_Item(int itemIdx, short count, eSealItemReason reason)
{
    if (m_user != 0)
        m_user->LogHistory("SealItem ,%d,%d,%d", itemIdx, (int)count, (int)reason);
}

// ORIG 0x08686f1e（DeleteInvaildItem.md）：直接以 m_user 调 LogHistory
// （ORIG 无空指针检查），格式 "DeleteInvaildItem, %s, %d, %d"。
void cUserHistoryLog::DeleteInvaildItem(const std::string& str, int a, int b)
{
    m_user->LogHistory("DeleteInvaildItem, %s, %d, %d", str.c_str(), a, b);
}

// SetTrader(accName, characName)：ORIG 0x8532036，strncpy 到 m_traderCharac(acc) 与 m_traderAccount(charac)
void cUserHistoryLog::SetTrader(const char* accName, const char* characName)
{
    strncpy(m_traderCharac, accName, 0x1e);
    strncpy(m_traderAccount, characName, 0x1e);
}

void cUserHistoryLog::AvatarItemAdd(int itemIdx, int avatarUid, const char* agency, eAvatarItemAddReason reason)
{
    if (reason == static_cast<eAvatarItemAddReason>(1) || reason == static_cast<eAvatarItemAddReason>(3)
        || reason == static_cast<eAvatarItemAddReason>(4)) {
        if (m_traderCharac[0] != 0)
            m_user->LogHistory("Avatar+,%d,%d,%s,%d,\"%s\",\"%s\"", itemIdx, avatarUid, agency,
                               static_cast<int>(reason), m_traderCharac, m_traderAccount);
    } else {
        m_user->LogHistory("Avatar+,%d,%d,\"%s\",%d", itemIdx, avatarUid, agency,
                           static_cast<int>(reason));
    }
}

// ORIG 0x8682e84：reason∈{1,2} 且交易中（m_traderCharac[0]!=0）→ 交易格式；
// 否则常规格式（含 getItemXPos/YPos(m_user+0x79700) 网格坐标【推断】）。
// 14 个 hex 字节 = item+0x25..0x32（ChronicleRandomOptionInfo 区）。
void cUserHistoryLog::ItemAdd(INVEN_TYPE invenType, int a3, int a4,
                              const Inven_Item& item, eItemAddReason reason)
{
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&item);
    if ((int)reason == 1 || (int)reason == 2)
    {
        if (m_traderCharac[0] != 0)
        {
            // ORIG：分支内构造 CItemGloballyUniqueIdentifierPrint，先渲染 GUID 串。
            CItemGloballyUniqueIdentifierPrint print;
            const char* guid = print(
                *(const itemGloballyUniqueIdentifier_t*)(const void*)&item.m_amp.m_pad4[1]);
            unsigned int reseal = item.GetReSealCount();
            unsigned int upgrade = item.GetUpgrade();
            m_user->LogHistory(
                "Item+,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
                "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
                (int)invenType, item.m_addInfo, a3, a4, (int)reason,
                m_traderCharac, m_traderAccount,
                upgrade & 0xff, (unsigned)item.m_fieldb,
                raw[0x25], raw[0x26], raw[0x27], raw[0x28], raw[0x29], raw[0x2a],
                raw[0x2b], raw[0x2c], raw[0x2d], raw[0x2e], raw[0x2f], raw[0x30],
                raw[0x31], raw[0x32],
                reseal & 0xff, guid);
        }
        // ORIG：getItemXPos/YPos(m_user + 0x79700)（history 子对象 +0x40/+0x42 网格坐标）。
        cUserHistoryLog* posSrc = &m_user->m_historyLog;
        unsigned short xPos = posSrc->getItemXPos();
        unsigned short yPos = posSrc->getItemYPos();
        CItemGloballyUniqueIdentifierPrint print;
        const char* guid = print(
            *(const itemGloballyUniqueIdentifier_t*)(const void*)&item.m_amp.m_pad4[1]);
        unsigned int reseal = item.GetReSealCount();
        unsigned int upgrade = item.GetUpgrade();
        m_user->LogHistory(
            "Item+,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
            (int)invenType, item.m_addInfo, a3, a4, (int)reason,
            xPos & 0xffff, yPos & 0xffff,
            item.m_addInfo2, item.m_fieldd,
            (unsigned)item.m_amp.m_abilityType, (unsigned)item.m_amp.m_abilityValue,
            upgrade & 0xff, (unsigned)item.m_fieldb,
            raw[0x25], raw[0x26], raw[0x27], raw[0x28], raw[0x29], raw[0x2a],
            raw[0x2b], raw[0x2c], raw[0x2d], raw[0x2e], raw[0x2f], raw[0x30],
            raw[0x31], raw[0x32],
            reseal & 0xff, guid);
    }
}

// ORIG 0x868340a：与 ItemAdd 对应的删除日志（无 XPos/YPos，11 个 %d）；
// 尾部 reason==8 且 ITEM_TYPE==1 时补 "Compound-" 日志。
void cUserHistoryLog::ItemDel(INVEN_TYPE invenType, int a3, int a4,
                              const Inven_Item& item, eItemDelReason reason)
{
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&item);
    if ((int)reason == 1 || (int)reason == 2)
    {
        if (m_traderCharac[0] != 0)
        {
            // ORIG：分支内构造 CItemGloballyUniqueIdentifierPrint，先渲染 GUID 串。
            CItemGloballyUniqueIdentifierPrint print;
            const char* guid = print(
                *(const itemGloballyUniqueIdentifier_t*)(const void*)&item.m_amp.m_pad4[1]);
            unsigned int reseal = item.GetReSealCount();
            unsigned int upgrade = item.GetUpgrade();
            m_user->LogHistory(
                "Item-,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
                "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
                (int)invenType, item.m_addInfo, a3, a4, (int)reason,
                m_traderCharac, m_traderAccount,
                upgrade & 0xff, (unsigned)item.m_fieldb,
                raw[0x25], raw[0x26], raw[0x27], raw[0x28], raw[0x29], raw[0x2a],
                raw[0x2b], raw[0x2c], raw[0x2d], raw[0x2e], raw[0x2f], raw[0x30],
                raw[0x31], raw[0x32],
                reseal & 0xff, guid);
        }
    }
    else
    {
        CItemGloballyUniqueIdentifierPrint print;
        const char* guid = print(
            *(const itemGloballyUniqueIdentifier_t*)(const void*)&item.m_amp.m_pad4[1]);
        unsigned int reseal = item.GetReSealCount();
        unsigned int upgrade = item.GetUpgrade();
        m_user->LogHistory(
            "Item-,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
            (int)invenType, item.m_addInfo, a3, a4, (int)reason,
            item.m_addInfo2, item.m_fieldd,
            (unsigned)item.m_amp.m_abilityType, (unsigned)item.m_amp.m_abilityValue,
            upgrade & 0xff, (unsigned)item.m_fieldb,
            raw[0x25], raw[0x26], raw[0x27], raw[0x28], raw[0x29], raw[0x2a],
            raw[0x2b], raw[0x2c], raw[0x2d], raw[0x2e], raw[0x2f], raw[0x30],
            raw[0x31], raw[0x32],
            reseal & 0xff, guid);
    }

    // Compound 补丁块（ORIG +0x543）：reason==8 && m_field1(ITEM_TYPE)==1。
    if ((int)reason == 8 && item.m_field1 == 1)
    {
        unsigned int lockV = item.m_amp.GetLock();
        unsigned int valueV = item.m_amp.getAbilityValue();
        unsigned int typeV = item.m_amp.getAbilityType();
        m_user->LogHistory(
            "Compound-,%u,%d,%d,%d,%d,%d,%d,%d,",
            (unsigned)item.m_addInfo, item.GetUpgrade() & 0xff, item.get_add_info(),
            (unsigned)(item.m_upgradeSep.m_field & 0x1f),
            item.m_fieldd,
            typeV & 0xff,
            valueV & 0xffff,
            lockV & 0xff);
    }
}

void cUserHistoryLog::AvatarItemDel(int itemIdx, int avatarUid, const char* agency, eAvatarItemDelReason reason)
{
    if (reason == static_cast<eAvatarItemDelReason>(0) || reason == static_cast<eAvatarItemDelReason>(1)) {
        if (m_traderCharac[0] != 0)
            m_user->LogHistory("Avatar-,%d,%d,%s,%d,\"%s\",\"%s\"", itemIdx, avatarUid, agency,
                               static_cast<int>(reason), m_traderCharac, m_traderAccount);
    } else {
        m_user->LogHistory("Avatar-,%d,%d,%s,%d", itemIdx, avatarUid, agency, static_cast<int>(reason));
    }
}

void cUserHistoryLog::CreatureItemDel(INVEN_TYPE invenType, int itemIdx, int count, int addInfo, int type, eItemDelReason reason)
{
    if (type == 5) {
        if (reason == static_cast<eItemDelReason>(0x14) || reason == static_cast<eItemDelReason>(0x15)) {
            if (m_traderCharac[0] != 0)
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

void cUserHistoryLog::MoneyAddTrade(int money, int add, eMoneyAddReason reason)
{
    if (m_traderCharac[0] != 0)
        m_user->LogHistory("Money+,%d,%d,%d,%s(%s)", money, add, static_cast<int>(reason),
                           m_traderCharac, m_traderAccount);
}

void cUserHistoryLog::MoneySubTrade(int money, int sub, eMoneySubReason reason)
{
    if (m_traderCharac[0] != 0)
        m_user->LogHistory("Money-,%d,%d,%d,%s(%s)", money, sub, static_cast<int>(reason),
                           m_traderCharac, m_traderAccount);
}

void cUserHistoryLog::MoneyAdd(int money, int add, eMoneyAddReason reason)
{
    if (reason == static_cast<eMoneyAddReason>(1) || reason == static_cast<eMoneyAddReason>(2)) {
        MoneyAddTrade(money, add, reason);
    } else {
        m_user->LogHistory("Money+,%d,%d,%d", money, add, static_cast<int>(reason));
    }
}

void cUserHistoryLog::MoneySub(int money, int sub, eMoneySubReason reason)
{
    if (reason == static_cast<eMoneySubReason>(2) || reason == static_cast<eMoneySubReason>(3)) {
        MoneySubTrade(money, sub, reason);
    } else {
        m_user->LogHistory("Money-,%d,%d,%d", money, sub, static_cast<int>(reason));
    }
}

// ORIG T 0x8682cc8（0x32 字节）：LogHistory("WP-,%d,%d,%d", cur, sub, reason)。
void cUserHistoryLog::WPSub(int cur, int sub, eWPSubReason reason)
{
    m_user->LogHistory("WP-,%d,%d,%d", cur, sub, static_cast<int>(reason));
}

// ORIG T 0x8685bb8（0x40 字节）：GuildAddExp,"%d/%d/%d",%d,"%s",%d,%d
// （年/月/日/guildKey/guildDBInfo/characNo/exp）。
void cUserHistoryLog::GuildAddExp(unsigned int guildKey,
                                  const char* guildDBInfo,
                                  unsigned int characNo, int exp)
{
    const time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    localtime_r(&now, &local);
    m_user->LogHistory(
        "GuildAddExp,\"%d/%d/%d\",%d,\"%s\",%d,%d",
        local.tm_year + 1900, local.tm_mon + 1, local.tm_mday,
        guildKey, guildDBInfo, characNo, exp);
}

// ORIG T 0x086853e6（0x32 字节）：LogHistory("CleanPad,%d,%d", a, b)。
void cUserHistoryLog::RequestCleanPad(unsigned int a, char b)
{
    m_user->LogHistory("CleanPad,%d,%d", a, static_cast<int>(b));
}

void cUserHistoryLog::CoinAdd(int coin, int amount, eCoinAddReason reason)
{
    m_user->LogHistory("Coin+,%d,%d,%d", coin, amount, static_cast<int>(reason));
}

void cUserHistoryLog::PayCoinAdd(int coin, int amount, eCoinAddReason reason)
{
    m_user->LogHistory("PCoin+,%d,%d,%d", coin, amount, static_cast<int>(reason));
}

void cUserHistoryLog::EventCoinAdd(int coin, int amount, eCoinAddReason reason)
{
    m_user->LogHistory("ECoin+,%d,%d,%d", coin, amount, static_cast<int>(reason));
}

// LevelUp(int, eLevelUpReason)：ORIG 0x86845b2，"Level+,%d,%d"
void cUserHistoryLog::LevelUp(int level, eLevelUpReason reason)
{
    m_user->LogHistory("Level+,%d,%d", level, static_cast<int>(reason));
}

// LevelDown(int, eLevelDownReason)：ORIG 0x86845de，"Level-,%d,%d"
void cUserHistoryLog::LevelDown(int level, eLevelDownReason reason)
{
    m_user->LogHistory("Level-,%d,%d", level, static_cast<int>(reason));
}

void cUserHistoryLog::CoinSub(int coin, int sub, eCoinSubReason reason)
{
    m_user->LogHistory("Coin-,%d,%d,%d", coin, sub, static_cast<int>(reason));
}

void cUserHistoryLog::EventCoinSub(int coin, int sub, eCoinSubReason reason)
{
    m_user->LogHistory("ECoin-,%d,%d,%d", coin, sub, static_cast<int>(reason));
}

void cUserHistoryLog::PayCoinSub(int coin, int sub, eCoinSubReason reason)
{
    m_user->LogHistory("PCoin-,%d,%d,%d", coin, sub, static_cast<int>(reason));
}

void cUserHistoryLog::pvpMissionClearReward(int expPoint, int exp, int missionKind, int missionIndex)
{
    m_user->LogHistory("PvPMissionReward+,%d,%d,%d,%d", expPoint, exp, missionKind, missionIndex);
}

void cUserHistoryLog::EnterDungeon(const char* dungeonName, int level)
{
    m_user->LogHistory("DungeonEnter,\"%s\",%d", dungeonName, level);
}

void cUserHistoryLog::LeaveDungeon(const char* dungeonName, int level)
{
    m_user->LogHistory("DungeonLeave,\"%s\",%d", dungeonName, level);
}

void cUserHistoryLog::LeaveDungeon(const char* dungeonName, int unk, const char* memberNames, int state)
{
    m_user->LogHistory("DungeonLeave,\"%s\",%d,%d,%s", dungeonName, unk, state, memberNames);
}

void cUserHistoryLog::LeaveDungeon(int dungeonIdx, int unk, const char* memberNames, int state)
{
    m_user->LogHistory("DungeonLeave,%d,%d,%d,%s", dungeonIdx, unk, state, memberNames);
}

void cUserHistoryLog::CreatureItemAdd(INVEN_TYPE invenType, int itemIdx, int count, int addInfo, int type, eItemAddReason reason)
{
    if (type == 5) {
        if (reason == static_cast<eItemAddReason>(0x1a) || reason == static_cast<eItemAddReason>(0x1c)
            || reason == static_cast<eItemAddReason>(0x1d)) {
            if (m_traderCharac[0] != 0)
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

void cUserHistoryLog::InitSkill(int treeKind, int level, int sp, int sfp, eSkillInitReason reason)
{
    m_user->LogHistory("SkillInit,%d,%d,%d,%d,%d", treeKind, level, sp, sfp, static_cast<int>(reason));
}

// ============================================================================
// cUserHistoryLog::SkillSPInfoSave @ 0x08686188
//   SkillSPInfoSave(CUser*, eSkillSPInfoSaveReason)
//   经 CSkillChanger::GetCurCharacTotalSP 计算总 SP；按当前技能树索引（-1 单树 /
//   其它双树）记录 "SkillSPInfo,%d,%d,%d,%d"（reason, remainSP, totalSP, treeKind）。
// ============================================================================
void cUserHistoryLog::SkillSPInfoSave(CUser* user, eSkillSPInfoSaveReason reason)
{
    bool isSingleTree = false;
    if (user != 0)
    {
        if (user->GetCurCharacSkillTreeIndex() == -1)
        {
            isSingleTree = true;
        }
    }

    WongWork::CSkillChanger changer;
    int totalSP = changer.GetCurCharacTotalSP(user, false);

    char buf1[128] = {0};
    char buf2[128] = {0};

    if (isSingleTree)
    {
        const SkillSlot* slot = (const SkillSlot*)user->getCurCharacSkillR();
        int remain0 = slot->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0);
        if (totalSP >= 0 && remain0 >= 0)
        {
            sprintf(buf1, "SkillSPInfo,%d,%d,%d,%d", (int)reason, remain0, totalSP, 0xffffffff);
            user->LogHistory(buf1);
        }
    }
    else
    {
        const SkillSlot* slot = (const SkillSlot*)user->getCurCharacSkillR();
        int remain0 = slot->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0);
        int remain1 = slot->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1);
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
// ============================================================================
void cUserHistoryLog::BillingSuccess(ENUM_BILLING_TYPE type,
                                     unsigned long long tranNo, int amount)
{
    char* str = NumberToString(tranNo, 0);
    if (type == 3)
        m_user->LogHistory("BILLING, Success,%d,%s", 3, str);
    else
        m_user->LogHistory("BILLING, Success,%d,%d,%d", (int)type, (int)str, amount);
}

// ============================================================================
// cUserHistoryLog::BillingFail @ 0x08685838
//   BillingFail(ENUM_BILLING_TYPE, unsigned long long, int, int, char*)
//   "BILLING, Fail,%d,%s,%d,%d,%s"（type, tranNo 串, amount, code, reason）。
// ============================================================================
void cUserHistoryLog::BillingFail(ENUM_BILLING_TYPE type,
                                  unsigned long long tranNo, int amount,
                                  int code, char* reason)
{
    char* str = NumberToString(tranNo, 0);
    m_user->LogHistory("BILLING, Fail,%d,%s,%d,%d,%s",
                       (int)type, str, amount, code, reason);
}

// ============================================================================
// cUserHistoryLog::SkillSave @ 0x08685dce
//   SkillSave(_Mastered_skill const*, unsigned int, int, eSkillSaveReason)
//   收集非空技能（跳过 0x8a..0x95 保留区），按每 15 个一批写入
//   "SaveLethe,%d, %d/%d/%d<,或/>%d/%d…" 的 Lethe 保存日志。
// ============================================================================
void cUserHistoryLog::SkillSave(const _Mastered_skill* skills, unsigned int a, int b,
                                eSkillSaveReason reason)
{
    char header[16];
    sprintf(header, ",%d, ", (int)reason);

    std::vector<std::pair<int, int> > vec;
    int count = 0;
    for (int i = 0; i < 0xcc; i++)
    {
        if (skills[i].m_index != 0 && (i < 0x8a || 0x95 < i))
        {
            vec.push_back(std::make_pair((int)skills[i].m_index, (int)skills[i].m_level));
            count++;
        }
    }

    int lineCount = (int)((float)count / 15.0f) + 1;
    for (int line = 0; line < lineCount; line++)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        memcpy(buf, "SaveLethe", 10);
        strcat(buf, header);

        int start = line * 0xf;
        int end;
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

        char tmp[40];
        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp + 0x10, "%d/%d/%d", a, n, b);
        strcat(buf, tmp + 0x10);

        char chunk[512];
        memset(chunk, 0, sizeof(chunk));
        for (int k = 0; k < n; k++)
        {
            int idx = vec[start + k].first;
            int lvl = vec[start + k].second;
            memset(tmp, 0, sizeof(tmp));
            if (k % 3 == 0)
                sprintf(tmp, ",%d/%d", idx, lvl);
            else
                sprintf(tmp, "/%d/%d", idx, lvl);
            strcat(chunk, tmp);
        }
        strcat(buf, chunk);

        m_user->LogHistory(buf);
    }
}

void cUserHistoryLog::SkillAdd(int job, int treeKind, int skillIdx, int count, eSkillAddReason reason)
{
    m_user->LogHistory("Skill+,%d,%d,%d,%d,%d", job, skillIdx, count, static_cast<int>(reason), treeKind);
}

void cUserHistoryLog::SkillDel(int job, int treeKind, int skillIdx, int count, eSkillDelReason reason)
{
    m_user->LogHistory("Skill-,%d,%d,%d,%d,%d", job, skillIdx, count, static_cast<int>(reason), treeKind);
}

void cUserHistoryLog::SPAdd(int treeKind, int remainSP, int sp, eSPAddReason reason)
{
    m_user->LogHistory("SP+,%d,%d,%d,%d", remainSP, sp, static_cast<int>(reason), treeKind);
}

void cUserHistoryLog::SPSub(int treeKind, int remainSP, int sp, eSPSubReason reason)
{
    m_user->LogHistory("SP-,%d,%d,%d,%d", remainSP, sp, static_cast<int>(reason), treeKind);
}

void cUserHistoryLog::SFPAdd(int treeKind, int remainSFP, int sfp, eSPAddReason reason)
{
    m_user->LogHistory("SFP+,%d,%d,%d,%d", remainSFP, sfp, static_cast<int>(reason), treeKind);
}

void cUserHistoryLog::SFPSub(int treeKind, int remainSFP, int sfp, eSPSubReason reason)
{
    m_user->LogHistory("SFP-,%d,%d,%d,%d", remainSFP, sfp, static_cast<int>(reason), treeKind);
}

void cUserHistoryLog::FPAdd(int a, int b, int c, eFPAddReason reason)
{
    m_user->LogHistory("FP+,%d,%d,%d,%d", a, b, c, static_cast<int>(reason));
}

void cUserHistoryLog::FPSub(int a, int b, int c, eFPSubReason reason)
{
    m_user->LogHistory("FP-,%d,%d,%d,%d", a, b, c, static_cast<int>(reason));
}

// WPAdd(int, int, eWPAddReason)：ORIG _ZN15cUserHistoryLog5WPAddEii12eWPAddReason @0x08682c96。
// 记录 "WP+,%d,%d,%d"（winPoint 当前值, 增加量, reason）。
void cUserHistoryLog::WPAdd(int winPoint, int add, eWPAddReason reason)
{
    m_user->LogHistory("WP+,%d,%d,%d", winPoint, add, static_cast<int>(reason));
}

void cUserHistoryLog::ChangeGrowType(int a, int b, int c, int d, int e, eChangeGrowTypeReason reason)
{
    m_user->LogHistory("CP,%d,%d,%d,%d,%d,%d", a, b, c, d, e, static_cast<int>(reason));
}

void cUserHistoryLog::ClearUsedQP(int before, int after, int initCount, int unused)
{
    m_user->LogHistory("ClearQP,%d,%d,%d,%d", before, after, initCount, unused);
}

// QPAdd(int, int, int)：ORIG _ZN15cUserHistoryLog5QPAddEiii @0x0868888c。
// 记录 "QP+,%d,%d,%d"（before, questIdx, add）。
void cUserHistoryLog::QPAdd(int before, int questIdx, int add)
{
    m_user->LogHistory("QP+,%d,%d,%d", before, questIdx, add);
}

// SendMail：ORIG 0x86847ea。MSG_MAILBOX_SEND 仅前向声明，按 ORIG 反编译的
// 包结构字节偏移访问（+0x11 标题串 / +0x2e 物品 / +0x36..0x37 计数 / +0x39,+0x3d /
// +0x45 附言）。条件为 mail 非空 且 &mail[0x11] 非空。
void cUserHistoryLog::SendMail(MSG_MAILBOX_SEND* mail, unsigned int a)
{
    if (mail != 0 && (mail->m_pad + 4) != 0) {
        char* nstr = NumberToString(a, 0);
        const char* mailBody = mail->m_pad + 0x38;                    // ORIG local：&mail[0x45] → -0x24
        int f3d = *reinterpret_cast<const int*>(&mail->m_pad[0x30]);  // -0x20
        int f39 = *reinterpret_cast<const int*>(&mail->m_pad[0x2c]);  // -0x1c
        unsigned short f37 = *reinterpret_cast<const unsigned short*>(&mail->m_pad[0x2a]);
        unsigned char f36 = static_cast<unsigned char>(mail->m_pad[0x29]);
        int f2e = *reinterpret_cast<const int*>(&mail->m_pad[0x21]);
        m_user->LogHistory("MailS,%s,%d,%d,%d,%d,%d,\"%s\",%s",
                           mail->m_pad + 4, f2e, static_cast<int>(f36), static_cast<int>(f37),
                           f39, f3d, mailBody, nstr);
    }
}

void cUserHistoryLog::MoveArea(int a, int b, int c, int d, int e)
{
    m_user->LogHistory("MoveArea,%d,%d,%d,%d", a, b, c, d);
}

// Logout：ORIG 0x86840c4。"IP-,%s,%d,%d,%d,%d,%d,%p"，
// 参数顺序：characName, a3, a4, (int)a6(uchar), server_group(CEnvironment), a5, this指针。
void cUserHistoryLog::Logout(const char* a, const char* b, int c, int d, int e,
                             unsigned char f)
{
    int serverGroup = G_CEnvironment()->get_server_group();
    m_user->LogHistory("IP-,%s,%d,%d,%d,%d,%d,%p",
                       b, c, d, static_cast<int>(f), serverGroup, e,
                       (const void*)this);
}

// RecvMail：ORIG 0x86848a8。"MailR,%d,\"%s\",\"%s\""
void cUserHistoryLog::RecvMail(unsigned int mailIdx, const char* senderName,
                               const char* accName)
{
    m_user->LogHistory("MailR,%d,\"%s\",\"%s\"", mailIdx, senderName, accName);
}

// ChangeItemLimitCount：ORIG 0x8686d54。m_user 非空才记录，
// "ChangeItemTradeLimitCount,%d,%d,%d,%d,%d"。
void cUserHistoryLog::ChangeItemLimitCount(int a, int b, int itemIdx, int d, int e)
{
    if (m_user != 0)
        m_user->LogHistory("ChangeItemTradeLimitCount,%d,%d,%d,%d,%d", a, b, itemIdx, d, e);
}

void cUserHistoryLog::TradeBegin()
{
    if (m_traderCharac[0] != 0)
        m_user->LogHistory("Trade+,\"%s\",\"%s\"", m_traderCharac, m_traderAccount);
}

void cUserHistoryLog::TradeEnd(eTradeEndReason reason, int a, int b, int c, int d)
{
    if (m_traderCharac[0] != 0) {
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

void cUserHistoryLog::TradeItemAddFail(int a, int b)
{
    m_user->LogHistory("Item!,%d,%d", a, b);
}

void cUserHistoryLog::AchievementComplete(int idx)
{
    m_user->LogHistory("AchieveComplete,%d", idx);
}

void cUserHistoryLog::pvpMissionAdd(int a, int b, int c, int d)
{
    m_user->LogHistory("PvPMission+,%d,%d,%d,%d", a, b, c, d);
}

void cUserHistoryLog::pvpMissionDel(int a, int b)
{
    m_user->LogHistory("PvPMission-,%d,%d", a, b);
}

void cUserHistoryLog::RedeemItemAdd(int a, int b)
{
    m_user->LogHistory("RedeemItem+,%d,%d", a, b);
}

void cUserHistoryLog::ItemDelCargo(int a, int b, int c, eItemDelReason reason)
{
    m_user->LogHistory("Inven-,%d,%d,%d,%d", a, b, c, static_cast<int>(reason));
}
// DungeonClearInfo(int, long)：ORIG 0x8684ac4，经 CUser::LogHistory 记录
void cUserHistoryLog::DungeonClearInfo(int isLast, long playTimeSec)
{
    m_user->LogHistory("DungeonClearInfo,%d,%d", isLast, playTimeSec);
}

// DungeonClearInfo(int,long,int,int,int,ENUM_DUNGEON_MODE,char const*,int)：ORIG 0x8684a6e
void cUserHistoryLog::DungeonClearInfo(int isLast, long playTimeSec, int idx, int mode2,
                                       int val2, ENUM_DUNGEON_MODE dungeonMode,
                                       const char* name, int val3)
{
    m_user->LogHistory("AdvanceDungeonClearInfo,%d,%d,%d,%d,%d,%d,%s,%d",
                       val2, idx, isLast, playTimeSec, mode2, static_cast<int>(dungeonMode),
                       name, val3);
}

// apply_Item(int,int,eApplyItemReason)：ORIG 0x8686c7c。
// m_user 非空才记录，"ApplyItem ,%d,%d,%d"（注意逗号后有空格，与 ORIG 字符串一致）。
void cUserHistoryLog::apply_Item(int a, int b, eApplyItemReason reason)
{
    if (m_user)
        m_user->LogHistory("ApplyItem ,%d,%d,%d", a, b, static_cast<int>(reason));
}

// UseSelectBooster(unsigned int,int,int,std::vector<Inven_Item>&)：ORIG 0x8686b52。
// 把 items 中每个元素 +0x02（Inven_Item::m_addInfo）拼成 "%d," 串（上限 0x400 字节缓冲，
// 每项至少占 0xd 字节），经 CUser::LogHistory 记录 "UseSelectBooster,%u,%d,%d,%s"。
void cUserHistoryLog::UseSelectBooster(unsigned int a, int b, int c,
                                       std::vector<Inven_Item>& items)
{
    char buffer[0x400];
    memset(buffer, 0, sizeof(buffer));
    unsigned int i = 0;
    while (i < items.size() && strlen(buffer) + 0xd < 0x3ff)
    {
        char* p = buffer + strlen(buffer);
        sprintf(p, "%d,", items[i].m_addInfo);
        ++i;
    }
    m_user->LogHistory("UseSelectBooster,%u,%d,%d,%s", a, b, c, buffer);
}

// ============================================================================
// cUserHistoryLog::UpgradeItem @ 0x8684ca0
//   记录强化日志 "Upgrade+,%d,%d,%d" / "Upgrade-,%d,%d,%d"。
// ============================================================================
void cUserHistoryLog::UpgradeItem(int itemIdx, int from, int to, bool isUpgrade)
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
void cUserHistoryLog::SeparateUpgradeItem(int itemIdx, int from, int to,
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
void cUserHistoryLog::ExpandInventory(int cap, int newCap, int a)
{
    m_user->LogHistory("ExpandInventory+,%d,%d,%d", cap, newCap, a);
}

// ============================================================================
// cUserHistoryLog::MapClear @ 0x08684b22（_ZN15cUserHistoryLog8MapClearEtii）
//   记录副本通关物品日志 "MapClear,%d,%d,%d"。
//   调用方：WongWork::CBossStage::handleMoveMap（逐有效成员）。
// ============================================================================
void cUserHistoryLog::MapClear(unsigned short itemIdx, int playSec, int a)
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
void cUserHistoryLog::KillMonster(int mobIndex, unsigned int killValue1,
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


// ORIG 逐字节 movzbl 读取 item 偏移（0x25..0x32 落于 ChronicleRandomOptionInfo 内部，
// 无具名字段，按报告字面偏移读取）。
static unsigned int privStoreItemByteAt(Inven_Item const& item, int offset)
{
    return (unsigned int)*reinterpret_cast<unsigned char const*>(
        reinterpret_cast<char const*>(&item) + offset);
}

// ==================== 私有商店历史（FixPrivateStoreSellerVtable 批次）====================
// 报告 docs/class_func_reports/cUserHistoryLog/{CreatePrivateStore,RemovePrivateStore,
// PrivateStoreItemAdd,PrivateStoreItemDel}.md 全文已读（汇编+反编译 C）。

// ORIG T 0x868455a（报告全文）："Private Store+,%d,%d"
void cUserHistoryLog::CreatePrivateStore(int zoneA, int zoneB, char const* name)
{
    m_user->LogHistory("Private Store+,%d,%d", zoneA, zoneB, name);
}

// ORIG T 0x8684586（报告全文）："Private Store-,%d,%d"
void cUserHistoryLog::RemovePrivateStore(int zoneA, int zoneB, char const* name)
{
    m_user->LogHistory("Private Store-,%d,%d", zoneA, zoneB, name);
}

// ORIG T 0x868433e（报告全文）："Item+,..."
// GUID 串：ORIG CItemGloballyUniqueIdentifierPrint 对 item+0x15 的渲染
// （W 0x84ebe58/0x84ebe7c，本文件真实建模）。
void cUserHistoryLog::PrivateStoreItemAdd(INVEN_TYPE invenType, Inven_Item const& item,
                                          int addNew, int addOld, eItemAddReason reason,
                                          char const* name1, char const* name2)
{
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&item);
    CItemGloballyUniqueIdentifierPrint print;
    const char* guid = print(
        *(const itemGloballyUniqueIdentifier_t*)(const void*)&item.m_amp.m_pad4[1]);
    m_user->LogHistory(
        "Item+,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
        (int)invenType, (int)item.m_addInfo, addNew, addOld, (int)reason,
        name1, name2,
        (unsigned)item.GetUpgrade() & 0xff,
        (unsigned)item.m_fieldb,
        privStoreItemByteAt(item, 0x25), privStoreItemByteAt(item, 0x26),
        privStoreItemByteAt(item, 0x27), privStoreItemByteAt(item, 0x28),
        privStoreItemByteAt(item, 0x29), privStoreItemByteAt(item, 0x2a),
        privStoreItemByteAt(item, 0x2b), privStoreItemByteAt(item, 0x2c),
        privStoreItemByteAt(item, 0x2d), privStoreItemByteAt(item, 0x2e),
        privStoreItemByteAt(item, 0x2f), privStoreItemByteAt(item, 0x30),
        privStoreItemByteAt(item, 0x31), privStoreItemByteAt(item, 0x32),
        (unsigned)item.GetReSealCount() & 0xff,
        guid);
}

// ORIG T 0x868412a（报告全文）："Item-,..."——注意 %s 只有一个名字参数（param_8）。
void cUserHistoryLog::PrivateStoreItemDel(INVEN_TYPE invenType, Inven_Item const& item,
                                          int delNew, int delOld, eItemDelReason reason,
                                          char const* name1, char const* name2)
{
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&item);
    CItemGloballyUniqueIdentifierPrint print;
    const char* guid = print(
        *(const itemGloballyUniqueIdentifier_t*)(const void*)&item.m_amp.m_pad4[1]);
    m_user->LogHistory(
        "Item-,%d,%d,%d,%d,%d,\"%s\",%d,%d,"
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
        (int)invenType, (int)item.m_addInfo, delNew, delOld, (int)reason,
        name2,
        (unsigned)item.GetUpgrade() & 0xff,
        (unsigned)item.m_fieldb,
        privStoreItemByteAt(item, 0x25), privStoreItemByteAt(item, 0x26),
        privStoreItemByteAt(item, 0x27), privStoreItemByteAt(item, 0x28),
        privStoreItemByteAt(item, 0x29), privStoreItemByteAt(item, 0x2a),
        privStoreItemByteAt(item, 0x2b), privStoreItemByteAt(item, 0x2c),
        privStoreItemByteAt(item, 0x2d), privStoreItemByteAt(item, 0x2e),
        privStoreItemByteAt(item, 0x2f), privStoreItemByteAt(item, 0x30),
        privStoreItemByteAt(item, 0x31), privStoreItemByteAt(item, 0x32),
        (unsigned)item.GetReSealCount() & 0xff,
        guid);
}

// ============================================================================
// 账号金库 / 副本进入 / 角色死亡历史日志（DBHistoryDeps 批次；各报告汇编+C 全文已读）
// ============================================================================

// ORIG T 0x08683cee（ItemAddCargo.md）："Inven+,%d,%d,%d"（itemIdx, count, slotType）。
void cUserHistoryLog::ItemAddCargo(int itemIdx, int count, int slotType)
{
    m_user->LogHistory("Inven+,%d,%d,%d", itemIdx, count, slotType);
}

// ORIG T 0x086868b4（AccountCargoAddItem.md）：'AccountCargo+,%d,%d,%d,%d'
// ——ORIG 变参首参是 reason（param_5），其后才是 itemIdx/count/slotType。
void cUserHistoryLog::AccountCargoAddItem(int itemIdx, int count, int slotType,
                                          eItemAddReason reason)
{
    m_user->LogHistory("AccountCargo+,%d,%d,%d,%d",
                       static_cast<int>(reason), itemIdx, count, slotType);
}

// ORIG T 0x086868ee（AccountCargoDelItem.md）：与 Add 对应的 "AccountCargo-,..."
// （变参顺序同上：reason 在前）。
void cUserHistoryLog::AccountCargoDelItem(int itemIdx, int count, int slotType,
                                          eItemDelReason reason)
{
    m_user->LogHistory("AccountCargo-,%d,%d,%d,%d",
                       static_cast<int>(reason), itemIdx, count, slotType);
}

// ORIG T 0x08684dfa（CharacDie.md）：'Die,"%s",%d,%d'
// （两个 ushort 零扩展后按 %d 打印，ORIG movzbl/movzwl 口径一致）。
void cUserHistoryLog::CharacDie(char const* name, unsigned short unkD,
                                unsigned short unkF)
{
    m_user->LogHistory("Die,\"%s\",%d,%d", name,
                       static_cast<unsigned>(unkD), static_cast<unsigned>(unkF));
}

// ORIG T 0x086849a2（EnterDungeon_086849a2.md）：'DungeonEnter,"%s",%d,%d,%s'
// ——第 3 个 %d 是形参 4（state）、%s 是形参 3（memberNames），按报告交错转写。
void cUserHistoryLog::EnterDungeon(const char* dungeonName, int unk,
                                   const char* memberNames, int state)
{
    m_user->LogHistory("DungeonEnter,\"%s\",%d,%d,%s",
                       dungeonName, unk, state, memberNames);
}

// ORIG T 0x0868491a（EnterDungeon.md 主报告）：
// 'AdvanceDungeonEnter,%d,%d,%d,%s,%d,%d'——char 形参经 movsbl 符号扩展后打印。
void cUserHistoryLog::EnterDungeon(int dungeonIdx, int a, int b,
                                   const char* memberNames, char flag, int state)
{
    m_user->LogHistory("AdvanceDungeonEnter,%d,%d,%d,%s,%d,%d",
                       dungeonIdx, a, b, memberNames, static_cast<int>(flag), state);
}

// ============================================================================
// ORIG T 0x08685452（Auction.md 全量转写）：按 m_type 0/1/2/3 分派
// LogHistory 格式串（SIG_AUCTION_LOG_MESSAGE_AG 布局见 SigTypes.h）。
// ============================================================================
void cUserHistoryLog::Auction(SIG_AUCTION_LOG_MESSAGE_AG* msg)
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
                msg->m_key, msg->m_arg09, msg->m_arg0d, msg->m_arg11);
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
void cUserHistoryLog::AuctionPoint(SIG_AUCTION_LOG_MESSAGE_PG* msg)
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
void cUserHistoryLog::AntibotPacketLog(AntibotPacketLogType::T type)
{
    m_user->LogHistory("AntibotPacketLog,%d", (int)type);
}

// ORIG _ZN15cUserHistoryLog15BuyCashShopItemEjmjPKccN19CeraShopPaymentType1TEy
//      @0x08686ea0：价格 u64 经 NumberToString 渲染后入参。
void cUserHistoryLog::BuyCashShopItem(unsigned int itemIdx, unsigned long uid,
                                      unsigned int count, const char* buyer, char flag,
                                      CeraShopPaymentType::T payType,
                                      unsigned long long price)
{
    m_user->LogHistory("BuyCashItem, %u, %u, %u, %s, %d, %d, %s",
                       itemIdx, (unsigned int)uid, count, buyer, (int)flag,
                       (int)payType, NumberToString(price, 0));
}

// ORIG _ZN15cUserHistoryLog12CashInvenAddE19eCashInvenAddReasonjjm @0x0868683a
void cUserHistoryLog::CashInvenAdd(eCashInvenAddReason reason, unsigned int a,
                                   unsigned int b, unsigned long c)
{
    m_user->LogHistory("CashInven+,%u,%u,%u,%u",
                       (unsigned)reason, a, b, (unsigned)c);
}

// ORIG _ZN15cUserHistoryLog12CashInvenDelE19eCashInvenDelReasonjjmj @0x08686874
void cUserHistoryLog::CashInvenDel(eCashInvenDelReason reason, unsigned int a,
                                   unsigned int b, unsigned long c, unsigned int d)
{
    m_user->LogHistory("CashInven-,%u,%u,%u,%u,%u",
                       (unsigned)reason, a, b, (unsigned)c, d);
}

// ORIG _ZN15cUserHistoryLog9CharacAddEi @0x08682a92：
//   栈上 TimeLog_ 计时后读 m_charac[idx].m_field26(+0x26 growtype) 日志 "Charac+,%d"。
//   【推断】TimeLog_ 为纯计时局部对象，无可观察副作用，重建中省略。
void cUserHistoryLog::CharacAdd(int idx)
{
    std::vector<_Charac_info>& characs =
        *reinterpret_cast<std::vector<_Charac_info>*>(
            reinterpret_cast<char*>(m_user) + 0x796e8);
    m_user->LogHistory("Charac+,%d", (int)characs[idx].m_field26);
}

// ORIG _ZN15cUserHistoryLog9CharacDelEi @0x08682ae0（与 CharacAdd 同型，"Charac-,"）
void cUserHistoryLog::CharacDel(int idx)
{
    std::vector<_Charac_info>& characs =
        *reinterpret_cast<std::vector<_Charac_info>*>(
            reinterpret_cast<char*>(m_user) + 0x796e8);
    m_user->LogHistory("Charac-,%d", (int)characs[idx].m_field26);
}

// ORIG _ZN15cUserHistoryLog6DiePvPE23ENUM_PVP_HISTORY_REASONjjiiiiPKc @0x08686490
void cUserHistoryLog::DiePvP(ENUM_PVP_HISTORY_REASON reason, unsigned int a, unsigned int b,
                             int c, int d, int e, int f, const char* name)
{
    m_user->LogHistory("PVP,%d,%u,%u,%d,%d,%d,%d,\"%s\"",
                       (int)reason, a, b, c, d, e, f, name);
}

// ORIG _ZN15cUserHistoryLog23Enchant3rdChronicleItemEbiiii @0x08684c28
void cUserHistoryLog::Enchant3rdChronicleItem(bool isEnchant, int itemIdx, int materialIdx,
                                              int equipType, int count)
{
    if (isEnchant)
        m_user->LogHistory("3rdEnchant+,%d,%d,%d,%d", itemIdx, materialIdx, equipType, count);
    else
        m_user->LogHistory("3rdEnchant-,%d,%d,%d,%d", itemIdx, materialIdx, equipType, count);
}

// ORIG _ZN15cUserHistoryLog11EnchantItemEiiibPKcS1_ @0x08684d74
void cUserHistoryLog::EnchantItem(int a, int b, int c, bool up,
                                  const char* name1, const char* name2)
{
    if (up)
        m_user->LogHistory("Enchant+,%d,%d,%d,\"%s\",\"%s\"", a, b, c, name1, name2);
    else
        m_user->LogHistory("Enchant-,%d,%d,%d,\"%s\",\"%s\"", a, b, c, name1, name2);
}

// ORIG _ZN15cUserHistoryLog6EndPvPE23ENUM_PVP_HISTORY_REASONbj @0x0868642e
void cUserHistoryLog::EndPvP(ENUM_PVP_HISTORY_REASON reason, bool flag, unsigned int v)
{
    m_user->LogHistory("PVP,%d,%d,%u", (int)reason, flag ? 1 : 0, v);
}

// ORIG _ZN15cUserHistoryLog10EndRidableENS_12eRidableTypeEi @0x0868695c：注意参数换位
void cUserHistoryLog::EndRidable(eRidableType type, int a)
{
    m_user->LogHistory("Transform,%d,%d,%d", 0, a, (int)type);
}

// ORIG _ZN15cUserHistoryLog6ExpAddEii13eExpAddReasoni @0x08684bd0：
//   仅当 reason>0xc 或 (1<<reason)&0x1892==0 时记录（排除常规来源）；
//   日志第三参数与判断均用 reason（reasonIdx 未参与）。
void cUserHistoryLog::ExpAdd(int exp, int sp, eExpAddReason reason, int reasonIdx)
{
    if ((0xc < (unsigned)reason) ||
        (((1u << ((unsigned char)reason & 0x1f)) & 0x1892U) == 0))
    {
        m_user->LogHistory("Exp+,%d,%d,%d", exp, sp, reason);
    }
}

// ORIG _ZN15cUserHistoryLog16FullIPCounterLogEiiiiiPc @0x08685d54
void cUserHistoryLog::FullIPCounterLog(int a, int b, int c, int d, int e, char* ip)
{
    m_user->LogHistory("D_IPCount,%d,%d,%d,%d,%d,%s", a, b, c, d, e, ip);
}

// ---- GuildCargo 族（全部为 "GuildCargo,%d..." 前缀透传；ORIG 地址见各注释） ----
void cUserHistoryLog::GuildCargo(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a)   // 0x086864e6
{
    m_user->LogHistory("GuildCargo,%d,%d", (int)reason, a);
}
void cUserHistoryLog::GuildCargoResult(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                       int a, int b)                              // 0x08686512
{
    m_user->LogHistory("GuildCargo,%d,%d,%d", (int)reason, a, b);
}
void cUserHistoryLog::GuildCargoHistory(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a) // 0x08686544
{
    m_user->LogHistory("GuildCargo,%d,%d", (int)reason, a);
}
void cUserHistoryLog::GuildCargoHistoryResult(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a) // 0x08686570
{
    m_user->LogHistory("GuildCargo,%d,%d", (int)reason, a);
}
void cUserHistoryLog::GuildCargoCheckPushItem(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                              int a, int b, int c, int d, int e, int f)  // 0x0868659c
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e, f);
}
void cUserHistoryLog::GuildCargoCheckPushItemResult(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                                    int a, int b, int c, int d, int e, int f) // 0x086865ea
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e, f);
}
void cUserHistoryLog::GuildCargoPushItem(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                         int a, int b, int c, int d, int e, int f,
                                         unsigned char g)                         // 0x08686638
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d,%d",
                       (int)reason, a, b, c, d, e, f, g & 0xff);
}
void cUserHistoryLog::GuildCargoItemRecovery(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                             int a, int b, int c, int d, int e,
                                             unsigned char f)                     // 0x08686694
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d",
                       (int)reason, a, b, c, d, e, f & 0xff);
}
void cUserHistoryLog::GuildCargoPopItem(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                        int a, int b, int c, int d, int e)        // 0x086866ea
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e);
}
void cUserHistoryLog::GuildCargoPopItemResult(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                              int a, int b, int c, int d, int e, int f) // 0x08686780
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e, f);
}
void cUserHistoryLog::GuildCargoPushItemResult(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                               int a, int b, int c, int d, int e, int f) // 0x08686732
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e, f);
}
void cUserHistoryLog::GuildCargoMoveItem(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                         int a, int b)                            // 0x086867ce
{
    m_user->LogHistory("GuildCargo,%d,%d,%d", (int)reason, a, b);
}
void cUserHistoryLog::GuildCargoMoveItemResult(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                               int a, int b, int c)               // 0x08686800
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d", (int)reason, a, b, c);
}

// ---- 公会日期族：ORIG 取 GlobalData::s_systemTime_ 当前秒 + localtime_r，
//      "%d/%d/%d" = tm_year+1900(tm_year+0x76c)/tm_mon+1/tm_mday ----
// ORIG _ZN15cUserHistoryLog11GuildCreateEjPKcj @0x08685978
void cUserHistoryLog::GuildCreate(unsigned int guildKey, const char* guildName,
                                  unsigned int characNo)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    localtime_r(&now, &broken);
    m_user->LogHistory("GuildCreate,\"%d/%d/%d\",%d,\"%s\",%d",
                       broken.tm_year + 0x76c, broken.tm_mon + 1, broken.tm_mday,
                       guildKey, guildName, characNo);
}

// ORIG _ZN15cUserHistoryLog9GuildJoinEjPKcjtc @0x08685a48
void cUserHistoryLog::GuildJoin(unsigned int guildKey, const char* guildName,
                                unsigned int characNo, unsigned short a, char b)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    localtime_r(&now, &broken);
    m_user->LogHistory("GuildJoin,\"%d/%d/%d\",%d,\"%s\",%d,%d,%d",
                       broken.tm_year + 0x76c, broken.tm_mon + 1, broken.tm_mday,
                       guildKey, guildName, characNo, (unsigned)a, (int)b);
}

// ORIG _ZN15cUserHistoryLog8GuildOutEjPKcjct @0x08685b00
void cUserHistoryLog::GuildOut(unsigned int guildKey, const char* guildName,
                               unsigned int characNo, char a, unsigned short b)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    localtime_r(&now, &broken);
    m_user->LogHistory("GuildOut,\"%d/%d/%d\",%d,\"%s\",%d,%d,%d",
                       broken.tm_year + 0x76c, broken.tm_mon + 1, broken.tm_mday,
                       guildKey, guildName, characNo, (int)a, (unsigned)b);
}

// ORIG _ZN15cUserHistoryLog16GuildChangeGradeEjPKcjjhh @0x08685c4e
void cUserHistoryLog::GuildChangeGrade(unsigned int guildKey, const char* guildName,
                                       unsigned int characNo, unsigned int grade,
                                       unsigned char oldGrade, unsigned char newGrade)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    localtime_r(&now, &broken);
    m_user->LogHistory("GuildChangeGrade,\"%d/%d/%d\",%d,\"%s\",%d,%d,%d,%d",
                       broken.tm_year + 0x76c, broken.tm_mon + 1, broken.tm_mday,
                       guildKey, guildName, characNo, grade,
                       (unsigned)(oldGrade & 0xff), (unsigned)(newGrade & 0xff));
}

// ORIG _ZN15cUserHistoryLog9useEmblemEmmjPKcj @0x08685a08
void cUserHistoryLog::useEmblem(unsigned long a, unsigned long b, unsigned int c,
                                const char* d, unsigned int e)
{
    m_user->LogHistory("UseEmblem,%d,%d,%d,%s,%d", (int)a, (int)b, c, d, e);
}

// ORIG _ZN15cUserHistoryLog12StartRidableENS_12eRidableTypeEi @0x08686928：参数换位同 EndRidable
void cUserHistoryLog::StartRidable(eRidableType type, int a)
{
    m_user->LogHistory("Transform,%d,%d,%d", 1, a, (int)type);
}

// ORIG _ZN15cUserHistoryLog11HackDetectedEjjjjjj @0x08685320：按尾参是否为零递增格式长度
void cUserHistoryLog::HackDetected(unsigned int a, unsigned int b, unsigned int c,
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
void cUserHistoryLog::HadesHistoryLog(unsigned int a, unsigned int b, unsigned int c,
                                      unsigned int d)
{
    m_user->LogHistory("Hades,%d,%d,%d,%d", a, b, c, d);
}

// ORIG _ZN15cUserHistoryLog12IPCounterLogEiiiiiPc @0x08685d0c
void cUserHistoryLog::IPCounterLog(int a, int b, int c, int d, int e, char* ip)
{
    m_user->LogHistory("IPCount,%d,%d,%d,%d,%d,%s", a, b, c, d, e, ip);
}

// ORIG _ZN15cUserHistoryLog21IPCounterLog_InsertIpEiiPc @0x08685d9c
void cUserHistoryLog::IPCounterLog_InsertIp(int a, int b, char* ip)
{
    m_user->LogHistory("IPCountInt,%d,%d,%s", a, b, ip);
}

// ORIG _ZN15cUserHistoryLog18ItemMakingSkillAddE21ENUM_ITEM_MAKING_TYPEiis @0x08686a88
void cUserHistoryLog::ItemMakingSkillAdd(ENUM_ITEM_MAKING_TYPE type, int a, int b, short c)
{
    m_user->LogHistory("ItemMakingSkill+,%d,%d,%d,%d",
                       (int)type, a, b, (int)c);
}

// ORIG _ZN15cUserHistoryLog20MercenaryCompetitionEjcc @0x08686aca
void cUserHistoryLog::MercenaryCompetition(unsigned int a, char b, char c)
{
    m_user->LogHistory("MercenaryCompetition,%u,%d,%d", a, (int)b, (int)c);
}

// ORIG _ZN15cUserHistoryLog15MercenaryReturnEjiiiii @0x08686b0a（注意 "MecenaryReturn" 拼写照抄 ORIG）
void cUserHistoryLog::MercenaryReturn(unsigned int mercNo, int a, int b, int c, int d, int e)
{
    m_user->LogHistory("MecenaryReturn,%u,%d,%d,%d,%d,%d", mercNo, a, b, c, d, e);
}

// ORIG _ZN15cUserHistoryLog5QPSubEiii @0x0868885a
void cUserHistoryLog::QPSub(int before, int questIdx, int sub)
{
    m_user->LogHistory("QP-,%d,%d,%d", before, questIdx, sub);
}

// ORIG _ZN15cUserHistoryLog11SecuServiceE22ENUM_SECU_SERVICE_TYPE24ENUM_SECU_SERVICE_ACTION
//      @0x0868479c：仅 type 0/1 有日志串。
void cUserHistoryLog::SecuService(ENUM_SECU_SERVICE_TYPE type, ENUM_SECU_SERVICE_ACTION action)
{
    if (type == ENUM_SECU_SERVICE_TYPE_GOBLIN)
        m_user->LogHistory("s_gob,%d", (int)action);
    else if (type == ENUM_SECU_SERVICE_TYPE_CARD)
        m_user->LogHistory("s_card,%d", (int)action);
}

// ORIG _ZN15cUserHistoryLog27SelectCubeForPremiumServiceEi @0x08686da0
void cUserHistoryLog::SelectCubeForPremiumService(int a)
{
    if (m_user != 0)
        m_user->LogHistory("SelectCubeForPremiumService,%d", a);
}

// ORIG _ZN15cUserHistoryLog21SendBuffertoClientLogEishb @0x08686e3a
void cUserHistoryLog::SendBuffertoClientLog(int kind, short a, unsigned char b, bool c)
{
    const char* name = (kind == 0) ? "ANTIBOT" : "DPROTO";
    m_user->LogHistory("SendBuffertoClient [%s],%d,%d,%d",
                       name, (int)a, (unsigned)b, (unsigned)c);
}

// ORIG _ZN15cUserHistoryLog23SendMailForCeraShopGiftEjcjj @0x086848da
void cUserHistoryLog::SendMailForCeraShopGift(unsigned int mailIdx, char flag,
                                              unsigned int a, unsigned int b)
{
    m_user->LogHistory("MailSG,%u,%d,%u,%u", mailIdx, (int)flag, a, b);
}

// ORIG _ZN15cUserHistoryLog8StartPvPE23ENUM_PVP_HISTORY_REASONPKcS2_S2_S2_S2_S2_S2_S2_
//      @0x086863b6：两条 PVP 行（前 4 名/后 4 名角色名）。
void cUserHistoryLog::StartPvP(ENUM_PVP_HISTORY_REASON reason, const char* n1, const char* n2,
                               const char* n3, const char* n4, const char* n5,
                               const char* n6, const char* n7, const char* n8)
{
    m_user->LogHistory("PVP,%d,\"%s\",\"%s\",\"%s\",\"%s\"", (int)reason, n1, n2, n3, n4);
    m_user->LogHistory("PVP,%d,\"%s\",\"%s\",\"%s\",\"%s\"", (int)reason, n5, n6, n7, n8);
}

// ORIG _ZN15cUserHistoryLog21UseAvatarOptionChangeEiiii @0x08686d0e
void cUserHistoryLog::UseAvatarOptionChange(int a, int b, int c, int d)
{
    if (m_user != 0)
        m_user->LogHistory("UseAvatarOptionChange,%d,%d,%d,%d", a, b, c, d);
}

// ORIG _ZN15cUserHistoryLog6UseDyeEiiiRKSsi @0x08686cba
void cUserHistoryLog::UseDye(int a, int b, int c, const std::string& dyeName, int d)
{
    if (m_user != 0)
        m_user->LogHistory("UseDye ,%d,%d,%d,%s,%d", a, b, c, dyeName.c_str(), d);
}

// ---- 物品网格坐标视图：ORIG 存于 this+0x40/+0x42（类尾部 m_pad 区）， ----
// ---- 此前 static helper 直读偏移，现改用具名成员 m_itemX/m_itemY。     ----

// ORIG W _ZN15cUserHistoryLog11getItemXPosEv @0x08696000（movzwl 0x40(%eax)）
unsigned short cUserHistoryLog::getItemXPos()
{
    return *reinterpret_cast<const unsigned short*>(
        reinterpret_cast<const char*>(this) + 0x40);
}

// ORIG W _ZN15cUserHistoryLog11getItemYPosEv @0x0869600c（movzwl 0x42(%eax)）
unsigned short cUserHistoryLog::getItemYPos()
{
    return *reinterpret_cast<const unsigned short*>(
        reinterpret_cast<const char*>(this) + 0x42);
}

// ORIG W _ZN15cUserHistoryLog12SetItemXYPosEtt @0x0822f084（movw → 0x40/0x42）
void cUserHistoryLog::SetItemXYPos(unsigned short x, unsigned short y)
{
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(this) + 0x40) = x;
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(this) + 0x42) = y;
}

// ORIG _ZN15cUserHistoryLog13QuestCompleteEi @0x0868514e（格式串 ORIG .rodata 0x8cf0957）
void cUserHistoryLog::QuestComplete(int questIdx)
{
    m_user->LogHistory("QuestComplete,%d", questIdx);
}

// ORIG _ZN15cUserHistoryLog13RedeemItemDelEii @0x086869bc（格式串 ORIG 0x8cf0e9d）
void cUserHistoryLog::RedeemItemDel(int a, int b)
{
    m_user->LogHistory("RedeemItem-,%d,%d", a, b);
}

// ORIG _ZN15cUserHistoryLog16ItemLockUsingLogEhiiii @0x08689808（格式串 ORIG 0x8cf1153）
void cUserHistoryLog::ItemLockUsingLog(unsigned char lockType, int a, int b, int c, int d)
{
    m_user->LogHistory("ItemLock,%d,%d,%d,%d,%d", lockType & 0xff, a, b, c, d);
}

// ORIG _ZN15cUserHistoryLog9CeraPointE23ENUM_CERAPOINT_LOG_TYPEj @0x0868593c：
//   尾参为当前余额 CUser::GetCeraPoint()。
void cUserHistoryLog::CeraPoint(ENUM_CERAPOINT_LOG_TYPE type, unsigned int amount)
{
    m_user->LogHistory("CeraPoint,%d,%d,%d", (int)type, amount,
                       (int)m_user->GetCeraPoint());
}

// ORIG _ZN15cUserHistoryLog5LoginEPKcS1_iihi @0x08683d86（Login.md 全量转写）：
//   扫描背包 invenType=0 槽 10..0x13 统计强化/稀有件、invenType=2 全槽计数、
//   回归判定 local_60、专家职业等级，输出 "IP+,..." 18 字段行。
void cUserHistoryLog::Login(const char* ip, const char* param2, int param3, int param4,
                            unsigned char loginType, int param6)
{
    int region;                                  // local_60 回归分类
    int upgradeFirst = 0;                        // local_58（首件强化）
    int firstItemId = 0;                         // local_54（10 号槽 itemId）
    int upgradeSum = 0;                          // local_50（其余件强化和）
    int rareCount = 0;                           // local_4c（rarity 0/1 计数）
    int petCount = 0;                            // local_48（invenType=2 计数）

    const CInventory* invenR = m_user->getCurCharacInvenR();  // ORIG 读路径（InvenR）
    int money = reinterpret_cast<CInventory*>(
                    reinterpret_cast<char*>(const_cast<_Charac_info*>(m_user->getCurCharacR()))
                    + 0xda)->get_money();

    for (int slot = 10; slot < 0x14; ++slot)
    {
        const Inven_Item* it = invenR->GetInvenRef(0, slot);
        if (it == 0 || *(const int*)&it->m_addInfo == 0)
            continue;
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
        if (item != 0 && (item->get_rarity() == 0 || item->get_rarity() == 1))
            ++rareCount;
    }
    for (int slot = 0; slot < 0x74; ++slot)
    {
        const Inven_Item* it = invenR->GetInvenRef(2, slot);
        if (it != 0 && *(const int*)&it->m_addInfo != 0)
            ++petCount;
    }

    // +0x7b 处回归用户标记【推断：ORIG *(int*)(charac+0x7b)，该字段未具名】
    const _Charac_info* ch = m_user->getCurCharacR();
    int returnUserFlag = *reinterpret_cast<const int*>(reinterpret_cast<const char*>(ch) + 0x7b);
    if (returnUserFlag == 0 && m_user->GetFirstLogin())
        region = 0;
    else if (loginType == 0 && returnUserFlag != 0)
        region = 1;
    else if (loginType == 1 && returnUserFlag != 0)
        region = 2;
    else if (loginType == 2 && returnUserFlag != 0)
        region = 3;
    else
        region = -1;   // ORIG 未初始化路径保持全零默认 → 与 ORIG 一致的 local_60 初值

    int expertJobLevel = 0;                      // local_3c
    int expertJobType = m_user->GetCurCharacExpertJobType();  // 返回值亦入 %d
    STExpertJobScript* script =
        reinterpret_cast<STExpertJobScript*>(G_CDataManager()->GetExpertJobScript(expertJobType));
    if (script != 0)
        expertJobLevel = script->GetLevel((unsigned)m_user->GetCurCharacExpertJobExp());

    m_user->LogHistory(
        "IP+,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%p",
        param2, param3, param4, (int)loginType, param6,
        m_user->get_charac_job(), m_user->get_charac_level(),
        (int)m_user->getCurCharFirstGrowType(), (int)m_user->getCurCharSecondGrowType(),
        region, money, upgradeFirst, firstItemId, upgradeSum, rareCount, petCount,
        expertJobType, expertJobLevel, reinterpret_cast<void*>(m_user));
}
