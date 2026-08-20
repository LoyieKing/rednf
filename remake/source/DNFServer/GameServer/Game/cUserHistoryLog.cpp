// 声明以 CUser.h 为准。
#include "CUser.h"

void cUserHistoryLog::SetUser(CUser*)
{
}

void cUserHistoryLog::AvatarItemAdd(int, int, const char*, eAvatarItemAddReason)
{
}

void cUserHistoryLog::ItemAdd(INVEN_TYPE, int, int, const Inven_Item&, eItemAddReason)
{
}

void cUserHistoryLog::ItemDel(INVEN_TYPE, int, int, const Inven_Item&, eItemDelReason)
{
}

void cUserHistoryLog::AvatarItemDel(int itemIdx, int avatarUid, const char* agency, eAvatarItemDelReason reason)
{
    CUser* user = *reinterpret_cast<CUser**>(this);
    if (reason == static_cast<eItemDelReason>(0) || reason == static_cast<eItemDelReason>(1)) {
        if (m_pad[0x22] != 0)
            user->LogHistory("Avatar-,%d,%d,%s,%d,\"%s\",\"%s\"", itemIdx, avatarUid, agency, static_cast<int>(reason), m_pad + 0x22, m_pad + 4);
    } else {
        user->LogHistory("Avatar-,%d,%d,%s,%d", itemIdx, avatarUid, agency, static_cast<int>(reason));
    }
}

void cUserHistoryLog::CreatureItemDel(INVEN_TYPE invenType, int itemIdx, int count, int addInfo, int type, eItemDelReason reason)
{
    CUser* user = *reinterpret_cast<CUser**>(this);
    if (type == 5) {
        if (reason == static_cast<eItemDelReason>(0x14) || reason == static_cast<eItemDelReason>(0x15)) {
            if (m_pad[0x22] != 0)
                user->LogHistory("CTItem-,%d,%d,%d,%d,\"%s\",\"%s\"", itemIdx, count, addInfo, static_cast<int>(reason), m_pad + 0x22, m_pad + 4);
        } else {
            user->LogHistory("CTItem-,%d,%d,%d,%d", itemIdx, count, addInfo, static_cast<int>(reason));
        }
        return;
    }
    Inven_Item item;
    item.reset();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&item) + 8) = itemIdx;
    ItemDel(invenType, count, addInfo, item, reason);
}

void cUserHistoryLog::MoneyAdd(int, int, eMoneyAddReason)
{
}

void cUserHistoryLog::MoneySub(int, int, eMoneySubReason)
{
}

void cUserHistoryLog::CoinSub(int, int, eCoinSubReason)
{
}

void cUserHistoryLog::EventCoinSub(int, int, eCoinSubReason)
{
}

void cUserHistoryLog::PayCoinSub(int, int, eCoinSubReason)
{
}

void cUserHistoryLog::pvpMissionClearReward(int, int, int, int)
{
}

void cUserHistoryLog::EnterDungeon(const char*, int)
{
}

void cUserHistoryLog::LeaveDungeon(const char*, int, const char*, int)
{
}

void cUserHistoryLog::LeaveDungeon(int, int, const char*, int)
{
}

void cUserHistoryLog::LeaveDungeon(const char*, int)
{
}

void cUserHistoryLog::DungeonClearInfo(int, int)
{
}

void cUserHistoryLog::CreatureItemAdd(INVEN_TYPE, int, int, int, int, eItemAddReason)
{
}

void cUserHistoryLog::InitSkill(int, int, int, int, eSkillInitReason)
{
}

void cUserHistoryLog::SkillAdd(int, int, int, int, eSkillAddReason)
{
}

void cUserHistoryLog::ClearUsedQP(int before, int after, int initCount, int unused)
{
    (*reinterpret_cast<CUser**>(this))->LogHistory("ClearQP,%d,%d,%d,%d", before, after, initCount, unused);
}
void cUserHistoryLog::SkillDel(int, int, int, int, eSkillDelReason)
{
}

void cUserHistoryLog::SPAdd(int, int, int, eSPAddReason)
{
}

void cUserHistoryLog::SPSub(int, int, int, eSPSubReason)
{
}

void cUserHistoryLog::SFPAdd(int, int, int, eSPAddReason)
{
}

void cUserHistoryLog::SFPSub(int, int, int, eSPSubReason)
{
}

void cUserHistoryLog::FPAdd(int, int, int, eFPAddReason)
{
}

void cUserHistoryLog::FPSub(int, int, int, eFPSubReason)
{
}

void cUserHistoryLog::ChangeGrowType(int, int, int, int, int, eChangeGrowTypeReason)
{
}

void cUserHistoryLog::LevelUp(int, int)
{
}

void cUserHistoryLog::RequestCleanPad(int, int)
{
}

void cUserHistoryLog::SendMail(MSG_MAILBOX_SEND*, unsigned int)
{
}

void cUserHistoryLog::MoveArea(int, int, int, int, int)
{
}

void cUserHistoryLog::SetTrader(const char*, const char*)
{
}

void cUserHistoryLog::TradeBegin()
{
}

void cUserHistoryLog::TradeEnd(eTradeEndReason, int, int, int, int)
{
}

void cUserHistoryLog::TradeItemAddFail(int, int)
{
}

void cUserHistoryLog::LevelDown(int, int)
{
}

void cUserHistoryLog::AchievementComplete(int)
{
}

void cUserHistoryLog::pvpMissionAdd(int, int, int, int)
{
}

void cUserHistoryLog::pvpMissionDel(int, int)
{
}

void cUserHistoryLog::ItemDelCargo(int, int, int, eItemDelReason)
{
}

void cUserHistoryLog::Logout(const char*, const char*, unsigned short, int, int, int)
{
}

void cUserHistoryLog::DeleteInvaildItem(const std::string&, int, int)
{
}
