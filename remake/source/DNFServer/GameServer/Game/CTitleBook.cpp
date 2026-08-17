// df_game_r 称号簿实现（G3-3 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CTitleBook.md（Ghidra C）+ ORIG 反汇编。
#include <algorithm>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CTitleBook.h"
#include "CAchievement.h"
#include "CItemAmplifier.h"
#include "CSystemTime.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"

// ============================================================================
// 本 TU 需要而其它 TU 提供的符号（ORIG 真实符号）
// ============================================================================

extern "C" int sub_CDataManager_GetInvenTypeFromItemSpace(int space)
    asm("_Z25GetInvenTypeFromItemSpace14ENUM_ITEMSPACE");
extern "C" unsigned int sub_GetIntegratedPvPItemAttr(const void* item)
    asm("_Z24GetIntegratedPvPItemAttrRK10Inven_Item");
extern "C" char* sub_NumberToString_uint(unsigned int value, int flag)
    asm("_Z14NumberToStringji");
extern "C" bool sub_GameWorld_IsEnchantRevisionChannel(void* self)
    asm("_ZN9GameWorld24IsEnchantRevisionChannelEv");
extern "C" bool sub_checkTimeoutItem(const void* item)
    asm("_Z17_checkTimeoutItemRK10Inven_Item");
extern "C" char sub_DeleteInvalidItemScript_IsInvalidItem2Delete(void* self,
                                                                int itemIdx)
    asm("_ZN23DeleteInvalidItemScript20IsInvalidItem2DeleteEi");
extern "C" void sub_CUser_RewardItem2DeleteInvalidItem(
    void* self, const std::string& title,
    const std::vector<std::pair<int, int> >& list)
    asm("_ZN5CUser28RewardItem2DeleteInvalidItemERKSsRKSt6vectorISt4pairIiiESaIS4_EE");

class GameWorld;
GameWorld* G_GameWorld();
class CExpandEquipslot;

// ---- 最小依赖声明（完整 CUser/CDataManager/CInventory 属其它 TU） ----
class CItem
{
public:
    virtual ~CItem();
    virtual void make_item(Inven_Item& item) const = 0;
    bool is_stackable() const;
};

class CDataManager
{
public:
    CItem* find_item(int idx) const;
};

CDataManager* G_CDataManager();

int IsEquipable(CUser* user, const CItem* item, int slot);

// ---- 最小 CInventory / PacketGuard（完整声明属背包域 TU） ----
enum INVEN_TYPE
{
    INVEN_TYPE_0 = 0,
    INVEN_TYPE_1 = 1
};

enum eItemDelReason
{
    eItemDelReason_0x2b = 0x2b
};

class CInventory
{
public:
    Inven_Item* GetInvenRef(int invenType, int slot);
    int tryInsertItemIntoInventory(Inven_Item item) const;
    int tryInsertItemIntoEquipment(Inven_Item item) const;
    int insertItemIntoInventory(Inven_Item item, eItemAddReason reason,
                                bool bLog, bool b);
    int insertItemIntoEquipment(Inven_Item item, eItemAddReason reason,
                                bool bLog, bool b);
    int delete_item(INVEN_TYPE invenType, int slot, int count,
                    eItemDelReason reason, bool bLog);
};

int GetInvenSlot(Inven_Item* out, CInventory* self, int invenType, int slot);

class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// CUserCharacInfo::getCurCharacInvenW/R 实际返回 CInventory*（头文件占位为
// Inven_Item*，按 ORIG ABI 使用）
class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenW();
    const CInventory* getCurCharacInvenR() const;
    int getCurCharacNo() const;
    int get_charac_level() const;
};

class CUser : public CUserCharacInfo
{
public:
    unsigned int GetUID() const;
    unsigned int get_acc_id() const;
    int get_charac_no(int type);
    int get_unique_id() const;
    CExpandEquipslot* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE eType) const;
    void Send(PacketGuard& packet);
    void send_equip(int slot);
};

static Inven_Item* GetTitleSlotRaw(CTitleBook* book, int category, int slot)
{
    return book->m_category[category] + slot;
}

