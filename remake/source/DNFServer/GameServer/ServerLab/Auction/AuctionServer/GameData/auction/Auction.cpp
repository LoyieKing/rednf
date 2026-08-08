// Rebuilt from df_auction_r DWARF (Auction.cpp CU), 2026-08-08
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Auction.h"
#include "AuctionDictionary.h"
#include "ServiceFactory.h"
#include "Script.h"
#include "TraceLog.h"
#include "CommonDataPool.h"
#include "Message.h"
#include "NSLDBThread.h"
#include "RDARScriptItemInfo.h"
#include "RDARScriptAvatarColorInfo.h"
#include "DNFFunctionLib.h"
#include "ObjectPoolByBoostPool.h"
#include "DBTransactionDesign.h"
#include "AuctionPacket.h"
#include "ServiceError.h"

extern char SENDER_NPC_NAME[0xff];

using namespace nsl;

struct PacketHeader
{
    unsigned short m_wMessageID;
    unsigned short m_wSize;
    unsigned short m_wSrcPort;
    unsigned short m_wDstPort;
    unsigned short m_nToken;

    PacketHeader()
    {
    }

    PacketHeader(unsigned short wMessageID, unsigned short wSize)
    {
        m_wMessageID = wMessageID;
        m_wSize = wSize;
        m_wSrcPort = 0;
        m_wDstPort = 0;
        m_nToken = 0;
    }
};

struct Packet_Monitor_Notify_Auction_Mail
{
    PacketHeader super_PacketHeader;
    unsigned int m_nCharacNo;
    unsigned int iIdByChannel;
    unsigned char m_nNotifyNo;
    unsigned long m_nItemId;
    unsigned char m_nPayType;
    RandomOption randomOption;

    Packet_Monitor_Notify_Auction_Mail()
        : super_PacketHeader(0xc1c, 0x26)
    {
    }
};

char Auction::msAuctionServerName[20] = {0};

Auction::Auction()
    : mEmblemInfoStructPool(0x20), mExpansionInfoStructPool(0x20)
{
    mMaxAuctionId = 0;
    mAUCTION_COMMISSION = 0.1;
    mAUCTION_VIP_COMMISSION = 0.05;
    mAUCTION_PRIVATE_STORE_COMISSION = 0.05;
    mAUCTION_EXPIRE_TIME = 0x15180;
    mSYSTEM_AUCTION_EXPIRE_TIME = 0x1c20;
    mAuctionDic.mpAuction = this;
    memset(mpSzBuffer, 0, 0x1000);
    mSearch.mpAuction = this;
    mSocketForMonitor.open();
    mSocketForMonitor.bind(G_Script()->findCharValue(0, 3),
                           (unsigned short)G_Script()->findIntValue(0, 5));
    G_TraceLog()->sysLog(5, "load item info");
    CodePage::initCodePage();
    if (!mItemInfo.Load("./iteminfo.dat", CodePage::script2Database))
    {
        G_TraceLog()->sysLog(7, "fail to load item info");
        exit(1);
    }
    G_TraceLog()->sysLog(5, "successfully load item info");
    G_TraceLog()->sysLog(5, "load avatar variation");
    if (!importAvatarColorVariation(&avatarColorInfo))
    {
        G_TraceLog()->sysLog(7, "fail to load avatar variation");
        exit(1);
    }
    G_TraceLog()->sysLog(5, "successfully load avatar variation");
    mAvatarDupChkMap.clear();
    mCreatureDupChkMap.clear();
    SetPayType(PAY_TYPE_GOLD);
    G_TraceLog()->sysLog(5, "Setting PayType : Gold");
}

Auction::~Auction()
{
    mSocketForMonitor.close();
    mAvatarDupChkMap.clear();
    mCreatureDupChkMap.clear();
}

