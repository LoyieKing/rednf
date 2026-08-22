// 声明以 CUser.h 为准。全部方法按 ORIG 反编译（docs/class_func_reports/cUserHistoryLog/）
// 通过 CUser::LogHistory 输出 uHistory 日志；成员布局见 CUser.h：+0x00 m_user、
// +0x04 m_traderAccount[0x1e]、+0x22 m_traderCharac[0x1e]（首字节当"交易中"标志）、+0x40 m_pad。
#include "CUser.h"
#include "DNFFunctionLib.h"   // NumberToString
#include <cstring>            // strncpy


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

void cUserHistoryLog::ItemAdd(INVEN_TYPE, int, int, const Inven_Item&, eItemAddReason)
{
    // 推断：ORIG 0x8682e84 逻辑复杂（CItemGloballyUniqueIdentifierPrint::operator() 生成 GUID
    // 十六进制串 + Inven_Item 内部 0x25..0x32 共14裸字节 + getItemXPos/YPos 全局），
    // 依赖缺失的 CItemGloballyUniqueIdentifierPrint / getItemXPos / getItemYPos 及
    // Inven_Item 裸字节布局（+0x25..0x32），超出本批次可触及文件范围，无法还原 identical。
    // 语义：reason∈{1,2} 且交易中时输出带 GUID 的长格式，否则输出短格式。此处暂空桩占位。
}

void cUserHistoryLog::ItemDel(INVEN_TYPE, int, int, const Inven_Item&, eItemDelReason)
{
    // 推断：ORIG 0x868340a 类似 ItemAdd（GUID 打印 + 14裸字节 + getItemXPos/YPos），
    // 另含补丁块 reason==8&&type==1 时输出 Compound- 日志。依赖缺失同上，无法 identical。
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

void cUserHistoryLog::ChangeGrowType(int a, int b, int c, int d, int e, eChangeGrowTypeReason reason)
{
    m_user->LogHistory("CP,%d,%d,%d,%d,%d,%d", a, b, c, d, e, static_cast<int>(reason));
}

void cUserHistoryLog::ClearUsedQP(int before, int after, int initCount, int unused)
{
    m_user->LogHistory("ClearQP,%d,%d,%d,%d", before, after, initCount, unused);
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
