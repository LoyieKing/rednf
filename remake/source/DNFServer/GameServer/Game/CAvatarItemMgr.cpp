// df_game_r Game/ WongWork::CAvatarItemMgr 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 语义字段：m_registeredItems/m_waitAssignItems/m_tempKeyCounter/m_emblemEnduranceMap
//          /m_enduranceUpdateMap。
#include "CAvatarItemMgr.h"
#include "AvatarRechargeServer.h"
#include "GameTypes.h"
#include "CDataManager.h"
#include "CItem.h"
#include <cstring>

namespace WongWork
{

void CAvatarItemMgr::Reset()
{
    m_tempKeyCounter = 1;
    if (m_registeredItems.empty() && m_waitAssignItems.empty())
        return;

    for (std::map<int, Avatar_Item*>::iterator it = m_registeredItems.begin();
         it != m_registeredItems.end(); ++it)
    {
        Avatar_Item* item = it->second;
        if (item != 0)
        {
            item->init();
            delete item;
        }
    }
    for (std::map<int, Avatar_Item*>::iterator it = m_waitAssignItems.begin();
         it != m_waitAssignItems.end(); ++it)
    {
        Avatar_Item* item = it->second;
        if (item != 0)
        {
            delete item;
        }
    }
    m_registeredItems.clear();
    m_waitAssignItems.clear();
    m_emblemInfo.init();
    m_emblemEnduranceMap.clear();
    m_enduranceUpdateMap.clear();
}

bool CAvatarItemMgr::IsTempKey(int key) const { return (key >> 31) != 0; }

int CAvatarItemMgr::_MakeTempKey(int key) const { return key | 0x80000000; }

int CAvatarItemMgr::_AssignTempKey()
{
    if (m_tempKeyCounter == 0x7fffffff)
        m_tempKeyCounter = 0;
    m_tempKeyCounter++;
    return _MakeTempKey(m_tempKeyCounter);
}

Avatar_Item* CAvatarItemMgr::_FindItem(int key) const
{
    std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.find(key);
    if (it == m_registeredItems.end())
        return 0;
    return it->second;
}

Avatar_Item* CAvatarItemMgr::_FindWaitAssignItem(int key) const
{
    std::map<int, Avatar_Item*>::const_iterator it = m_waitAssignItems.find(key);
    if (it == m_waitAssignItems.end())
        return 0;
    return it->second;
}

Avatar_Item* CAvatarItemMgr::_FindItemAll(int key) const
{
    if (IsTempKey(key))
        return _FindWaitAssignItem(key);
    std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.find(key);
    if (it == m_registeredItems.end())
        return 0;
    return it->second;
}

void CAvatarItemMgr::AddAvatarItem(int key, Avatar_Item* item)
{
    m_registeredItems.insert(std::make_pair(key, item));
    AddEmblemEndurance(key, item);
}

void CAvatarItemMgr::AddEmblemEndurance(int key, const Avatar_Item* item)
{
    const char* jewelData = item->getJewelSocketData();
    short totalEndurance = 0;
    for (int i = 0; i <= 4; i++)
    {
        int itemId = *(int*)(jewelData + i * 6 + 2);
        if (itemId != 0)
        {
            CItem* citem = G_CDataManager()->find_item(itemId);
            if (citem == 0)
                return;
            int grade = citem->get_grade();
            int rate = AvatarRechargeServer::GetRateFromEmblemGrade(grade);
            totalEndurance += (short)rate;
        }
    }
    std::map<int, int>::iterator it = m_emblemEnduranceMap.find(key);
    if (it != m_emblemEnduranceMap.end())
    {
        it->second = (int)totalEndurance;
    }
    else
    {
        m_emblemEnduranceMap.insert(std::make_pair(key, (int)totalEndurance));
    }
}

bool CAvatarItemMgr::AssignAvatarItemInfo(int key, int newKey, int expireDate,
                                          int itemIdx, const char* ipgAgency)
{
    Avatar_Item* item = _FindWaitAssignItem(key);
    if (item == 0)
        return false;
    item->SetData(expireDate, itemIdx, ipgAgency);
    m_waitAssignItems.erase(key);
    m_registeredItems.insert(std::make_pair(newKey, item));
    AddEmblemEndurance(newKey, item);
    return true;
}

bool CAvatarItemMgr::AssignAvatarItemKey(int oldKey, int newKey)
{
    Avatar_Item* item = _FindWaitAssignItem(oldKey);
    if (item == 0)
        return false;
    m_waitAssignItems.erase(oldKey);
    m_registeredItems.insert(std::make_pair(newKey, item));
    AddEmblemEndurance(newKey, item);
    return true;
}

unsigned long CAvatarItemMgr::RegistNotAssignedAvatarItem(int itemIdx, int tick,
                                                          const char* ipgAgency, bool b,
                                                          const stAvatarEmblemInfo_t& emblem)
{
    int tempKey = _AssignTempKey();
    Avatar_Item* item = new Avatar_Item();
    int expireDate;
    if (itemIdx == 0 || itemIdx == 0x16d)
    {
        expireDate = 0;
    }
    else if (!b)
    {
        expireDate = itemIdx * 0x15180 + tick;
    }
    else
    {
        expireDate = tick + itemIdx;
    }
    item->SetData(expireDate, tick, ipgAgency);
    item->setEmblemSocketData(emblem);
    item->setColorEss(0, 0);
    // ORIG 0x82f95d3: lea 0x18(%eax),%ecx → 插入 m_waitAssignItems(+0x18)
    m_waitAssignItems.insert(std::make_pair(tempKey, item));
    return (unsigned long)tempKey;
}

unsigned long CAvatarItemMgr::RegistNotAssignedAvatarItem(const Inven_Item& invenItem)
{
    int tempKey = _AssignTempKey();
    Avatar_Item* item = new Avatar_Item();
    item->SetData(invenItem.m_addInfo2, 0, "");
    item->setColorEss(0, 0);
    m_waitAssignItems.insert(std::make_pair(tempKey, item));
    return (unsigned long)tempKey;
}

int CAvatarItemMgr::RegistItem(int key, int expireDate, int registTime, const char* ipgAgency,
                               const stAvatarEmblemInfo_t& emblem,
                               const stAvatarExpansionInfo_t& expansion, bool b)
{
    Avatar_Item* item = _FindItem(key);
    if (item != 0)
        return 0;
    item = new Avatar_Item();
    int expireDate2 = 0;
    if (b)
    {
        if (expireDate != 0 && expireDate != 0x16d)
            expireDate2 = expireDate * 0x15180 + registTime;
    }
    else
    {
        expireDate2 = expireDate;
    }
    item->SetData(expireDate2, registTime, ipgAgency);
    item->setEmblemSocketData(emblem);
    item->setExpansionInfo(expansion);
    m_registeredItems.insert(std::make_pair(key, item));
    AddEmblemEndurance(key, item);
    return 1;
}

void CAvatarItemMgr::RegistItem(const AvatarDetailInfo& info, bool b)
{
    RegistItem(info.m_key, info.m_expireDate, info.m_registTime, info.m_ipgAgencyNo,
               *(const stAvatarEmblemInfo_t*)&info.m_jewelSocket,
               *(const stAvatarExpansionInfo_t*)&info.m_expansionInfo, b);
}

int CAvatarItemMgr::GetExpireDate(int avatarUid) const
{
    Avatar_Item* item = _FindItemAll(avatarUid);
    if (item == 0)
        return -1;
    return item->GetExpireDate();
}

int CAvatarItemMgr::GetRegistTime(int avatarUid) const
{
    Avatar_Item* item = _FindItemAll(avatarUid);
    if (item == 0)
        return -1;
    return item->GetRegistTime();
}

const char* CAvatarItemMgr::GetIPGAgencyNo(int addInfo) const
{
    Avatar_Item* item = _FindItemAll(addInfo);
    if (item == 0)
        return 0;
    return item->GetIPGAgencyNo();
}

int CAvatarItemMgr::GetRemainDate(int nAvatarUid, int nTick) const
{
    Avatar_Item* item = _FindItemAll(nAvatarUid);
    if (item == 0)
        return -1;
    return item->GetRemainDate(nTick);
}

int CAvatarItemMgr::GetEmblemReduceRate(int key) const
{
    std::map<int, int>::const_iterator it = m_emblemEnduranceMap.find(key);
    if (it == m_emblemEnduranceMap.end())
        return 0;
    return it->second;
}

const char* CAvatarItemMgr::getJewelSocketData(int key) const
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return (const char*)&m_emblemInfo;
    return item->getJewelSocketData();
}