int Auction::ProcessMostRecentExpireItem()
{
    int error_code = 0;
    bool one_processing;
    unsigned int cnt = 0;
    do
    {
        if (99 < cnt)
        {
            goto system_loop;
        }
        one_processing = false;
        error_code = mAuctionDic.ProcessMostRecentExpireItem(one_processing, ENUM_AUCTION_USER);
        if (error_code != 0)
        {
            if (error_code != 0x2c)
            {
                G_TraceLog()->sysLog(7,
                    "ERROR: Auction::ProcessMostRecentExpireItem(ENUM_AUCTION_USER), %s",
                    GetErrorStr(error_code));
            }
            goto system_loop;
        }
        else
        {
            if (one_processing != true)
            {
                goto system_loop;
            }
            cnt = cnt + 1;
        }
    } while (true);
system_loop:
    unsigned int cnt2 = 0;
    while (true)
    {
        if (2 < cnt2)
        {
            return error_code;
        }
        one_processing = false;
        error_code = mAuctionDic.ProcessMostRecentExpireItem(one_processing, ENUM_AUCTION_SYSTEM);
        if (error_code != 0)
        {
            break;
        }
        if (one_processing != true)
        {
            return 0;
        }
        cnt2 = cnt2 + 1;
        error_code = 0;
    }
    if (error_code == 0x2c)
    {
        return 0x2c;
    }
    G_TraceLog()->sysLog(7,
        "ERROR: Auction::ProcessMostRecentExpireItem(ENUM_AUCTION_SYSTEM), %s",
        GetErrorStr(error_code));
    return error_code;
}

CNRDItemInfoList::STItemInfo* Auction::GetItemInfo(unsigned long item_id) const
{
    return mItemInfo.GetItemInfo(item_id);
}

bool Auction::IsAvatarCategory(int category)
{
    if (((category < 15000) || (0x3dc3 < category)) &&
        ((category < 17000) || (0x4593 < category)) &&
        ((category < 19000) || (0x4d63 < category)) &&
        ((category < 21000) || (0x5533 < category)) &&
        ((category < 23000) || (0x5d03 < category)) &&
        ((category < 25000) || (0x64d3 < category)))
    {
        return false;
    }
    return true;
}

int Auction::AddAvatarEmblemInfo(int ui_id, const stAvatarEmblemInfo_t* pInfo)
{
    std::map<int, stAvatarEmblemInfo_t*>::iterator iter =
        mEmblemInfoMap.find(ui_id);
    stAvatarEmblemInfo_t* new_emblem_info;
    if (iter == mEmblemInfoMap.end())
    {
        new_emblem_info = mEmblemInfoStructPool.malloc();
        if (new_emblem_info == NULL)
        {
            return 9;
        }
        mEmblemInfoMap.insert(
            std::make_pair<const int&, stAvatarEmblemInfo_t*&>(ui_id, new_emblem_info));
    }
    else
    {
        new_emblem_info = iter->second;
    }
    memcpy(new_emblem_info, pInfo, 0x1e);
    return 0;
}

int Auction::AddAvatarExpansionInfo(int ui_id, const stAvatarExpansionInfo_t* pInfo)
{
    std::map<int, stAvatarExpansionInfo_t*>::iterator iter =
        mExpansionInfoMap.find(ui_id);
    stAvatarExpansionInfo_t* new_expansion_info;
    if (iter == mExpansionInfoMap.end())
    {
        new_expansion_info = mExpansionInfoStructPool.malloc();
        if (new_expansion_info == NULL)
        {
            return 9;
        }
        mExpansionInfoMap.insert(
            std::make_pair<const int&, stAvatarExpansionInfo_t*&>(ui_id, new_expansion_info));
    }
    else
    {
        new_expansion_info = iter->second;
    }
    memcpy(new_expansion_info, pInfo, 4);
    return 0;
}