const int TITLE_BOOK_MAX[4] = { 0x46, 0xc8, 0x78, 0x3c };

// ---- CDataManager 称号表（+0xa788 stTitleBookInfo，0x58 字节） ----
struct CTitleBookInfoRaw
{
    char m_pad0[0xc];                                            // +0x00
    std::vector<stTitleSection> m_sections;                      // +0x0c
    std::map<int, std::vector<stTitleElement> > m_elements;      // +0x18
    char m_pad34[0x8];                                           // +0x34
    std::vector<stAdjust> m_adjustList;                          // +0x3c
};

static std::vector<stTitleSection>& GetTitleSections(CDataManager* mgr)
{
    return ((CTitleBookInfoRaw*)((char*)mgr + 0xa788))->m_sections;
}

static std::map<int, std::vector<stTitleElement> >& GetTitleElements(
    CDataManager* mgr)
{
    return ((CTitleBookInfoRaw*)((char*)mgr + 0xa788))->m_elements;
}

// 称号簿 DB 存档（stTitleBook_Save，0x6b3e 字节）
struct stTitleBook_Save
{
    char m_data[0x6b3a];   // +0x00
    int m_characNo;        // +0x6b3a
};

// ============================================================================
// stTitleBook
// ============================================================================

stTitleBook::stTitleBook()
{
}

void stTitleBook::clear()
{
    std::memset(this, 0, sizeof(*this));
}

// ============================================================================
// CTitleBook
// ============================================================================

CTitleBook::CTitleBook()
{
    m_user = 0;
    m_category[0] = m_book.m_category0;
    m_category[1] = m_book.m_category1;
    m_category[2] = m_book.m_category2;
    m_category[3] = m_book.m_category3;
}

CTitleBook::~CTitleBook()
{
}

bool CTitleBook::loadData(CUser* pUser, char* pData)
{
    m_user = pUser;
    std::memcpy(&m_book, pData, 0x6b3a);
    return 1;
}

void CTitleBook::getData(char* pData) const
{
    std::memcpy(pData, &m_book, 0x6b3a);
}
bool CTitleBook::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("TitleBook.cpp", 0x9c), true);
    **guard << 0x285;
    **guard << (int)pUser->GetUID();
    stTitleBook_Save* pData = guard->GetInBuffer<stTitleBook_Save>();
    bool bRet = (pData != 0);
    if (bRet)
    {
        std::memset(pData, 0, 0x6b3e);
        pData->m_characNo = pUser->get_charac_no(-1);
        std::memcpy(pData->m_data, m_book.m_category0, 0x10ae);
        std::memcpy(pData->m_data + 0x10ae, m_book.m_category1,
                    0x2fa8);
        std::memcpy(pData->m_data + 0x4056, m_book.m_category2,
                    0x1c98);
        std::memcpy(pData->m_data + 0x5cee, m_book.m_category3,
                    0xe4c);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return bRet;
}
void CTitleBook::_reset()
{
    m_user = 0;
    m_book.clear();
}

void CTitleBook::_gm_clear()
{
    alter();
    m_book.clear();
}

Inven_Item* CTitleBook::getTitleSlot(ENUM_TITLE_BOOK_CATEGORY category,
                                     int slot)
{
    alter();
    return GetTitleSlotRaw(this, (int)category, slot);
}

Inven_Item CTitleBook::getTitleSlotR(ENUM_TITLE_BOOK_CATEGORY category,
                                     int slot)
{
    return _getTitleR(category, slot);
}

bool CTitleBook::findItemLock(unsigned char lock, ENUM_ITEMSPACE& space,
                              int& slot) const
{
    for (int category = 0; category < 4; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            const Inven_Item* item = m_category[category] + i;
            if (item->m_amp.GetLock() == lock)
            {
                space = (ENUM_ITEMSPACE)(category + 0x13);
                slot = i;
                return 1;
            }
        }
    }
    return 0;
}

