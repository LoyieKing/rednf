// ============================================================================
// df_game_r 还原 —— CExpandEquipslot（G1-2 扩展装备槽，26 符号）
// 参照 docs/class_func_reports/CExpandEquipslot.md + ORIG 反汇编逐函数核对。
// 布局：charac_expand::CData 基类（pack(1)，vptr+0x0 / m_bIsSave+0x4）+
//   Inven_Item m_equipSlot[12]@+0x05 / m_expandEquipSlot[12]@+0x2e1 /
//   m_expandAvatarSlot[12]@+0x5bd，sizeof = 0x899。
// CUser/CInventory/CItem/CParty 等暂无权威头，仅声明本 TU 调用形态。
// ============================================================================

#include <cstring>
#include <vector>

#include "CExpandEquipslot.h"
#include "CItem.h"
#include "CItemAmplifier.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "InterfacePacketBuf.h"
#include "Stream.h"

// ---- PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char）----
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// ---- CUserCharacInfo / CUser（最小声明；CUser 继承于 +0x00）----
class CParty;

class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenW();
    const CInventory* getCurCharacInvenR() const;
    char getCurChannelEquipslotSwitch();
    char getCurExpandEquipslotSwitch();
    void setCurChannelEquipslotSwitch(char value);
};

class CUser : public CUserCharacInfo
{
public:
    int get_state();
    unsigned int GetUID() const;
    unsigned short get_unique_id() const;
    int get_charac_no(int type);
    CExpandEquipslot* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE type) const;
    bool CheckItemLock(int a, int b) const;
    CParty* GetParty();
    void send_equip(int slot);
    void make_basic_info(char* pBuf, char flag);
    void Send(PacketGuard& packet);
};

// ---- CInventory（最小声明；m_pEquipSlot 经字节算术访问 +0x650）----
class CInventory
{
public:
    bool CheckValidSlot(int invenType, int slot) const;
    const Inven_Item* GetInvenRef(int invenType, int slot) const;
    Inven_Item GetInvenSlot(int invenType, int slot) const;
    void GetInvenData(int invenType, void* out, int size) const;
    void SetInvenData(int invenType, void* in, int size);
    void MakeEquipList(void* packet, bool flag, ENUM_USERINFO info,
                       ENUM_EQUIPSLOT slot) const;

    char m_pad1c[0x1c];
    Inven_Item m_invenItem[26];   // +0x1c
    char m_pad64e[0x650 - 0x64e];
    Inven_Item* m_pEquipSlot;  // +0x650
};

bool CheckEquipable(const Inven_Item& a, const Inven_Item& b);
int IsEquipable(CUser* user, const CItem* item, int slot);

// ---- item_lock（EquipslotSwitch 迭代用，最小声明）----
namespace item_lock
{
struct stItemLockRef
{
    char m_field0;    // +0x00
    char m_space;     // +0x01
    unsigned short m_slot;  // +0x02
};

class CItemLock
{
public:
    std::vector<stItemLockRef>& GetItemLockRefVec();
};
}

class CParty
{
public:
    void Reset_party_overlapped_drop_ratio();
};

class CCharacterView
{
public:
    void enableSaveCharacView();
};

class CDataManager
{
public:
    CItem* find_item(int itemIdx) const;
};

extern CDataManager* G_CDataManager();

class CSecu_ProtectionField
{
public:
    static int Check(CSecu_ProtectionField* field, CUser* user,
                     SECURITY_PROTCTION type);
};

class StreamPool
{
public:
    Stream* Acquire(const char* pFile, int nLine);
};

class CStreamGuard
{
public:
    CStreamGuard(Stream* stream, bool bOwn);
    ~CStreamGuard();

    CStreamGuard* operator->();
    CStreamGuard* operator*();
    Stream& operator<<(int v);

    template <class T> T* GetInBuffer();

    Stream* m_stream;  // +0x00
    bool m_bOwn;       // +0x04
};

template <class T>
T* CStreamGuard::GetInBuffer()
{
    return m_stream->GetInBuffer<T>();
}

class MsgQueueMgr
{
public:
    enum QUEUE_IDX
    {
        NETWORK_Q = 0,
        DISPATCH_Q = 1,
        DB_Q = 2,
        TIMER_Q = 3,
        LOGDB_Q = 4
    };

    void put(QUEUE_IDX queue_idx, CStreamGuard& guard);
};