int Auction::SubAvatarEmblemInfo(int ui_id)
{
    std::map<int, stAvatarEmblemInfo_t*>::iterator iter =
        mEmblemInfoMap.find(ui_id);
    bool bVar1 = iter != mEmblemInfoMap.end();
    if (!bVar1)
    {
        G_TraceLog()->sysLog(7, "ERROR, Try to SubAvatarEmblemInfo(), ui_id = %d", ui_id);
    }
    else
    {
        mEmblemInfoStructPool.free(iter->second);
        mEmblemInfoMap.erase(iter);
    }
    return !bVar1;
}

int Auction::SubAvatarExpansionInfo(int ui_id)
{
    std::map<int, stAvatarExpansionInfo_t*>::iterator iter =
        mExpansionInfoMap.find(ui_id);
    bool bVar1 = iter != mExpansionInfoMap.end();
    if (!bVar1)
    {
        G_TraceLog()->sysLog(7, "ERROR, Try to SubAvatarEmblemInfo(), ui_id = %d", ui_id);
    }
    else
    {
        mExpansionInfoStructPool.free(iter->second);
        mExpansionInfoMap.erase(iter);
    }
    return !bVar1;
}

int Auction::GetAvatarEmblemInfo(int ui_id, stAvatarEmblemInfo_t* pInfo)
{
    std::map<int, stAvatarEmblemInfo_t*>::iterator iter =
        mEmblemInfoMap.find(ui_id);
    bool bVar1 = iter != mEmblemInfoMap.end();
    if (!bVar1)
    {
        G_TraceLog()->sysLog(7, "ERROR, Try to GetAvatarEmblemInfo(), ui_id = %d", ui_id);
    }
    else
    {
        memcpy(pInfo, iter->second, 0x1e);
    }
    return !bVar1;
}

int Auction::GetAvatarExpansionInfo(int ui_id, stAvatarExpansionInfo_t* pInfo)
{
    std::map<int, stAvatarExpansionInfo_t*>::iterator iter =
        mExpansionInfoMap.find(ui_id);
    bool bVar1 = iter != mExpansionInfoMap.end();
    if (!bVar1)
    {
        G_TraceLog()->sysLog(7, "ERROR, Try to GetAvatarExpansionInfo(), ui_id = %d", ui_id);
    }
    else
    {
        memcpy(pInfo, iter->second, 4);
    }
    return !bVar1;
}

void Auction::SetMaxHistoryAuctionId(unsigned long long auctionId)
{
    if (mMaxAuctionId < auctionId)
    {
        mMaxAuctionId = auctionId;
        G_TraceLog()->sysLog(0, "SetMaxHistoryAuctionId() triggered!!! au_id : %llu",
                             mMaxAuctionId);
    }
}

void Auction::SendMessageToMonitor(unsigned int characId, unsigned char messageId,
                                   unsigned long itemId, const RandomOption& _roi_option)
{
    G_TraceLog()->sysLog(8, "SendMessageToMonitor(), characId(%u), messageId(%hhu), itemId(%u)",
                         characId, (unsigned int)messageId, itemId);
    Packet_Monitor_Notify_Auction_Mail packet;
    packet.m_nCharacNo = characId;
    packet.iIdByChannel = 0;
    packet.m_nItemId = itemId;
    packet.m_nNotifyNo = messageId;
    packet.m_nPayType = (unsigned char)GetPayType();
    packet.randomOption = _roi_option;
    mSocketForMonitor.send((char*)&packet, 0x26,
                           (unsigned short)G_Script()->findIntValue(1, 2),
                           G_Script()->findCharValue(1, 1));
}