void CTitleBook::getItemLockList(
    std::vector<item_lock::stItemLockRef>& out)
{
    for (int category = 0; category < 4; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            const Inven_Item* item = m_category[category] + i;
            unsigned char lock = item->m_amp.GetLock();
            if (lock != 0)
            {
                item_lock::stItemLockRef ref;
                ref.m_lock = lock;
                ref.m_space = (char)category + 0x13;
                ref.m_slot = (short)i;
                out.push_back(ref);
            }
        }
    }
}

void CTitleBook::sendList()
{
    for (int category = 0; category < 4; ++category)
    {
        _sendCategoryData(m_user, 0, (ENUM_TITLE_BOOK_CATEGORY)category,
                          ENUM_TITLE_BOOK_INFO_TYPE_0);
    }
}

void CTitleBook::sendListOtherUser(CUser* user,
                                   ENUM_TITLE_BOOK_INFO_TYPE type)
{
    for (int category = 0; category < 4; ++category)
    {
        unsigned short uniqueId = (unsigned short)m_user->get_unique_id();
        _sendCategoryData(user, uniqueId, (ENUM_TITLE_BOOK_CATEGORY)category,
                          type);
    }
}

bool CTitleBook::isAbleItemLock(unsigned long itemIdx)
{
    std::map<int, std::vector<stTitleElement> >& elements =
        GetTitleElements(G_CDataManager());
    for (std::map<int, std::vector<stTitleElement> >::iterator it =
             elements.begin();
         it != elements.end(); ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); ++i)
        {
            const stTitleElement& element = it->second[i];
            if (std::find(element.m_fitJobs.begin(), element.m_fitJobs.end(),
                          (int)itemIdx) != element.m_fitJobs.end())
            {
                if (element.m_achievementIdx == -1)
                {
                    return 1;
                }
                return 0;
            }
        }
    }
    return 1;
}

bool CTitleBook::_checkFitSlot(int itemIdx, ENUM_TITLE_BOOK_CATEGORY category,
                               int slot)
{
    std::map<int, std::vector<stTitleElement> >& elements =
        GetTitleElements(G_CDataManager());
    std::map<int, std::vector<stTitleElement> >::iterator it =
        elements.find((int)category);
    if (it == elements.end())
    {
        return 0;
    }
    if ((int)slot >= (int)it->second.size())
    {
        return 0;
    }
    const stTitleElement& element = it->second[slot];
    if (std::find(element.m_fitJobs.begin(), element.m_fitJobs.end(),
                  itemIdx) != element.m_fitJobs.end())
    {
        return 1;
    }
    if (element.m_achievementIdx == -1)
    {
        return 1;
    }
    return 0;
}

bool CTitleBook::_getFitSlot(int itemIdx, ENUM_TITLE_BOOK_CATEGORY& category,
                             int& slot)
{
    std::map<int, std::vector<stTitleElement> >& elements =
        GetTitleElements(G_CDataManager());
    for (std::map<int, std::vector<stTitleElement> >::iterator it =
             elements.begin();
         it != elements.end(); ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); ++i)
        {
            const stTitleElement& element = it->second[i];
            if (std::find(element.m_fitJobs.begin(), element.m_fitJobs.end(),
                          itemIdx) != element.m_fitJobs.end())
            {
                category = (ENUM_TITLE_BOOK_CATEGORY)it->first;
                slot = (int)i;
                return 1;
            }
        }
    }
    return 0;
}

void CTitleBook::deleteTimeoutItem(
    const std::vector<std::pair<int, int> >& list)
{
    std::vector<std::pair<int, int> > toDelete;
    for (int category = 0; category < 4; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            Inven_Item* item = m_category[category] + i;
            if (item->m_addInfo == 0)
            {
                continue;
            }
            if (sub_checkTimeoutItem(item) ||
                (_isMatchedItem(item->m_addInfo, list) != -1))
            {
                item->reset();
                alter();
            }
            else if (sub_DeleteInvalidItemScript_IsInvalidItem2Delete(
                         (char*)G_CDataManager() + 0xb658, item->m_addInfo))
            {
                toDelete.push_back(
                    std::make_pair(item->m_addInfo, item->m_addInfo2));
                item->reset();
                alter();
            }
        }
    }
    if (m_user)
    {
        sub_CUser_RewardItem2DeleteInvalidItem(
            m_user, std::string("TitleBook"), toDelete);
    }
}