namespace GlobalData
{
extern CSecu_ProtectionField* s_pSecuProtectionField;
extern StreamPool* s_stream_pool;
extern MsgQueueMgr* s_msgq_mgr;
extern CSystemTime s_systemTime_;
}

// ---- 存档结构（0x89c 字节：characNo + 0x24 个 Inven_Item + 2 个 switch）----
#pragma pack(push, 1)
struct SIG_SAVE_EXPAND_CHARAC
{
    int m_characNo;              // +0x00
    Inven_Item m_items[0x24];    // +0x04
    unsigned char m_switch1;     // +0x898
    unsigned char m_switch2;     // +0x899
    char m_pad[0x89c - 0x89a];   // +0x89a
};
#pragma pack(pop)

// ============================================================================
// 构造 / 析构
// ============================================================================

CExpandEquipslot::CExpandEquipslot()
{
}

CExpandEquipslot::~CExpandEquipslot()
{
}

// ============================================================================
// 简单访问器
// ============================================================================

Inven_Item* CExpandEquipslot::getExpandEquipslotR(ENUM_EQUIPSLOT slot) const
{
    if (slot == ENUM_EQUIPSLOT_1)
    {
        return const_cast<Inven_Item*>(m_equipSlot);
    }
    if (slot == ENUM_EQUIPSLOT_2)
    {
        return const_cast<Inven_Item*>(m_expandEquipSlot);
    }
    if (slot == ENUM_EQUIPSLOT_3)
    {
        return const_cast<Inven_Item*>(m_expandAvatarSlot);
    }
    return 0;
}

Inven_Item* CExpandEquipslot::getExpandEquipslotW(ENUM_EQUIPSLOT slot)
{
    if (slot == ENUM_EQUIPSLOT_1)
    {
        alter();
        return m_equipSlot;
    }
    if (slot == ENUM_EQUIPSLOT_2)
    {
        alter();
        return m_expandEquipSlot;
    }
    if (slot == ENUM_EQUIPSLOT_3)
    {
        alter();
        return m_expandAvatarSlot;
    }
    return 0;
}

Inven_Item* CExpandEquipslot::GetInvenRefR(INVEN_TYPE type, int slot) const
{
    if (slot < 10 || 0x16 < slot)
    {
        return 0;
    }
    switch (type)
    {
    case INVEN_TYPE_5:
        return const_cast<Inven_Item*>(&m_equipSlot[slot - 10]);
    default:
        return 0;
    }
}

Inven_Item* CExpandEquipslot::GetInvenRefW(INVEN_TYPE type, int slot)
{
    if (slot < 10 || 0x16 < slot)
    {
        return 0;
    }
    switch (type)
    {
    case INVEN_TYPE_5:
        alter();
        return &m_equipSlot[slot - 10];
    default:
        return 0;
    }
}

Inven_Item CExpandEquipslot::GetInvenSlot(INVEN_TYPE type, int slot) const
{
    Inven_Item item;
    if (slot < 10 || 0x16 < slot)
    {
        return item;
    }
    switch (type)
    {
    case INVEN_TYPE_5:
        return m_equipSlot[slot - 10];
    default:
        return item;
    }
}

bool CExpandEquipslot::FindItemLock(unsigned char lock, ENUM_ITEMSPACE& space,
                                    int& slot) const
{
    slot = 0;
    while (slot <= 0xb)
    {
        if (m_equipSlot[slot].m_amp.GetLock() == lock)
        {
            slot = slot + 10;
            space = (ENUM_ITEMSPACE)0x12;
            return 1;
        }
        slot = slot + 1;
    }
    return 0;
}

// ============================================================================
// 数据 / 存档
// ============================================================================

void CExpandEquipslot::_reset()
{
    for (int i = 0; i < 0xc; ++i)
    {
        m_equipSlot[i].reset();
        m_expandEquipSlot[i].reset();
        m_expandAvatarSlot[i].reset();
    }
}

void CExpandEquipslot::getData(char* pData) const
{
    SIG_SAVE_EXPAND_CHARAC* pData2 = (SIG_SAVE_EXPAND_CHARAC*)pData;
    int nGroupCount = 0xc;
    int nTotalCount = 0x24;
    int nType = 1;
    for (unsigned int i = 0; i < 0x24; ++i)
    {
        if (i != 0 && (i % 0xc) == 0)
        {
            if (i == 0xc)
            {
                nType = 2;
            }
            else if (0xc < i)
            {
                nType = 3;
            }
        }
        if (nType == 1)
        {
            pData2->m_items[i].setCopy(m_equipSlot[i]);
        }
        else if (nType == 2)
        {
            pData2->m_items[i].setCopy(m_expandEquipSlot[i % 0xc]);
        }
        else if (nType == 3)
        {
            pData2->m_items[i].setCopy(m_expandAvatarSlot[i % 0xc]);
        }
    }
}