int Auction::RegistItem(int ownerId, const char* ownerName, char ownerType,
                        int price, int instantPrice, unsigned int black_point, int unitPrice,
                        const ROI_Category& roiKey, char* ownerNexonId, DnfItemInfo itemInfo,
                        unsigned long long& auctionId, long expireTime, int buyerId,
                        const char* buyerName, bool isLoad)
{
    int return_code = 0;
    int now_registed_item_num = 0;
    CNRDItemInfoList::STItemInfo* pItemInfo = GetItemInfo(itemInfo.item_id);
    if (pItemInfo == NULL)
    {
        return 0;
    }
    bool db_loaded = isLoad;
    if ((GetPayType() != PAY_TYPE_POINT) || (db_loaded))
    {
        db_loaded = false;
    }
    else
    {
        db_loaded = true;
    }
    if (db_loaded)
    {
        ROI_AverageKey roiAverageKey;
        char itemRefineValue = 0;
        int AvePrice = 0;
        unsigned char itemUpgradeValue = itemInfo.GetUpgradeValue();
        unsigned long itemId = itemInfo.GetItemId();
        GetAveragePrice(itemId, itemUpgradeValue, roiAverageKey, '\0', &AvePrice);
        if (((AvePrice != -1) && (AvePrice != 0)) &&
            (((double)instantPrice / (double)AvePrice) < 0.5))
        {
            return 0x37;
        }
    }
    EnumAuctionItemType item_type = CheckItemType(itemInfo.item_id);
    char is_available_item = '\x01';
    if (item_type == AUCTION_ITEM_TYPE_CREATURE)
    {
        is_available_item = (char)RegistChkMapForAvatarCreature(false, itemInfo.add_info);
    }
    else if (item_type == AUCTION_ITEM_TYPE_AVATAR)
    {
        is_available_item = (char)RegistChkMapForAvatarCreature(true, itemInfo.add_info);
    }
    else if (item_type != AUCTION_ITEM_TYPE_PLAIN)
    {
        is_available_item = '\0';
        G_TraceLog()->sysLog(7, "ERROR: Auction::RegistItem(), item_type check unknown, item_id : %d",
                             itemInfo.item_id);
    }
    if (is_available_item == '\x01')
    {
        now_registed_item_num = GetNowRegistedItemNum(ownerId);
        if (ownerType == '\x01')
        {
            if (0x1d < now_registed_item_num)
            {
                return 0x1e;
            }
        }
        else if (9 < now_registed_item_num)
        {
            return 0x1e;
        }
        char db_loaded_auction_id = '\0';
        if ((auctionId == 0))
        {
            auctionId = getNextAuctionId();
        }
        else
        {
            db_loaded_auction_id = '\x01';
            checkMaxAuctionId(auctionId);
        }
        if (expireTime == 0)
        {
            time(&expireTime);
            if (ownerId == -1)
            {
                expireTime = mSYSTEM_AUCTION_EXPIRE_TIME + expireTime;
            }
            else
            {
                if ((strlen(ownerName) == 0xc) &&
                    (strcmp(ownerName, "\xb0\xe6\xb8\xc5\xb7\xce\xc0\xfa\xb7\xb9\xba\xf3") == 0))
                {
                    expireTime = mSYSTEM_AUCTION_EXPIRE_TIME + expireTime;
                    ownerId = -1;
                }
                else
                {
                    expireTime = mAUCTION_EXPIRE_TIME + expireTime;
                }
            }
        }
        return_code = mAuctionDic.RegistItem(
            auctionId, ownerId, ownerName, price, instantPrice, black_point, unitPrice,
            itemInfo, expireTime, buyerId, buyerName, roiKey, ownerType, ownerNexonId);
        if (return_code == 0)
        {
            if (db_loaded_auction_id != '\x01')
            {
                PrintDnfItemInfo(itemInfo, mpSzBuffer);
                G_TraceLog()->sysLog(
                    5, "AuctionDictionary::RegistItem(), before DB insert, o_id : %d , item : %s",
                    ownerId, mpSzBuffer);
                tagAUCTION_DB_REGIST_ITEM dbtr_regist_item;
                dbtr_regist_item.auction_id = auctionId;
                dbtr_regist_item.expire_time = expireTime;
                dbtr_regist_item.owner_id = ownerId;
                if (ownerId == -1)
                {
                    strncpy(dbtr_regist_item.owner_name, SENDER_NPC_NAME, 0xd);
                }
                else
                {
                    strncpy(dbtr_regist_item.owner_name, ownerName, 0xc);
                }
                dbtr_regist_item.buyer_id = -1;
                dbtr_regist_item.buyer_name[0] = '\0';
                dbtr_regist_item.price = price;
                dbtr_regist_item.instant_price = instantPrice;
                dbtr_regist_item.item_info = itemInfo;
                dbtr_regist_item.black_point = black_point;
                dbtr_regist_item.unit_price = unitPrice;
                dbtr_regist_item._reg_roi_category_key = roiKey;
                dbtr_regist_item.owner_type = ownerType;
                strncpy(dbtr_regist_item.owner_nexon_id, ownerNexonId,
                        strlen(ownerNexonId));
                CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(tlsThreadId);
                Message* pMsg = pPool->createMessage(3);
                CMsgCell* pNewCell = pMsg->getCellFromMessage();
                *pNewCell << &dbtr_regist_item;
                pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
            }
            return_code = 0;
        }
    }
    else
    {
        return_code = 0x23;
    }
    return return_code;
}