int CTitleBook::_isMatchedItem(
    int itemIdx, const std::vector<std::pair<int, int> >& list)
{
    for (std::vector<std::pair<int, int> >::const_iterator it = list.begin();
         it != list.end(); ++it)
    {
        if (it->first == itemIdx)
        {
            return it->second;
        }
    }
    return -1;
}

int CTitleBook::checkError(CUser* user, ENUM_TITLE_BOOK_CATEGORY category,
                           int slot)
{
    if (!user)
    {
        return 3;
    }
    int result = 0;
    if (category == 1)
    {
        if ((slot < 0) || (slot > 199))
        {
            result = 3;
        }
    }
    else if ((int)category < 2)
    {
        if (category == 0)
        {
            if ((slot >= 0) && (slot < 0x46))
            {
                return 0;
            }
            return 3;
        }
    }
    else
    {
        if (category == 2)
        {
            if ((slot >= 0) && (slot < 0x78))
            {
                return 0;
            }
            return 3;
        }
        if (category == 3)
        {
            if ((slot >= 0) && (slot < 0x3c))
            {
                return 0;
            }
            return 3;
        }
        result = 3;
    }
    return result;
}

int CTitleBook::getTitle(CUser* user, ENUM_TITLE_BOOK_CATEGORY category,
                         int slot, int itemIdx, ENUM_ITEMSPACE space,
                         int& outSlot)
{
    CTitleBook* book = (CTitleBook*)user->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe);
    if (!book)
    {
        return 3;
    }
    if (space == (ENUM_ITEMSPACE)0x12)
    {
        space = (ENUM_ITEMSPACE)3;
    }
    int result = checkError(user, category, slot);
    if (result != 0)
    {
        return result;
    }

    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    int invenType = sub_CDataManager_GetInvenTypeFromItemSpace((int)space);
    Inven_Item title = *book->_getTitle(category, slot);
    if ((itemIdx != 0) && (title.m_addInfo != itemIdx))
    {
        return 2;
    }
    CItem* pItem = G_CDataManager()->find_item(title.m_addInfo);
    if (!pItem)
    {
        return 2;
    }

    if (invenType == 1)
    {
        if (!book->_checkMovetoInven(category, slot))
        {
            return 5;
        }
        if (inven->tryInsertItemIntoInventory(title) < 0)
        {
            return 0xb;
        }
        book->_deleteTitle(category, slot);
        int ret = inven->insertItemIntoInventory(
            title, eItemAddReason_7, true, false);
        if (ret < 0)
        {
            // ORIG 此处仅日志
        }
        outSlot = ret;
    }
    else if (invenType == 0)
    {
        if (outSlot != 0xb)
        {
            return 7;
        }
        if (inven->tryInsertItemIntoEquipment(title) < 0)
        {
            return 7;
        }
        if (IsEquipable(user, pItem, outSlot) != 0)
        {
            return 6;
        }
        unsigned char lock = title.m_amp.GetLock();
        if ((lock != 0) && (title.m_field0 != 0))
        {
            return 1;
        }
        book->_deleteTitle(category, slot);
        int ret = inven->insertItemIntoEquipment(
            title, eItemAddReason_7, true, false);
        if (ret < 0)
        {
            // ORIG 此处仅日志
        }
        user->send_equip(ret);
    }
    return 0;
}

