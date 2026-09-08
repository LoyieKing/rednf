#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <utility>
#include <vector>

#include "CBoosterGage.h"
#include "CSystemTime.h"
#include "Stream.h"

#include "CMailBoxHelper.h"   // WongWork::CMailBoxHelper 权威叶子头（连带真实 Inven_Item）
#include "NpServerXml.h"  // np_server_xml::CServerXml/g_ServerString_
#include "PacketGuard.h"
// ============================================================================
// 本 TU 最小依赖声明（CUser/CInventory/CDataManager/CItem 等属后续批次，
// 只声明本 TU 调用所需的符号形态；与 CInventory.h/GameStubs 的版本待合并）。
// ============================================================================





class CItem
{
public:
    virtual ~CItem();
    virtual void make_item(Inven_Item& item) const = 0;
    virtual int GetItemType() const = 0;
    virtual bool IsAvatarItem() const = 0;
    virtual bool IsCreatureItem() const = 0;
};

namespace WongWork
{
class CAvatarItemMgr
{
public:
    int GetRemainDate(int nAvatarUid, int nTick) const;
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

// CServerXml 类声明与 g_ServerString_ extern 见 NpServerXml.h（跨 TU 唯一声明点）；
// 全局定义（C 名 g_ServerString_）在 ServerCommon/ServerXml.cpp（ORIG B 0x093fa660）。

struct SIG_BOOSTER_GAGE_DATA
{
    unsigned int m_dwAccountNo;  // +0x00
    int m_dwBoosterGage;         // +0x04
};

// ============================================================================
// CBoosterGage
// ============================================================================

CBoosterGage::~CBoosterGage()
{
}

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

// [b28d-32 N(N78/P116): ORIG memset 内联 rep stos 慢路 vs 4.4 call（210/177） | ORIG 0x080dc6d6]
void CBoosterGage::make_postar(CUser* pUser, const Inven_Item& item,
                               const char* pAgency, int nPeriod)
{
    char szPost[0x100];
    memset(szPost, 0, sizeof(szPost));  // ORIG：memset 0x100（gcc4.4 `= {0}` 仅清 0x40）
    char szTitle[0x14];
    std::string strTitle = g_ServerString_.GetServerString(0x436, NULL);
    strncpy(szTitle, strTitle.c_str(), 0x14);
    std::string strBody = g_ServerString_.GetServerString(0x437, NULL);
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
         it != rewardList.end(); it++)
    {
        std::pair<Inven_Item, bool> item = *it;
        char szItem[0xff] = {0};
        sprintf(szItem, "%d ", item.first.m_addInfo);
        strLog += szItem;
    }
    cMyTrace(
        "void CBoosterGage::log_booster_gage_bonus_reward(CUser*, const MULTIBOX_REWARD_LIST&)",
        0x7c, 0)("BOOSTER_GAGE : REWARD ITEM (%s) x 2 : CHARAC_NO (%u)",
                 strLog.c_str(), pUser->getCurCharacNo());
}

// [b28d-32 判词·N82] ORIG 0x080dcbd6：NEAR 封顶（118/118 条），残余 = 帧差
// 引发的全帧槽号平移
// （ORIG -0x64 vs 4.4 -0x24，N82/P182 族），助记符序列全等。
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
            item.m_addInfo, nCount, bFlag, 0, -1, pAgency, eReason, 0, 0);
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
            itemList.push_back(std::make_pair(nSlot, item.GetItemSpace()));
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
            itemList.push_back(std::make_pair(nSlot, item.GetItemSpace()));
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
            itemList.push_back(std::make_pair(nSlot, item.GetItemSpace()));
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
    // [b28c-11] NEAR 封顶（52/52 条）：残余 = 虚调用 callee 寄存器 %eax↔%edx
    // 互换（`mov (%eax),%edx; …call *%edx` vs `mov (%eax),%eax; …call *%eax`），
    // N7 族（4.4 间接 call 目标恒 %eax）。
    _reset();
    SIG_BOOSTER_GAGE_DATA* p = (SIG_BOOSTER_GAGE_DATA*)pData;
    m_dwTotalGage = p->m_dwBoosterGage;
    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
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

// [b28d-32 N(EH 尾岛): ORIG 正常路径 jmp 尾岛 + 异常段居中，4.4 就地内联 epilogue（N104 族），81/80 | ORIG 0x080dd340]
bool CBoosterGage::_saveData(CUser* pUser)
{
    // [b28c-11] N 封顶（81/80）：残余 = EH 段布局——ORIG 正常路径 `jmp` 尾岛
    // （guard D1 析构置于 +0x113 共享尾）且异常段在中间；4.4 正常路径
    // fall-through 到 guard D1、异常段置尾（负例表「EH 双份 dtor epilogue vs
    // merge」）。语义等价，无源码解。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("BoosterGage.cpp", 0x193), true);
    **guard << 0x2d8;
    **guard << (int)pUser->GetUID();
    SIG_BOOSTER_GAGE_DATA* pData = guard->GetInBuffer<SIG_BOOSTER_GAGE_DATA>();
    register bool bRet;
    if (pData == NULL)
    {
        bRet = false;
    }
    else
    {
        memset(pData, 0, sizeof(SIG_BOOSTER_GAGE_DATA));
        pData->m_dwAccountNo = pUser->get_acc_id();
        pData->m_dwBoosterGage = m_dwTotalGage;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        bRet = true;
    }
    return bRet;
}

void CBoosterGage::_reset()
{
    m_dwTotalGage = 0;
}