void* CAvatarItemMgr::GetExpansionInfo(int key) const
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return 0;
    return item->getExpansionInfo();
}

bool CAvatarItemMgr::SetAvatarColor(int key, short color1, short color2)
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return false;
    item->setColorEss(color1, color2);
    return true;
}

Avatar_Item* CAvatarItemMgr::GetAvatarInfo(int key) const
{
    return _FindItemAll(key);
}

int CAvatarItemMgr::GetItem(AvatarDetailInfo* info, int* count) const
{
    int i = 0;
    for (std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.begin();
         it != m_registeredItems.end() && i <= 0x72; ++it, ++i)
    {
        Avatar_Item* item = it->second;
        info[i].m_key = it->first;
        info[i].m_expireDate = item->GetExpireDate();
        info[i].m_registTime = item->GetRegistTime();
        strncpy(info[i].m_ipgAgencyNo, item->GetIPGAgencyNo(), 0x19);
        const char* jewelData = item->getJewelSocketData();
        memcpy(&info[i].m_jewelSocket, jewelData, 0x1e);
        info[i].m_expansionInfo = *(const stAvatarExpansionInfo_t*)item->getExnansionInfoRef();
    }
    *count = i;
    return 1;
}

bool CAvatarItemMgr::UnRegistItem(int key)
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return false;
    m_registeredItems.erase(key);
    delete item;
    std::map<int, int>::iterator it = m_emblemEnduranceMap.find(key);
    if (it != m_emblemEnduranceMap.end())
        m_emblemEnduranceMap.erase(it);
    return true;
}