int CTitleBook::putTitle(CUser* user, ENUM_ITEMSPACE space, int slot,
                         int itemIdx, ENUM_TITLE_BOOK_CATEGORY category,
                         int titleSlot)
{
    CTitleBook* book = (CTitleBook*)user->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe);
    if (!book)
    {
        return 3;
    }
    if (space == (ENUM_ITEMSPACE)0x12)
    {
        space = (ENUM_ITEMSPACE)3;
    }
    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    int invenType = sub_CDataManager_GetInvenTypeFromItemSpace((int)space);

    Inven_Item invenItem;
    GetInvenSlot(&invenItem, inven, invenType, slot);
    if ((itemIdx != 0) && (invenItem.m_addInfo != itemIdx))
    {
        return 2;
    }

    unsigned int sectionIdx =
        CAchievement::_getSectionIdx(category, titleSlot);
    if (sectionIdx != 0xffffffff)
    {
        stTitleSection& section =
            GetTitleSections(G_CDataManager())[sectionIdx];
        if (!section.m_name.empty())
        {
            return 0xc;
        }
    }
    if (user->get_charac_level() < 10)
    {
        return 8;
    }
    if ((category == 2) && (user->get_charac_level() < 0x46))
    {
        return 9;
    }
    if (!book->_checkFitSlot(itemIdx, category, titleSlot))
    {
        return 2;
    }
    if (!book->_checkInsertTitle(category, titleSlot, &invenItem))
    {
        return 2;
    }
    if (!inven->delete_item((INVEN_TYPE)invenType, slot, 1,
                            eItemDelReason_0x2b, true))
    {
        return 2;
    }
    if ((invenType == 0) && (slot == 0xb))
    {
        user->send_equip(0xb);
    }

    Inven_Item old = book->getTitleSlotR(category, titleSlot);
    if (old.m_addInfo != 0)
    {
        if (invenType == 1)
        {
            int ret = inven->insertItemIntoInventory(
                old, eItemAddReason_7, true, false);
            if (ret < 0)
            {
                // ORIG 此处仅日志
            }
        }
        else if (invenType == 0)
        {
            int ret = inven->insertItemIntoEquipment(
                old, eItemAddReason_7, true, false);
            if (ret < 0)
            {
                // ORIG 此处仅日志
            }
            else
            {
                user->send_equip(slot);
            }
        }
    }
    book->_insertTitle(category, titleSlot, &invenItem);
    return 0;
}

int CTitleBook::putTitleOnly(CUser* user, int itemIdx)
{
    ENUM_TITLE_BOOK_CATEGORY category;
    int slot;
    return putTitleOnly(user, itemIdx, category, slot);
}

int CTitleBook::putTitleOnly(CUser* user, int itemIdx,
                             ENUM_TITLE_BOOK_CATEGORY& category, int& slot)
{
    CTitleBook* book = (CTitleBook*)user->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe);
    if (!book)
    {
        return 0;
    }
    if (itemIdx == 0)
    {
        return 0;
    }
    Inven_Item item;
    item.m_addInfo = itemIdx;
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (!pItem)
    {
        return 0;
    }
    pItem->make_item(item);
    item.set_add_info(1);
    if (!_getFitSlot(itemIdx, category, slot))
    {
        return 0;
    }
    if (!book->_checkInsertTitle(category, slot, &item))
    {
        return 0;
    }
    book->_insertTitle(category, slot, &item);
    return 1;
}

// ============================================================================
// 内部辅助
// ============================================================================

Inven_Item* CTitleBook::_getTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    alter();
    return GetTitleSlotRaw(this, (int)category, slot);
}

Inven_Item CTitleBook::_getTitleR(ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    Inven_Item out;
    std::memmove(&out, GetTitleSlotRaw((CTitleBook*)this, (int)category, slot),
                 0x3d);
    return out;
}

void CTitleBook::_insertTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot,
                              Inven_Item* item)
{
    alter();
    GetTitleSlotRaw(this, (int)category, slot)->setCopy(*item);
}

void CTitleBook::_deleteTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    alter();
    GetTitleSlotRaw(this, (int)category, slot)->reset();
}

bool CTitleBook::_checkInsertTitle(ENUM_TITLE_BOOK_CATEGORY category,
                                   int slot, Inven_Item* item)
{
    if (!item)
    {
        return 0;
    }
    CItem* pItem = G_CDataManager()->find_item(item->m_addInfo);
    if (!pItem)
    {
        return 0;
    }
    if (pItem->is_stackable())
    {
        return 0;
    }
    if ((category == 3) && (item->m_field0 != 0))
    {
        return 0;
    }
    return 1;
}

