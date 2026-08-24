// 声明以 CUser.h 为准。全部方法按 ORIG 反编译（docs/class_func_reports/cUserHistoryLog/）
// 通过 CUser::LogHistory 输出 uHistory 日志；成员布局见 CUser.h：+0x00 m_user、
// +0x04 m_traderAccount[0x1e]、+0x22 m_traderCharac[0x1e]（首字节当"交易中"标志）、+0x40 m_pad。
#include "CUser.h"
#include "SkillSlot.h"        // CSkillChanger / SkillSlot / _Mastered_skill（SkillSPInfoSave/SkillSave 依赖）
#include "DNFFunctionLib.h"   // NumberToString
#include <cstring>            // strncpy
#include <cstdio>             // sprintf
#include <vector>
#include <utility>
#include "CEnvironment.h"     // G_CEnvironment / get_server_group（Logout）
#include "Secu_HackLogCheckByParty.h"  // MSG_MONSTER_DIE 完整定义（KillMonster）


// ---- 本地辅助（ORIG 依赖 CItemGloballyUniqueIdentifierPrint / getItemXPos / getItemYPos，
//      三者均未在重建源码中还原，故此处在文件内实现等价行为并标「推断」）----
// GUID 十六进制串：ORIG 用 CItemGloballyUniqueIdentifierPrint::operator() 把
// item+0x15 的 0x11 字节 GUID 渲染为大写 hex 字符串（返回指针供 %s）。此处等价复刻。【推断】
static const char* itemGUIDToHex(const Inven_Item& item)
{
    static char s_buf[17 * 2 + 1];
    static const char hex[] = "0123456789ABCDEF";
    const unsigned char* p = (const unsigned char*)&item + 0x15;
    for (int i = 0; i < 17; ++i)
    {
        s_buf[i * 2]     = hex[(p[i] >> 4) & 0xf];
        s_buf[i * 2 + 1] = hex[p[i] & 0xf];
    }
    s_buf[34] = 0;
    return s_buf;
}

// item X/Y 网格坐标：ORIG 从 (m_user + 0x79700) 读取（该地址实为用户自身 cUserHistoryLog
// 子对象，原 getItemXPos/getItemYPos 全局函数体未还原）。此处按报告口径从入参首字节读取
// ushort 作占位，语义待 getItemXPos/getItemYPos 还原后替换。【推断】
static unsigned short getItemXPos(const void* p)
{
    return *(const unsigned short*)p;
}
static unsigned short getItemYPos(const void* p)
{
    return *((const unsigned short*)p + 1);
}
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
        if (*(const char*)((const char*)this + 0x22) != 0)
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
    if (!m_user)
        return;
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&item);
    unsigned int upgrade = item.GetUpgrade();
    unsigned int reseal = item.GetReSealCount();
    if (reason == static_cast<eItemAddReason>(1) ||
        reason == static_cast<eItemAddReason>(2))
    {
        if (m_traderCharac[0] == 0)
            return;
        m_user->LogHistory(
            "Item+,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
            "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
            (int)invenType, item.m_addInfo, a3, a4, (int)reason,
            m_traderCharac, m_traderAccount,
            upgrade & 0xff, (unsigned)item.m_fieldb,
            raw[0x25], raw[0x26], raw[0x27], raw[0x28], raw[0x29], raw[0x2a],
            raw[0x2b], raw[0x2c], raw[0x2d], raw[0x2e], raw[0x2f], raw[0x30],
            raw[0x31], raw[0x32],
            reseal & 0xff, itemGUIDToHex(item));
    }
    else
    {
        // ORIG：getItemXPos/YPos(*(int*)this + 0x79700)；该全局函数未还原，
        // 按现有占位 helper 从指针首部取 ushort。【推断】
        const void* posSrc = *reinterpret_cast<char* const*>(this) + 0x79700;
        unsigned short xPos = getItemXPos(posSrc);
        unsigned short yPos = getItemYPos(posSrc);
        m_user->LogHistory(
            "Item+,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
            (int)invenType, item.m_addInfo, a3, a4, (int)reason,
            xPos & 0xffff, yPos & 0xffff,
            item.m_addInfo2, item.m_fieldd,
            (unsigned)raw[0x11], (unsigned)(*reinterpret_cast<const unsigned short*>(raw + 0x12)),
            upgrade & 0xff, (unsigned)item.m_fieldb,
            raw[0x25], raw[0x26], raw[0x27], raw[0x28], raw[0x29], raw[0x2a],
            raw[0x2b], raw[0x2c], raw[0x2d], raw[0x2e], raw[0x2f], raw[0x30],
            raw[0x31], raw[0x32],
            reseal & 0xff, itemGUIDToHex(item));
    }
}