Avatar_Item* CAvatarItemMgr::EraseAvatarItem(int key)
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return 0;
    m_registeredItems.erase(key);
    std::map<int, int>::iterator it = m_emblemEnduranceMap.find(key);
    if (it != m_emblemEnduranceMap.end())
        m_emblemEnduranceMap.erase(it);
    return item;
}

bool CAvatarItemMgr::isSocketAvatar(int key) const
{
    const char* jewelData = getJewelSocketData(key);
    return *(short*)jewelData != 0;
}

int CAvatarItemMgr::setEmblemSocketData(int key, const stAvatarEmblemInfo_t& emblem)
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return 0;
    item->setEmblemSocketData(emblem);
    AddEmblemEndurance(key, item);
    return 1;
}

void CAvatarItemMgr::UpdateEndurance(int key, int endurance)
{
    std::map<int, int>::iterator it = m_enduranceUpdateMap.find(key);
    if (it != m_enduranceUpdateMap.end())
        it->second = endurance;
    else
        m_enduranceUpdateMap.insert(std::make_pair(key, endurance));
}

void CAvatarItemMgr::SaveEndurance()
{
    if (m_enduranceUpdateMap.empty())
        return;
    for (std::map<int, int>::iterator it = m_enduranceUpdateMap.begin();
         it != m_enduranceUpdateMap.end(); ++it)
    {
        AvatarRechargeServer::SaveAvatarEnduranceUnit(it->first, it->second);
    }
    m_enduranceUpdateMap.clear();
}

int CAvatarItemMgr::updateEmblemSocket(int key, unsigned int count, char* grades, unsigned long* itemIds)
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return 0;
    for (unsigned int i = 0; i < count; i++)
    {
        item->updateEmblemSocket(grades[i], itemIds[i]);
    }
    AddEmblemEndurance(key, item);
    return 1;
}

}  // namespace WongWork