bool CTitleBook::_checkMovetoInven(ENUM_TITLE_BOOK_CATEGORY category,
                                   int slot)
{
    std::map<int, std::vector<stTitleElement> >& elements =
        GetTitleElements(G_CDataManager());
    std::map<int, std::vector<stTitleElement> >::iterator it =
        elements.find((int)category);
    if (it == elements.end())
    {
        return 0;
    }
    if ((int)slot >= (int)it->second.size())
    {
        return 0;
    }
    return it->second[slot].m_achievementIdx == -1;
}

bool CTitleBook::_getOwnerTitleInfo(ENUM_TITLE_BOOK_CATEGORY& category,
                                    int& slot, Inven_Item* out)
{
    const CInventory* inven =
        (const CInventory*)m_user->getCurCharacInvenR();
    Inven_Item item;
    GetInvenSlot(&item, (CInventory*)inven, 0, 0xb);
    *out = item;
    if ((out->m_addInfo != 0) &&
        _getFitSlot(out->m_addInfo, category, slot))
    {
        return 1;
    }
    return 0;
}

int CTitleBook::_getCategoryItemCount(ENUM_TITLE_BOOK_CATEGORY category)
{
    int count = 0;
    for (int i = 0; i < TITLE_BOOK_MAX[(int)category]; ++i)
    {
        if (GetTitleSlotRaw(this, (int)category, i)->m_addInfo != 0)
        {
            ++count;
        }
    }
    return count;
}

bool CTitleBook::_empty()
{
    for (int category = 0; category < 4; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            if (GetTitleSlotRaw(this, category, i)->m_addInfo != 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

void CTitleBook::_sendCategoryData(CUser* user, unsigned short extra,
                                   ENUM_TITLE_BOOK_CATEGORY category,
                                   ENUM_TITLE_BOOK_INFO_TYPE type)
{
    int count = 0;
    int ownerSlot = 0;
    int category2 = 0;
    Inven_Item ownerItem;
    char hasOwner = 0;

    PacketGuard packet;
    packet.put_header(0, 0x165);
    packet.put_byte((int)type);
    packet.put_short((int)extra);
    packet.put_int((int)category);
    count = _getCategoryItemCount(category);
    if (extra != 0)
    {
        ENUM_TITLE_BOOK_CATEGORY ownerCategory;
        if (_getOwnerTitleInfo(ownerCategory, ownerSlot, &ownerItem) &&
            (category == ownerCategory))
        {
            ++count;
            hasOwner = 1;
        }
    }
    packet.put_int(count);
    for (int i = 0; i < TITLE_BOOK_MAX[(int)category]; ++i)
    {
        Inven_Item* item = GetTitleSlotRaw(this, (int)category, i);
        if (item->m_addInfo != 0)
        {
            _putItemData(packet, (short)i, *item);
        }
    }
    if (hasOwner)
    {
        _putItemData(packet, (short)ownerSlot, ownerItem);
    }
    packet.finalize(true);
    user->Send(packet);
}

void CTitleBook::_putItemData(PacketGuard& packet, short slot,
                              Inven_Item& item)
{
    packet.put_short((int)slot);
    packet.put_int(item.m_addInfo);
    packet.put_int(item.m_addInfo2);
    packet.put_byte((int)sub_GetIntegratedPvPItemAttr(&item) & 0xff);
    packet.put_short((int)item.m_fieldb);
    packet.put_byte((int)item.m_field0);
    int fieldD = item.m_fieldd;
    if (sub_GameWorld_IsEnchantRevisionChannel(G_GameWorld()))
    {
        fieldD = 0;
    }
    packet.put_int(fieldD);
    packet.put_byte((int)item.m_amp.getAbilityType() & 0xff);
    packet.put_short((int)item.m_amp.getAbilityValue() & 0xffff);
}
