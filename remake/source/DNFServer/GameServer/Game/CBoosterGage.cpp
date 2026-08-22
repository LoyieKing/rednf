#include <iostream>

#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <utility>
#include <vector>

#include "CBoosterGage.h"
#include "CSystemTime.h"
#include "Stream.h"

// ============================================================================
// 本 TU 最小依赖声明（CUser/CInventory/CDataManager/CItem 等属后续批次，
// 只声明本 TU 调用所需的符号形态；与 CInventory.h/GameStubs 的版本待合并）。
// ============================================================================

#pragma pack(push, 1)

class Inven_Item
{
public:
    int GetItemSpace() const;

    char m_field0;        // +0x00
    unsigned char m_field1;  // +0x01（ITEM_TYPE）
    int m_addInfo;        // +0x02（item idx）
    unsigned char m_attr; // +0x06
    int m_addInfo2;       // +0x07（avatar uid / add_info）
    char m_rest[0x3d - 0x0b];  // +0x0b..+0x3c
};

#pragma pack(pop)

class InterfacePacketBuf
{
public:
    InterfacePacketBuf();
    ~InterfacePacketBuf();
    void clear();
    bool put_header(int nPacketId, int nPacketSize);
    bool put_byte(int nValue);
    bool put_short(int nValue);
    bool finalize(bool bEncryptRequired);

private:
    char m_pad[4];
};

class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

class CItem
{
public:
    virtual ~CItem();
    virtual void make_item(Inven_Item& item) const = 0;
    virtual int GetItemType() const = 0;
    virtual bool IsAvatarItem() const = 0;
    virtual bool IsCreatureItem() const = 0;
};

#ifndef DNF_ENUM_SERVER_GROUP_DEFINED
#define DNF_ENUM_SERVER_GROUP_DEFINED
enum ENUM_SERVER_GROUP { E_SERVER_GROUP_NONE = 0 };
#endif
namespace WongWork
{
class CAvatarItemMgr
{
public:
    int GetRemainDate(int nAvatarUid, int nTick) const;
};

class CMailBoxHelper
{
public:
    static void ReqDBSendNewAvatarMail(CUser* pUser, int nCharacNo, int nItemIdx,
                                       int nExpireDate, char cA, char cB,
                                       const char* pAgency, const char* pTitle, int nTitleLen);
    static void ReqDBSendNewCreatureMail(CUser* pUser, int nCharacNo,
                                         const Inven_Item& item, const char* pTitle,
                                         int nTitleLen);
    static void ReqDBSendNewSystemMail(const char* pSender, const Inven_Item& item,
                                       unsigned int nA, unsigned int nCharacNo,
                                       const char* pTitle, int nTitleLen,
                                       unsigned int nB, ENUM_SERVER_GROUP nServerGroup,
                                       bool bA, bool bB);
};
}

namespace user_creature
{
class CCreatureMgr
{
public:
    void InsertCreatureItem(Inven_Item* pItem, int nSlot, int nA, int nB, int nC);
};
}

class CInventory
{
public:
    int AddAvatarItem(int nItemIdx, int nCount, bool bA, char cB, char cC,
                      const char* pAgency, eAvatarItemAddReason eReason,
                      bool bD, unsigned short nE);
    int insertItemIntoInventory(Inven_Item item, eItemAddReason eReason,
                                bool bLog, bool bParam);
    Inven_Item GetInvenSlot(int nInvenType, int nSlot) const;
    const WongWork::CAvatarItemMgr* GetAvatarItemMgrR() const;
    user_creature::CCreatureMgr* GetCreatureMgrW();
};

class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenW();
    const CInventory* getCurCharacInvenR() const;
    int getCurCharacNo() const;
};

class CUser : public CUserCharacInfo
{
public:
    unsigned int GetUID() const;
    unsigned int get_acc_id() const;
    int GetAccountLastPlayTime();
    char GetServerGroup() const;
    CBoosterGage* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE eType) const;
    void Send(PacketGuard& packetGuard);
};

class CDataManager
{
public:
    CItem* find_item(int nItemIdx) const;

    char m_pad0[0x8770];
    BoosterGageData m_boosterGage;  // +0x8770
};

extern CDataManager* G_CDataManager();

class CEnvironment
{
public:
    char m_pad[0x37c];
    int m_nDailyScheduleTime;  // +0x37c
};

extern CEnvironment* G_CEnvironment();
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime);

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
extern StreamPool* s_stream_pool;
extern MsgQueueMgr* s_msgq_mgr;
extern CSystemTime s_systemTime_;
}

namespace np_server_xml
{
class CServerXml
{
public:
    std::string GetServerString(int nIndex, bool* pFlag) const;
};

CServerXml g_ServerString_;  // ORIG 全局（0x93fa660，np_server_xml::g_ServerString_），原 GameStubs.cpp 定义迁移
}

class cMyTrace
{
public:
    cMyTrace(const char* pName, int nLine, int nFlag);
    void operator()(const char* pFmt, ...);
};