bool CExpandEquipslot::loadData(CUser* pUser, char* pData)
{
    _reset();
    SIG_SAVE_EXPAND_CHARAC* pData2 = (SIG_SAVE_EXPAND_CHARAC*)pData;
    int nType = 1;
    int nGroupCount = 0xc;
    int nTotalCount = 0x24;
    for (unsigned int i = 0; i < 0x24; ++i)
    {
        if (i != 0 && (i % 0xc) == 0)
        {
            if (i == 0xc)
            {
                nType = 2;
            }
            else if (0xc < i)
            {
                nType = 3;
            }
        }
        if (nType == 1)
        {
            m_equipSlot[i].setCopy(pData2->m_items[i]);
        }
        else if (nType == 2)
        {
            m_expandEquipSlot[i % 0xc].setCopy(pData2->m_items[i]);
        }
        else if (nType == 3)
        {
            m_expandAvatarSlot[i % 0xc].setCopy(pData2->m_items[i]);
        }
    }
    alter();
    return 1;
}

bool CExpandEquipslot::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ExpandEquipslot.cpp", 0x247),
        true);
    **guard << 0x26d;
    **guard << (int)pUser->GetUID();
    int nGroupCount = 0xc;
    int nTotalCount = 0x24;
    SIG_SAVE_EXPAND_CHARAC* pData =
        guard->GetInBuffer<SIG_SAVE_EXPAND_CHARAC>();
    if (pData == 0)
    {
        return 0;
    }
    memset(pData, 0, 0x89c);
    pData->m_characNo = pUser->get_charac_no(-1);
    int nType = 1;
    for (unsigned int i = 0; i < 0x24; ++i)
    {
        if (i != 0 && (i % 0xc) == 0)
        {
            if (i == 0xc)
            {
                nType = 2;
            }
            else if (0xc < i)
            {
                nType = 3;
            }
        }
        if (nType == 1)
        {
            pData->m_items[i].setCopy(m_equipSlot[i]);
        }
        else if (nType == 2)
        {
            pData->m_items[i].setCopy(m_expandEquipSlot[i % 0xc]);
        }
        else if (nType == 3)
        {
            pData->m_items[i].setCopy(m_expandAvatarSlot[i % 0xc]);
        }
    }
    pData->m_switch1 = (unsigned char)pUser->getCurChannelEquipslotSwitch();
    pData->m_switch2 = (unsigned char)pUser->getCurExpandEquipslotSwitch();
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

void CExpandEquipslot::SetExpandEquipslot(int type, void* data, int size)
{
    switch (type)
    {
    case 1:
        if (size == 0x2dc)
        {
            memcpy(m_equipSlot, data, size);
            alter();
        }
        break;
    case 2:
        if (size == 0x2dc)
        {
            memcpy(m_expandEquipSlot, data, size);
            alter();
        }
        break;
    case 3:
        if (size == 0x2dc)
        {
            memcpy(m_expandAvatarSlot, data, size);
            alter();
        }
        break;
    default:
        break;
    }
}

// ============================================================================
// 发包
// ============================================================================

int CExpandEquipslot::Send_Equip_Info(CUser* pUser)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            "int CExpandEquipslot::Send_Equip_Info(CUser*)", 0x9a,
            "Fail Send_Equip_Info : pUser is NULL");
        return 1;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            "int CExpandEquipslot::Send_Equip_Info(CUser*)", 0xa0,
            "Fail Send_Equip_Info : pUser is not valid state");
        return 0x13;
    }
    PacketGuard packetGuard;
    packetGuard.clear();
    packetGuard.put_header(0, 2);
    packetGuard.put_byte(1);
    pUser->make_basic_info((char*)&packetGuard, 1);
    packetGuard.finalize(true);
    pUser->Send(packetGuard);
    return 0;
}