int Auction::AddItemAveragePrice(unsigned long item_id, unsigned char upgrade,
                                 int average_price, unsigned int purchase_cnt,
                                 const ROI_AverageKey& roiKey, unsigned char refine, bool bFlag)
{
    int return_code = 0;
    bool first_add = false;
    int average = 0;
    CNRDItemInfoList::STItemInfo* pItemInfo = GetItemInfo(item_id);
    if (pItemInfo != NULL)
    {
        bool is_stack = IsStackableCategory(pItemInfo->category_);
        return_code = mAuctionDic.AddItemAveragePrice(
            item_id, upgrade, average_price, first_add, average, is_stack, roiKey,
            purchase_cnt, refine);
    }
    if (return_code == 0)
    {
        if (!bFlag)
        {
            if (first_add == false)
            {
                tagAUCTION_DB_UPDATE_AVERAGE_PRICE dbtr_update_average_price;
                dbtr_update_average_price.item_id = item_id;
                dbtr_update_average_price.average_price = average;
                dbtr_update_average_price._roi_average_key = roiKey;
                dbtr_update_average_price.purchase_cnt = purchase_cnt;
                dbtr_update_average_price.upgrade = upgrade;
                dbtr_update_average_price.seperate_upgrade = refine;
                CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(tlsThreadId);
                Message* pMsg = pPool->createMessage(3);
                CMsgCell* pNewCell = pMsg->getCellFromMessage();
                *pNewCell << &dbtr_update_average_price;
                pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
            }
            else
            {
                tagAUCTION_DB_INSERT_AVERAGE_PRICE dbtr_insert_average_price;
                dbtr_insert_average_price.item_id = item_id;
                dbtr_insert_average_price.average_price = average;
                dbtr_insert_average_price._roi_average_key = roiKey;
                dbtr_insert_average_price.upgrade = upgrade;
                dbtr_insert_average_price.seperate_upgrade = refine;
                CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(tlsThreadId);
                Message* pMsg = pPool->createMessage(3);
                CMsgCell* pNewCell = pMsg->getCellFromMessage();
                *pNewCell << &dbtr_insert_average_price;
                pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
            }
        }
        return_code = 0;
    }
    else
    {
        G_TraceLog()->sysLog(7, "ERROR: Auction::AddItemAveragePrice(), AddItemAveragePrice, %s",
                             GetErrorStr(return_code));
    }
    return return_code;
}

void Auction::Set_ROI_Constraint(const ROI_Average_Constraint& constraint)
{
    mAuctionDic.dic_Set_ROI_Constraint(constraint);
}