struct SIG_BOOSTER_GAGE_DATA
{
    unsigned int m_dwAccountNo;  // +0x00
    int m_dwBoosterGage;         // +0x04
};

// ============================================================================
// CBoosterGage
// ============================================================================

void CBoosterGage::inc_total_gage(int nGage)
{
    alter();
    m_dwTotalGage += nGage;
}

void CBoosterGage::sub_total_gage(int nGage)
{
    alter();
    m_dwTotalGage -= nGage;
    if (m_dwTotalGage < 0)
        m_dwTotalGage = 0;
}

int CBoosterGage::get_booster_point(unsigned long dwItemIdx)
{
    unsigned char cRet = 0;
    cRet = G_CDataManager()->m_boosterGage.getBoosterPoint(dwItemIdx);
    if (cRet == 0)
        return 0;
    return cRet;
}

void CBoosterGage::make_postar(CUser* pUser, const Inven_Item& item,
                               const char* pAgency, int nPeriod)
{
    char szPost[0x100] = {0};
    char szTitle[0x14];
    std::string strTitle = np_server_xml::g_ServerString_.GetServerString(0x436, NULL);
    strncpy(szTitle, strTitle.c_str(), 0x14);
    std::string strBody = np_server_xml::g_ServerString_.GetServerString(0x437, NULL);
    strncpy(szPost, strBody.c_str(), 0xff);

    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem != NULL)
    {
        if (pItem->IsAvatarItem())
        {
            int nExpireDate = 0;
            if (nPeriod != 0 && nPeriod != 0x16d)
                nExpireDate = GlobalData::s_systemTime_.getCurSec() + nPeriod * 0x15180;
            WongWork::CMailBoxHelper::ReqDBSendNewAvatarMail(
                pUser, pUser->getCurCharacNo(), item.m_addInfo, nExpireDate,
                0, -1, pAgency, szPost, (int)strlen(szPost));
        }
        else if (pItem->IsCreatureItem())
        {
            WongWork::CMailBoxHelper::ReqDBSendNewCreatureMail(
                pUser, pUser->getCurCharacNo(), item, szPost, (int)strlen(szPost));
        }
        else
        {
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                szTitle, item, 0, pUser->getCurCharacNo(), szPost,
                (int)strlen(szPost), 0, (ENUM_SERVER_GROUP)pUser->GetServerGroup(), 0, 0);
        }
    }
}

void CBoosterGage::log_booster_gage_bonus_reward(
    CUser* pUser, const std::vector<std::pair<Inven_Item, bool> >& rewardList)
{
    std::string strLog;
    strLog.clear();
    for (std::vector<std::pair<Inven_Item, bool> >::const_iterator it = rewardList.begin();
         it != rewardList.end(); ++it)
    {
        std::pair<Inven_Item, bool> item = *it;
        char szItem[0xff] = {0};
        sprintf(szItem, "%d ", item.first.m_addInfo);
        strLog += szItem;
    }
    cMyTrace trace(
        "void CBoosterGage::log_booster_gage_bonus_reward(CUser*, const MULTIBOX_REWARD_LIST&)",
        0x7c, 0);
    trace("BOOSTER_GAGE : REWARD ITEM (%s) x 2 : CHARAC_NO (%u)",
          strLog.c_str(), pUser->getCurCharacNo());
}

int CBoosterGage::AddAvatarItem(std::vector<std::pair<int, int> >& itemList,
                                bool bIsFirst, CUser* pUser, const Inven_Item& item,
                                int nCount, bool bFlag, char cType, char cParam,
                                const char* pAgency, eAvatarItemAddReason eReason,
                                int& nRemainDate)
{
    int nSlot = 0;
    int nLoop = 1;
    if (bIsFirst)
    {
        nLoop = G_CDataManager()->m_boosterGage.m_cMultiboxCount;
        eReason = eAvatarItemAddReason_0x16;
    }
    for (int i = 0; i < nLoop; ++i)
    {
        nSlot = pUser->getCurCharacInvenW()->AddAvatarItem(
            item.m_addInfo, nCount, cType, 0, -1, pAgency, eReason, 0, 0);
        if (nSlot == -1)
        {
            make_postar(pUser, item, pAgency, nCount);
        }
        else
        {
            time_t tNow = time(0);
            Inven_Item invenItem = pUser->getCurCharacInvenR()->GetInvenSlot(2, nSlot);
            nRemainDate = pUser->getCurCharacInvenR()->GetAvatarItemMgrR()
                              ->GetRemainDate(invenItem.m_addInfo2, (int)tNow);
            int nItemSpace = item.GetItemSpace();
            itemList.push_back(std::make_pair(nSlot, nItemSpace));
        }
    }
    return nSlot;
}