int CExpandEquipslot::Send_Expand_Equip_Info(CUser* pUser)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            "int CExpandEquipslot::Send_Expand_Equip_Info(CUser*)", 0x21b,
            "Fail Send_Expand_Equip_Info : pUser is NULL");
        return 1;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            "int CExpandEquipslot::Send_Expand_Equip_Info(CUser*)", 0x221,
            "Fail Send_Expand_Equip_Info : pUser is NULL");
        return 0x13;
    }
    PacketGuard packetGuard;
    packetGuard.clear();
    packetGuard.put_header(0, 0x153);
    packetGuard.put_short(pUser->get_unique_id());
    for (unsigned char d = 1; d < 4; ++d)
    {
        if (d == 1)
        {
            if (pUser->getCurChannelEquipslotSwitch() == 1)
            {
                packetGuard.put_byte(0);
            }
            else
            {
                packetGuard.put_byte((int)d);
            }
        }
        else
        {
            packetGuard.put_byte((int)d);
        }
        pUser->getCurCharacInvenR()->MakeEquipList(
            &packetGuard, true, (ENUM_USERINFO)1, (ENUM_EQUIPSLOT)d);
    }
    packetGuard.finalize(true);
    pUser->Send(packetGuard);
    return 0;
}

// ============================================================================
// 装备切换 / 移动
// ============================================================================

int CExpandEquipslot::EquipslotSwitch(CUser* pUser, char flag,
                                      char channelSwitch, char expandSwitch)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            "int CExpandEquipslot::EquipslotSwitch(CUser*, char, char, char)",
            0x44, "Fail Equipslot Switch : pUser is NULL");
        return 1;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            "int CExpandEquipslot::EquipslotSwitch(CUser*, char, char, char)",
            0x4a, "Fail Equipslot Switch : pUser is not valid state");
        return 0x13;
    }
    char curChannel = pUser->getCurChannelEquipslotSwitch();
    char curExpand = pUser->getCurExpandEquipslotSwitch();
    if (curChannel != channelSwitch || curExpand != expandSwitch)
    {
        return 1;
    }
    item_lock::CItemLock* lock =
        (item_lock::CItemLock*)pUser->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)2);
    std::vector<item_lock::stItemLockRef>& refVec =
        lock->GetItemLockRefVec();
    for (std::vector<item_lock::stItemLockRef>::iterator it = refVec.begin();
         it != refVec.end(); it++)
    {
        item_lock::stItemLockRef& ref = *it;
        if (ref.m_slot > 9 && ref.m_slot < 0x16)
        {
            if (ref.m_space == 3)
            {
                ref.m_space = 0x12;
            }
            else if (ref.m_space == 0x12)
            {
                ref.m_space = 3;
            }
        }
    }
    if (flag == 0)
    {
        CInventory* inven = pUser->getCurCharacInvenW();
        Inven_Item expandInven[12];
        Inven_Item saveExpand;
        Inven_Item saveEquip;
        Inven_Item slotItem1 = inven->GetInvenSlot(0, 11);
        saveEquip.setCopy(slotItem1);
        saveExpand.setCopy(m_equipSlot[1]);
        inven->GetInvenData(5, expandInven, 0x2dc);
        inven->SetInvenData(5, m_equipSlot, 0x2dc);
        SetExpandEquipslot(1, expandInven, 0x2dc);
        inven->m_invenItem[11].setCopy(saveEquip);
        m_equipSlot[1].setCopy(saveExpand);
        CParty* party = pUser->GetParty();
        if (party != 0)
        {
            party->Reset_party_overlapped_drop_ratio();
        }
        Inven_Item slotItem2 = inven->GetInvenSlot(0, 10);
        if (slotItem2.m_addInfo != 0 ||
            m_equipSlot[0].m_addInfo != 0)
        {
            ((CCharacterView*)((char*)pUser + 0x796f4))
                ->enableSaveCharacView();
            pUser->send_equip(10);
        }
        if (curChannel == 0)
        {
            pUser->setCurChannelEquipslotSwitch(1);
        }
        else if (curChannel == 1)
        {
            pUser->setCurChannelEquipslotSwitch(0);
        }
        return 0;
    }
    return 1;
}