int Auction::TransErrToReason(int err_no)
{
    int reason;
    switch (err_no)
    {
    case 0x23:
        reason = 0x91;
        break;
    case 0x24:
        reason = 0x96;
        break;
    case 0x1f:
    case 0x26:
        reason = 0x98;
        break;
    case 0x2b:
        reason = 0x9a;
        break;
    case 0x2d:
        reason = 0x9b;
        break;
    case 0x2e:
        reason = 0x9c;
        break;
    case 0x2f:
        reason = 0x9d;
        break;
    case 0x31:
        reason = 0x9e;
        break;
    case 0x37:
        reason = 0x76;
        break;
    default:
        reason = 0x99;
        break;
    }
    return reason;
}

int Auction::GetAveragePrice(unsigned long item_id, unsigned char upgrade,
                             const ROI_AverageKey& roiKey, unsigned char refine, int* out)
{
    int error_no = mAuctionDic.GetAveragePrice(item_id, upgrade, roiKey, refine, out);
    if (error_no != 0)
    {
        G_TraceLog()->sysLog(7, "ERROR: Auction::GetAveragePrice(), GetItemAveragePrice, %s",
                             GetErrorStr(error_no));
    }
    return error_no;
}

int Auction::SearchByItemId(PSearchByItemId pSearch, unsigned long* pItemIdArray,
                            unsigned int* pTotalNumberOfFound, unsigned short* pNumberOfFound,
                            AuctionItemInfo* pOutAuctionItemInfoArray)
{
    int error_no = mSearch.FindByItem(
        pSearch, pItemIdArray, pTotalNumberOfFound, pNumberOfFound,
        pOutAuctionItemInfoArray);
    if (error_no != 0)
    {
        G_TraceLog()->sysLog(7, "ERROR: %s, %s", "SearchByItemId", GetErrorStr(error_no));
    }
    return error_no;
}

int Auction::SearchByCategory(PSearchByCategory pSearch, unsigned int* pTotalNumberOfFound,
                              unsigned short* pNumberOfFound,
                              AuctionItemInfo* pOutAuctionItemInfoArray)
{
    G_TraceLog()->sysLog(
        8, "Auction::SearchByCategory, category:%d, rarity:%d, upgrade:%d~%d, lv:%d~%d, start_idx:%d",
        (unsigned int)pSearch->category, (unsigned int)pSearch->rarity,
        (unsigned int)pSearch->upgradeStart, (unsigned int)pSearch->upgradeEnd,
        (unsigned int)pSearch->lvStart, (unsigned int)pSearch->lvEnd,
        pSearch->startIndex);
    int error_no = mSearch.FindByCategory(
        pSearch, pTotalNumberOfFound, pNumberOfFound, pOutAuctionItemInfoArray);
    if (error_no != 0)
    {
        G_TraceLog()->sysLog(7, "ERROR: %s, %s", "SearchByCategory", GetErrorStr(error_no));
    }
    return error_no;
}

int Auction::RegistCancel(int owner_id, unsigned long long auction_id)
{
    int error_no = mAuctionDic.RegistCancel(owner_id, auction_id);
    if (error_no != 0)
    {
        G_TraceLog()->sysLog(5, "Auction::RegistCancel(), RegistCancel, %s",
                             GetErrorStr(error_no));
    }
    return error_no;
}

int Auction::OpenPrivateStore(unsigned int m_id, int character_num, int* character_ids)
{
    std::map<unsigned int, std::vector<int> >::iterator iter = mIdMap.find(m_id);
    if (iter != mIdMap.end())
    {
        std::vector<int>& cIdListPrevious = iter->second;
        int size = (int)cIdListPrevious.size();
        for (int i = 0; i < size; i = i + 1)
        {
            int cId = cIdListPrevious[i];
            mCidSet.erase(cId);
        }
    }
    std::vector<int> cIdList;
    for (int i = 0; i < character_num; i = i + 1)
    {
        cIdList.push_back(character_ids[i]);
        mCidSet.insert(character_ids[i]);
    }
    mIdMap[m_id] = cIdList;
    return 0;
}