int CBoosterGage::InsertCreatureItem(std::vector<std::pair<int, int> >& itemList,
                                     bool bIsFirst, CUser* pUser,
                                     const Inven_Item& item, eItemAddReason eReason,
                                     bool bLog, bool bParam, int nA, int nB,
                                     const char* pMailTitle)
{
    int nSlot = 0;
    int nLoop = 1;
    if (bIsFirst)
    {
        nLoop = G_CDataManager()->m_boosterGage.m_cMultiboxCount;
        eReason = eItemAddReason_0x3a;
    }
    for (int i = 0; i < nLoop; ++i)
    {
        nSlot = pUser->getCurCharacInvenW()->insertItemIntoInventory(
            item, eReason, bLog, bParam);
        if (nSlot < 0)
        {
            nSlot = -1;
            make_postar(pUser, item, pMailTitle, 0);
        }
        else
        {
            pUser->getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItem(
                (Inven_Item*)&item, nSlot, eReason, nA, nB);
            int nItemSpace = item.GetItemSpace();
            itemList.push_back(std::make_pair(nSlot, nItemSpace));
        }
    }
    return nSlot;
}

int CBoosterGage::insertItemIntoInventory(std::vector<std::pair<int, int> >& itemList,
                                          bool bIsFirst, CUser* pUser,
                                          const Inven_Item& item,
                                          eItemAddReason eReason, bool bLog)
{
    int nSlot = 0;
    int nLoop = 1;
    if (bIsFirst)
    {
        nLoop = G_CDataManager()->m_boosterGage.m_cMultiboxCount;
        eReason = eItemAddReason_0x3a;
    }
    for (int i = 0; i < nLoop; ++i)
    {
        nSlot = pUser->getCurCharacInvenW()->insertItemIntoInventory(
            item, eReason, bLog, true);
        if (nSlot < 0)
        {
            nSlot = -1;
            make_postar(pUser, item, NULL, 0);
        }
        else
        {
            int nItemSpace = item.GetItemSpace();
            itemList.push_back(std::make_pair(nSlot, nItemSpace));
        }
    }
    return nSlot;
}

unsigned char CBoosterGage::check_max_booster_gage(unsigned long dwItemIdx)
{
    int nRet = get_booster_point(dwItemIdx);
    if (nRet == 0)
        return 0;
    if (m_dwTotalGage >= G_CDataManager()->m_boosterGage.m_cMaxBoosterGage)
        return 1;
    return 0;
}

int CBoosterGage::cal_booster_gage(unsigned long dwItemIdx, CUser* pUser)
{
    int nResult;
    return 0;
}

void CBoosterGage::send_error_state_packet(CUser* pUser, unsigned long dwItemIdx)
{
    unsigned char cResult = pUser->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_17)
                                ->check_max_booster_gage(dwItemIdx);
    int nMaxCount = 2;
    if (cResult != 0)
        nMaxCount = G_CDataManager()->m_boosterGage.m_cMultiboxCount * nMaxCount;
    PacketGuard packetGuard;
    packetGuard.put_header(1, 0xd3);
    packetGuard.put_byte(0);
    packetGuard.put_byte(0x13);
    packetGuard.put_byte(nMaxCount);
    packetGuard.finalize(true);
    pUser->Send(packetGuard);
}

void CBoosterGage::send_data(CUser* pUser, int nParam)
{
    int nUnused;
}

void CBoosterGage::getData(char* pData) const
{
    SIG_BOOSTER_GAGE_DATA* pData2 = (SIG_BOOSTER_GAGE_DATA*)pData;
    memset(pData2, 0, sizeof(SIG_BOOSTER_GAGE_DATA));
    pData2->m_dwBoosterGage = m_dwTotalGage;
}

bool CBoosterGage::loadData(CUser* pUser, char* pData)
{
    _reset();
    SIG_BOOSTER_GAGE_DATA* p = (SIG_BOOSTER_GAGE_DATA*)pData;
    m_dwTotalGage = p->m_dwBoosterGage;
    if (CheckDailyScheduleTime(G_CEnvironment()->m_nDailyScheduleTime,
                               pUser->GetAccountLastPlayTime(),
                               GlobalData::s_systemTime_.getCurSec()))
    {
        ResetDaily();
        send_data(pUser, 0);
    }
    return true;
}

void CBoosterGage::ResetDaily()
{
    alter();
    m_dwTotalGage = 0;
}

bool CBoosterGage::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("BoosterGage.cpp", 0x193), true);
    **guard << 0x2d8;
    **guard << (int)pUser->GetUID();
    SIG_BOOSTER_GAGE_DATA* pData = guard->GetInBuffer<SIG_BOOSTER_GAGE_DATA>();
    bool bRet = (pData != NULL);
    if (bRet)
    {
        memset(pData, 0, sizeof(SIG_BOOSTER_GAGE_DATA));
        pData->m_dwAccountNo = pUser->get_acc_id();
        pData->m_dwBoosterGage = m_dwTotalGage;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return bRet;
}

void CBoosterGage::_reset()
{
    m_dwTotalGage = 0;
}