// ORIG 0x868340a：与 ItemAdd 对应的删除日志（无 XPos/YPos，11 个 %d）；
// 尾部 reason==8 且 ITEM_TYPE==1 时补 "Compound-" 日志。
void cUserHistoryLog::ItemDel(INVEN_TYPE invenType, int a3, int a4,
                              const Inven_Item& item, eItemDelReason reason)
{
    if (!m_user)
        return;
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&item);
    unsigned int upgrade = item.GetUpgrade();
    unsigned int reseal = item.GetReSealCount();
    if (reason == static_cast<eItemDelReason>(1) ||
        reason == static_cast<eItemDelReason>(2))
    {
        if (m_traderCharac[0] != 0)
        {
            m_user->LogHistory(
                "Item-,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
                "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
                (int)invenType, item.m_addInfo, a3, a4, (int)reason,
                m_traderCharac, m_traderAccount,
                upgrade & 0xff, (unsigned)item.m_fieldb,
                raw[0x25], raw[0x26], raw[0x27], raw[0x28], raw[0x29], raw[0x2a],
                raw[0x2b], raw[0x2c], raw[0x2d], raw[0x2e], raw[0x2f], raw[0x30],
                raw[0x31], raw[0x32],
                reseal & 0xff, itemGUIDToHex(item));
        }
    }
    else
    {
        m_user->LogHistory(
            "Item-,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
            (int)invenType, item.m_addInfo, a3, a4, (int)reason,
            item.m_addInfo2, item.m_fieldd,
            (unsigned)raw[0x11], (unsigned)(*reinterpret_cast<const unsigned short*>(raw + 0x12)),
            upgrade & 0xff, (unsigned)item.m_fieldb,
            raw[0x25], raw[0x26], raw[0x27], raw[0x28], raw[0x29], raw[0x2a],
            raw[0x2b], raw[0x2c], raw[0x2d], raw[0x2e], raw[0x2f], raw[0x30],
            raw[0x31], raw[0x32],
            reseal & 0xff, itemGUIDToHex(item));
    }

    // Compound 补丁块（ORIG +0x543）：reason==8 && m_field1(ITEM_TYPE)==1。
    if ((int)reason == 8 && item.m_field1 == 1)
    {
        m_user->LogHistory(
            "Compound-,%u,%d,%d,%d,%d,%d,%d,%d,",
            (unsigned)item.m_addInfo, upgrade & 0xff, item.get_add_info(),
            (unsigned)(item.m_upgradeSep.m_field & 0x1f),
            item.m_fieldd,
            (unsigned)item.m_amp.getAbilityType() & 0xff,
            (unsigned)item.m_amp.getAbilityValue() & 0xffff,
            (unsigned)item.m_amp.GetLock() & 0xff);
    }
}

void cUserHistoryLog::AvatarItemDel(int itemIdx, int avatarUid, const char* agency, eAvatarItemDelReason reason)
{
    if (reason == static_cast<eAvatarItemDelReason>(0) || reason == static_cast<eAvatarItemDelReason>(1)) {
        if (*(const char*)((const char*)this + 0x22) != 0)
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
            if (*(const char*)((const char*)this + 0x22) != 0)
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
    if (*(const char*)((const char*)this + 0x22) != 0)
        m_user->LogHistory("Money+,%d,%d,%d,%s(%s)", money, add, static_cast<int>(reason),
                           m_traderCharac, m_traderAccount);
}

void cUserHistoryLog::MoneySubTrade(int money, int sub, eMoneySubReason reason)
{
    if (*(const char*)((const char*)this + 0x22) != 0)
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
            if (*(const char*)((const char*)this + 0x22) != 0)
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
    const char* p = reinterpret_cast<const char*>(mail);
    if (mail != 0 && (p + 0x11) != 0) {
        char* nstr = NumberToString(a, 0);
        const char* mailBody = p + 0x45;                              // ORIG local：&mail[0x45] → -0x24
        int f3d = *reinterpret_cast<const int*>(p + 0x3d);            // -0x20
        int f39 = *reinterpret_cast<const int*>(p + 0x39);            // -0x1c
        unsigned short f37 = *reinterpret_cast<const unsigned short*>(p + 0x37);
        unsigned char f36 = static_cast<unsigned char>(p[0x36]);
        int f2e = *reinterpret_cast<const int*>(p + 0x2e);
        m_user->LogHistory("MailS,%s,%d,%d,%d,%d,%d,\"%s\",%s",
                           p + 0x11, f2e, static_cast<int>(f36), static_cast<int>(f37),
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
    if (*(const char*)((const char*)this + 0x22) != 0)
        m_user->LogHistory("Trade+,\"%s\",\"%s\"", m_traderCharac, m_traderAccount);
}

void cUserHistoryLog::TradeEnd(eTradeEndReason reason, int a, int b, int c, int d)
{
    if (*(const char*)((const char*)this + 0x22) != 0) {
        int sum1 = a + b;
        int sum2 = c + d;
        char flag = 0;
        if (sum1 * 10 < sum2)
            flag = 1;

        else if (sum2 * 10 < sum1)
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
// GUID 串沿用本文件既有等价实现 itemGUIDToHex（ORIG CItemGloballyUniqueIdentifierPrint
// 对 item+0x15 的渲染，W 0x84ebe58/0x84ebe7c 未单独还原）。
void cUserHistoryLog::PrivateStoreItemAdd(INVEN_TYPE invenType, Inven_Item const& item,
                                          int addNew, int addOld, eItemAddReason reason,
                                          char const* name1, char const* name2)
{
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
        itemGUIDToHex(item));
}

// ORIG T 0x868412a（报告全文）："Item-,..."——注意 %s 只有一个名字参数（param_8）。
void cUserHistoryLog::PrivateStoreItemDel(INVEN_TYPE invenType, Inven_Item const& item,
                                          int delNew, int delOld, eItemDelReason reason,
                                          char const* name1, char const* name2)
{
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
        itemGUIDToHex(item));
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