int Auction::GetMyRegistedItemInfo(int owner_id, int* pNum, MyRegistedItemInfo* pOut)
{
    int error_no = mAuctionDic.GetRegistedItemInfo(owner_id, pNum, pOut);
    if (error_no != 0)
    {
        G_TraceLog()->sysLog(5, "Auction::GetMyRegistedItemInfo(), %s", GetErrorStr(error_no));
    }
    return error_no;
}

int Auction::GetMyBiddingInfo(int buyer_id, int* pNum, MyBiddingItemInfo* pOut)
{
    int error_no = mAuctionDic.GetBiddingInfo(buyer_id, pNum, pOut);
    if (error_no != 0)
    {
        G_TraceLog()->sysLog(5, "Auction::GetMyBiddingInfo(), %s", GetErrorStr(error_no));
    }
    return error_no;
}

int Auction::ClosePrivateStore(unsigned int m_id)
{
    std::map<unsigned int, std::vector<int> >::iterator iter = mIdMap.find(m_id);
    if (iter == mIdMap.end())
    {
        G_TraceLog()->sysLog(7, "ERROR: Auction::ClosePrivateStore(), %s, mid:%s",
                             GetErrorStr(0x30), NumberToString(m_id, 0));
    }
    else
    {
        std::vector<int>& cIdList = iter->second;
        int size = (int)cIdList.size();
        for (int i = 0; i < size; i = i + 1)
        {
            int cId = cIdList[i];
            mCidSet.erase(cId);
        }
    }
    return 0;
}

int Auction::BuyItemApiece(unsigned long long auction_id, int buyer_id, int price, int count)
{
    int return_code = mAuctionDic.BuyItemApiece(auction_id, buyer_id, price, count);
    if (return_code != 0)
    {
        G_TraceLog()->sysLog(5, "Auction::BuyItemApiece(), %s", GetErrorStr(return_code));
    }
    return return_code;
}

int Auction::Bidding(int buyer_id, const char* buyer_name, unsigned long long auction_id,
                     int price, char* out, int& ret)
{
    int return_code = 0;
    if (price < 1)
    {
        G_TraceLog()->sysLog(5, "Auction::Bidding(), price, %s", GetErrorStr(0));
        return_code = 0x25;
    }
    else
    {
        return_code = mAuctionDic.Bidding(
            auction_id, buyer_id, buyer_name, price, out, ret);
        if (return_code != 0)
        {
            G_TraceLog()->sysLog(5, "Auction::Bidding(), Bidding, %s",
                                 GetErrorStr(return_code));
        }
    }
    return return_code;
}

int Auction::IsOwnerVIP(unsigned long long auction_id, OwnerInfo& ownerInfo)
{
    return mAuctionDic.GetOwnerIsVIP(auction_id, ownerInfo) != 0;
}

char* Auction::GetAvatarColorName(int color_key)
{
    stAvatarExpansionInfo_t expansionInfo;
    int iVar2 = GetAvatarExpansionInfo(color_key, &expansionInfo);
    if (iVar2 == 1)
    {
        return "";
    }
    if ((expansionInfo.color[0] < 0) || (0x1ff < expansionInfo.color[0]))
    {
        return "";
    }
    std::map<int, std::string>::const_iterator itr =
        avatarColorInfo.avatarColorNameMap.find(expansionInfo.color[0]);
    if (itr != avatarColorInfo.avatarColorNameMap.end())
    {
        return (char*)itr->second.c_str();
    }
    return "";
}

