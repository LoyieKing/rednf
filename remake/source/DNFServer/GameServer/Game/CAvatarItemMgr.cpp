// df_game_r Game/ WongWork::CAvatarItemMgr 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 语义字段：m_registeredItems/m_waitAssignItems/m_tempKeyCounter/m_emblemEnduranceMap
//          /m_enduranceUpdateMap。
#include "CAvatarItemMgr.h"
#include "AvatarRechargeServer.h"
#include "GameTypes.h"
#include "CDataManager.h"
#include <cstring>
namespace WongWork
{

// ORIG 静态池 B 0x5c @0x943bd80；静态初始化 0x82fa193 实证 ctor(0x3a98=15000, 0x3e8=1000)。
DynamicPool<Avatar_Item> CAvatarItemMgr::m_AvatarItemPool(15000, 1000);

// ORIG 布局（nm/报告实证）：m_registeredItems(+0x00)/m_waitAssignItems(+0x18)
// /m_tempKeyCounter(+0x30)/m_emblemInfo(+0x34)/m_emblemEnduranceMap(+0x54)
// /m_enduranceUpdateMap(+0x6c)，总 0x84（CInventory.h:783 权威注释）。
static_assert(sizeof(CAvatarItemMgr) == 0x84, "CAvatarItemMgr ABI size changed");

// ORIG D1/D2 0x0822d2e0：调 Reset() 后成员 map 自动析构（D1 汇编
// Reset → D1(+0x6c) → D1(+0x54) → D1(+0x18) → D1(+0x00)）。
CAvatarItemMgr::~CAvatarItemMgr()
{
    Reset();
}

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
            m_AvatarItemPool.Free(item);
        }
    }
    for (std::map<int, Avatar_Item*>::iterator it = m_waitAssignItems.begin();
         it != m_waitAssignItems.end(); ++it)
    {
        Avatar_Item* item = it->second;
        if (item != 0)
        {
            m_AvatarItemPool.Free(item);
        }
    }
    m_registeredItems.clear();
    m_waitAssignItems.clear();
    m_emblemInfo.init();
    m_emblemEnduranceMap.clear();
    m_enduranceUpdateMap.clear();
}

bool CAvatarItemMgr::IsTempKey(int key) { return (key >> 31) != 0; }   // ORIG 非 const

int CAvatarItemMgr::_MakeTempKey(int key) const { return key | 0x80000000; }

int CAvatarItemMgr::_AssignTempKey()
{
    // ORIG 帧 0x28：P31 dummy local 撑帧（4.4 对 ≥16 字节数组保槽，标量死局部
    // 会被消除；探针 /tmp/slotprobe5.cpp 帧对齐实证）。残余 counter 槽 -0x4 vs
    // ORIG -0xc 为 4.1 单标量首槽策略差（探针 slotprobe6/7/8x/90/91 全败），
    // N82 族封顶不追。
    char framePad[0x10];
    (void)framePad;
    if (m_tempKeyCounter == 0x7fffffff)
        m_tempKeyCounter = 0;
    m_tempKeyCounter++;
    int counter = m_tempKeyCounter;   // ORIG -0xc 槽往返（读→存→读）
    // ORIG 0x082f9e02：成员读后经 -0xc 槽往返再传 _MakeTempKey（P0 具名局部），
    // 帧 0x28；4.4 折叠该 read→temp→read 往返且帧缩至 0x18/槽 -0x4 ⇒ N76-N82 梳。
    return _MakeTempKey(counter);
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
    // ORIG：_FindItemAll 调 IsTempKey（W 0x0822d20d）判定符号位；
    // IsTempKey 为 static（mangled 不变），const 方法可直接调用。
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
            int rate = ARAD::Singleton<AvatarRechargeServer>::Get()->GetRateFromEmblemGrade(grade);
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
    Avatar_Item* item = m_AvatarItemPool.Acquire();
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
    item->setColor(0, 0);   // ORIG 真名 setColor(short,short)
    // ORIG 0x82f95d3: lea 0x18(%eax),%ecx → 插入 m_waitAssignItems(+0x18)
    m_waitAssignItems.insert(std::make_pair(tempKey, item));
    return (unsigned long)tempKey;
}

unsigned long CAvatarItemMgr::RegistNotAssignedAvatarItem(const Inven_Item& invenItem)
{
    int tempKey = _AssignTempKey();
    Avatar_Item* item = m_AvatarItemPool.Acquire();
    item->SetData(invenItem.m_addInfo2, 0, "");
    item->setColor(0, 0);
    m_waitAssignItems.insert(std::make_pair(tempKey, item));
    return (unsigned long)tempKey;
}

// ORIG T 0x082f95f4 _ZN8WongWork14CAvatarItemMgr27RegistNotAssignedAvatarItemE10Inven_Item
// 按值版：与引用重载同语义（SetData(item.m_addInfo2,0,"")、setColorEss(0,0)、
// 插入 m_waitAssignItems(+0x18)，返回 tempKey）。
unsigned long CAvatarItemMgr::RegistNotAssignedAvatarItem(Inven_Item invenItem)
{
    int tempKey = _AssignTempKey();
    Avatar_Item* item = m_AvatarItemPool.Acquire();
    item->SetData(invenItem.m_addInfo2, 0, "");
    item->setColor(0, 0);
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
    item = m_AvatarItemPool.Acquire();
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

int CAvatarItemMgr::GetEmblemReduceRate(int key)   // ORIG 非 const
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
    item->setColor(color1, color2);
    return true;
}

Avatar_Item* CAvatarItemMgr::GetAvatarInfo(int key) const
{
    return _FindItemAll(key);
}

bool CAvatarItemMgr::GetItem(AvatarDetailInfo* info, int* count) const
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
    m_AvatarItemPool.Free(item);
    // [b27 封顶] 残差 = find!=end 双实参求值序（ORIG 4.1 先 end 后 find，
    // 4.4 先 find 后 end——求值序由编译器固定不可定向，N/P23 族）。
    // EraseAvatarItem 同款（end 槽 -0x14/-0x10 对调同根）。
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

bool CAvatarItemMgr::isSocketAvatar(int key)   // ORIG 非 const
{
    const char* jewelData = getJewelSocketData(key);
    if (*(short*)jewelData == 0)
        return false;
    return true;
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
    // [b27 封顶] 残差 = begin/end 槽号互换（ORIG begin@-0xc/end@-0x10，
    // 4.4 反向——P212 类对象槽向差；endIt 先声明形实测反劣）。循环体/clear 全等。
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
        item->updateEmblemSocket((unsigned int)grades[i], (unsigned int)itemIds[i]);
    }
    AddEmblemEndurance(key, item);
    return 1;
}

}  // namespace WongWork