int CExpandEquipslot::Move_Item(CUser* pUser, int type, int slot, int space,
                                int expandSlot)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            "int CExpandEquipslot::Move_Item(CUser*, int, int, int, int)",
            0x17c, "Fail Move_Item : pUser is NULL");
        return 0x11;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            "int CExpandEquipslot::Move_Item(CUser*, int, int, int, int)",
            0x182, "Fail Move_Item : pUser is not valid state");
        return 0x11;
    }
    CInventory* inven = pUser->getCurCharacInvenW();
    switch (type)
    {
    case 0:
        switch (space)
        {
        case 0x12:
        {
            if (!CheckEquipable(
                    inven->m_pEquipSlot[slot],
                    m_equipSlot[expandSlot]))
            {
                return 0x11;
            }
            if (!inven->m_pEquipSlot[slot].m_amp.isIdentified() ||
                !m_equipSlot[expandSlot].m_amp.isIdentified())
            {
                return 0x11;
            }
            Inven_Item localItem;
            localItem.setCopy(inven->m_pEquipSlot[slot]);
            if (localItem.m_addInfo != 0)
            {
                CItem* item =
                    G_CDataManager()->find_item(localItem.m_addInfo);
                if (item == 0 || pUser == 0)
                {
                    cMyTrace(
                        "int CExpandEquipslot::Move_Item(CUser*, int, int, int, int)",
                        0x1a0, 5)(
                        "CExpandEquipslot::Move_Item Move_Item return false (%d)",
                        localItem.m_addInfo);
                    return 0x11;
                }
                if (item->IsAvatarItem())
                {
                    return 0x11;
                }
                if (item->IsCreatureItem())
                {
                    return 0x11;
                }
                int nEquip = 0;
                nEquip = IsEquipable(pUser, item, expandSlot + 10);
                if (nEquip != 0)
                {
                    return nEquip;
                }
                if (item->PeriodAfterUnsealing() != 0)
                {
                    int nExpire = GlobalData::s_systemTime_.getCurSec() +
                                  item->PeriodAfterUnsealing() * 0x15180;
                    if (nExpire != 0)
                    {
                        localItem.m_fieldb =
                            (unsigned short)((nExpire - 0x44a54a80) /
                                             0x15180);
                    }
                }
            }
            inven->m_pEquipSlot[slot].setCopy(
                m_equipSlot[expandSlot]);
            localItem.m_field0 = 0;
            m_equipSlot[expandSlot].setCopy(localItem);
            return 0;
        }
        default:
            return 0x11;
        }
    default:
        return 0x11;
    }
}

int CExpandEquipslot::Change_Equip(CUser* pUser, int type, int slot,
                                   int space, int expandSlot)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            "int CExpandEquipslot::Change_Equip(CUser*, int, int, int, int)",
            0xd8, "Fail Change_Equip : pUser is NULL");
        return 1;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            "int CExpandEquipslot::Change_Equip(CUser*, int, int, int, int)",
            0xde, "Fail Change_Equip : pUser is not valid state");
        return 0x13;
    }
    CInventory* invenW = pUser->getCurCharacInvenW();
    switch (type)
    {
    case 0:
    {
        const Inven_Item* srcItem =
            pUser->getCurCharacInvenR()->GetInvenRef(1, slot);
        CItem* item = G_CDataManager()->find_item(srcItem->m_addInfo);
        switch (space)
        {
        case 0x12:
        {
            if (invenW->CheckValidSlot(1, slot) != 1)
            {
                return 1;
            }
            if (invenW->CheckValidSlot(5, expandSlot) != 1)
            {
                return 1;
            }
            if (pUser->getCurChannelEquipslotSwitch() == 0)
            {
                if (item != 0 &&
                    !item->IsEnableWorld((ENUM_WORLD_TYPE)1))
                {
                    return 1;
                }
            }
            Inven_Item slotItem1 = invenW->GetInvenSlot(1, slot);
            if (slotItem1.m_field0 != 0)
            {
                int secuRet = CSecu_ProtectionField::Check(
                    GlobalData::s_pSecuProtectionField, pUser,
                    (SECURITY_PROTCTION)0x16);
                if (secuRet != 0)
                {
                    return secuRet;
                }
            }
            if (pUser != 0)
            {
                if (pUser->CheckItemLock(1, slot))
                {
                    Inven_Item slotItem2 = invenW->GetInvenSlot(1, slot);
                    if (slotItem2.m_field0 != 0)
                    {
                        return 0xd5;
                    }
                }
            }
            int moveRet = 0;
            moveRet = Move_Item(pUser, 0, slot, 0x12, expandSlot - 10);
            if (moveRet != 0)
            {
                return moveRet;
            }
            alter();
            return 0;
        }
        default:
            return 1;
        }
    }
    default:
        return 1;
    }
}