EnumAuctionItemType Auction::CheckItemType(unsigned long item_id)
{
    if (item_id == 0)
    {
        return AUCTION_ITEM_TYPE_UNKNOWN;
    }
    CNRDItemInfoList::STItemInfo* pSVar2 = GetItemInfo(item_id);
    unsigned int category = (unsigned int)pSVar2->category_;
    if (IsAvatarCategory(category))
    {
        return AUCTION_ITEM_TYPE_AVATAR;
    }
    if ((category < 0x36b1) || (0x36b4 < category))
    {
        return AUCTION_ITEM_TYPE_PLAIN;
    }
    return AUCTION_ITEM_TYPE_CREATURE;
}

void Auction::UnregistChkMapForAvatarCreature(bool isAvatar, int ui_id)
{
    if (isAvatar)
    {
        mAvatarDupChkMap.erase(ui_id);
    }
    else
    {
        mCreatureDupChkMap.erase(ui_id);
    }
}

bool Auction::RegistChkMapForAvatarCreature(bool isAvatar, int ui_id)
{
    std::map<int, char>::iterator iter;
    if (isAvatar)
    {
        iter = mCreatureDupChkMap.find(ui_id);
        if (!(iter == mCreatureDupChkMap.end()))
        {
            return false;
        }
        mCreatureDupChkMap[ui_id] = 'C';
    }
    else
    {
        iter = mAvatarDupChkMap.find(ui_id);
        if (!(iter == mAvatarDupChkMap.end()))
        {
            return false;
        }
        mAvatarDupChkMap[ui_id] = 'A';
    }
    return true;
}

bool Auction::isEmblemAvatar(int category)
{
    if (((category < 19000) || (0x4d63 < category)) &&
        ((category < 21000) || (0x5533 < category)) &&
        ((category < 23000) || (0x5d03 < category)) &&
        ((category < 25000) || (0x64d3 < category)))
    {
        return false;
    }
    return true;
}

bool Auction::isValidEmblemAvatar(int category, avatarEmblemType_t emblemSocketType_)
{
    if (emblemSocketType_ != 0)
    {
        switch (category % 10)
        {
        case 2:
        case 3:
            if (emblemSocketType_ != 1)
            {
                return false;
            }
            break;
        case 4:
        case 8:
            if (emblemSocketType_ != 2)
            {
                return false;
            }
            break;
        case 5:
        case 6:
            if ((emblemSocketType_ != 4) && (emblemSocketType_ != 0x10))
            {
                return false;
            }
            break;
        case 7:
        case 9:
            if (emblemSocketType_ != 8)
            {
                return false;
            }
            break;
        default:
            return false;
        }
    }
    return true;
}

bool Auction::IsPrivateStoreOpen(int cId)
{
    std::set<int>::const_iterator iter;
    iter = mCidSet.find(cId);
    bool bVar1 = iter == mCidSet.end();
    return !bVar1;
}

int Auction::GetNowRegistedItemNum(int owner_id)
{
    return mAuctionDic.GetNowRegistedItemNum(owner_id);
}

char* Auction::GetAuctionServerName()
{
    return msAuctionServerName;
}

unsigned long long Auction::getNextAuctionId()
{
    return ++mMaxAuctionId;
}

void Auction::checkMaxAuctionId(unsigned long long auctionId)
{
    if (mMaxAuctionId < auctionId)
    {
        mMaxAuctionId = auctionId;
    }
}

AuctionDictionary::AuctionDictionaryData* Auction::GetAuctionDicData(unsigned long item_id) const
{
    return mAuctionDic.GetAuctionDicData(item_id);
}

void PrintDnfItemInfo(DnfItemInfo& itemInfo, char* out)
{
    sprintf(out, "s:%d, id:%d,up:%d,sc:%d,add:%d,en:%d,ex:%d",
            (unsigned int)itemInfo.seal, itemInfo.item_id,
            (unsigned int)(itemInfo.uniItemAttr & 0x1f),
            (unsigned int)(itemInfo.uniItemAttr >> 5),
            itemInfo.add_info, (unsigned int)itemInfo.endurance, itemInfo.extendInfo);
}
