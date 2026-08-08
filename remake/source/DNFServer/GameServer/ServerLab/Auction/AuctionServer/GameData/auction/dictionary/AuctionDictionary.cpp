// Rebuilt from df_auction_r DWARF (AuctionDictionary.cpp CU), 2026-08-08
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "AuctionDictionary.h"
#include "Auction.h"
#include "TraceLog.h"
#include "Script.h"
#include "Thread.h"
#include "ServiceFactory.h"
#include "DataPools.h"
#include "CommonDataPool.h"
#include "NSLDBThread.h"
#include "Message.h"
#include "ServiceError.h"

using namespace nsl;

extern char LETTER_TEXT[9][255];
extern char SENDER_NPC_NAME[0xff];

void PrintDnfItemInfo(DnfItemInfo& itemInfo, char* out);
void GetRandomOptionName(DnfItemInfo* item, char* itemName, int itemNameLength);

AuctionDictionary::AuctionDictionary()
    : mAuctionDicDataPool(0x20), mCharacterNamePool(0x20)
{
    mpAuction = NULL;
}

AuctionDictionary::~AuctionDictionary()
{
    mAuctionDicTable.clear();
}

int AuctionDictionary::AddItemAveragePrice(
    unsigned long itemId, unsigned char itemUpgradeValue, int price,
    bool& rFristAdd, int& rAveragePrice, bool isStack,
    const ROI_AverageKey& _roi_average_key, unsigned int& _real_purchase_cnt,
    unsigned char itemRefineValue)
{
    return mAvrgPriceDic.AddItemAveragePrice(itemId, itemUpgradeValue, price, rFristAdd,
                                             rAveragePrice, isStack, _roi_average_key,
                                             _real_purchase_cnt, itemRefineValue);
}

int AuctionDictionary::GetAveragePrice(unsigned long itemId,
                                       unsigned char itemUpgradeValue,
                                       const ROI_AverageKey& _roi_average_key,
                                       unsigned char itemRefineValue,
                                       int* pOutAveragePrice)
{
    return mAvrgPriceDic.GetItemAveragePrice(itemId, itemUpgradeValue, _roi_average_key,
                                             itemRefineValue, pOutAveragePrice);
}

int AuctionDictionary::dic_Set_ROI_Constraint(
    const ROI_Average_Constraint& _roi_average_constraint)
{
    return (unsigned int)mAvrgPriceDic.aver_Set_ROI_Constraint(_roi_average_constraint);
}

int AuctionDictionary::GetNowRegistedItemNum(int ownerId)
{
    if (ownerId == -1)
    {
        return 0;
    }
    return mRegisterDic.NowItemNum(ownerId);
}

AuctionDictionary::AuctionDictionaryData* AuctionDictionary::GetAuctionDicData(
    unsigned long long auctionId) const
{
    std::map<unsigned long long, AuctionDictionaryData*>::const_iterator iter =
        mAuctionDicTable.find(auctionId);
    if (iter != mAuctionDicTable.end())
    {
        return iter->second;
    }
    return NULL;
}

unsigned char AuctionDictionary::GetOwnerIsVIP(unsigned long long auctionid, OwnerInfo& info)
{
    std::map<unsigned long long, AuctionDictionaryData*>::const_iterator iter =
        mAuctionDicTable.find(auctionid);
    if (iter != mAuctionDicTable.end())
    {
        info.is_vip = iter->second->owner_type != '\0';
        info.average_price = 0;
        return iter->second->owner_type;
    }
    return '\0';
}

unsigned char AuctionDictionary::getExpiringTime(long expirationTime, long nowTime)
{
    long now_time = nowTime;
    if (now_time == 0)
    {
        now_time = (long)((unsigned long long)nsl::pApp->getTick() / 1000);
    }
    expirationTime -= now_time;
    if (expirationTime < 0xe11)
    {
        return 1;
    }
    return (unsigned char)(expirationTime / 0xe10) + 1;
}

char* AuctionDictionary::getCharacterName(int characterId)
{
    std::map<const int, CharacterNameStruct*>::iterator iter =
        mCharacterNameTable.find(characterId);
    CharacterNameStruct* ptr_character_name;
    if (iter == mCharacterNameTable.end())
    {
        G_TraceLog()->sysLog(7, "getCharacterName, failed(), characterId : %d", characterId);
        ptr_character_name = (CharacterNameStruct*)0x8151740;
    }
    else
    {
        ptr_character_name = iter->second;
    }
    return ptr_character_name->char_name;
}

void AuctionDictionary::AuctionDictionaryData::PrintOut(unsigned long long auctionId)
{
    char temp[524];
    PrintDnfItemInfo(item_info, temp);
    G_TraceLog()->sysLog(5, "%llu:%d:%d:%d:%d:%d:%s", auctionId, price, instant_price,
                         owner_id, buyer_id, expire_time, temp);
}

void GetRandomOptionName(DnfItemInfo* item, char* itemName, int itemNameLength)
{
    unsigned int uVar11;
    if (item->random_option_.modify_option_.option_index_ == '\0')
    {
        uVar11 = 0xffffffff;
    }
    else
    {
        uVar11 = (unsigned int)(item->random_option_.modify_seed_.seed_ & 3);
    }
    unsigned char bVar1 = item->random_option_.seed_.seed_;
    unsigned char bVar2 = item->random_option_.get_second_value(ENUM_RANDOM_OPTION_THIRD);
    unsigned char bVar3 = item->random_option_.get_first_value(ENUM_RANDOM_OPTION_THIRD);
    unsigned char bVar4 = item->random_option_.get_option_index(ENUM_RANDOM_OPTION_THIRD);
    unsigned char bVar5 = item->random_option_.get_second_value(ENUM_RANDOM_OPTION_SECOND);
    unsigned char bVar6 = item->random_option_.get_first_value(ENUM_RANDOM_OPTION_SECOND);
    unsigned char bVar7 = item->random_option_.get_option_index(ENUM_RANDOM_OPTION_SECOND);
    unsigned char bVar8 = item->random_option_.get_second_value(ENUM_RANDOM_OPTION_FIRST);
    unsigned char bVar9 = item->random_option_.get_first_value(ENUM_RANDOM_OPTION_FIRST);
    unsigned char bVar10 = item->random_option_.get_option_index(ENUM_RANDOM_OPTION_FIRST);
    snprintf(itemName, itemNameLength, "@%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d@",
             item->item_id, (unsigned int)bVar10, (unsigned int)bVar9, (unsigned int)bVar8,
             (unsigned int)bVar7, (unsigned int)bVar6, (unsigned int)bVar5,
             (unsigned int)bVar4, (unsigned int)bVar3, (unsigned int)bVar2,
             (unsigned int)bVar1, uVar11);
}

void AuctionDictionary::PutDBBuyerHistory(unsigned long long auctionId, int pre_buyer_id,
                                          int buyer_id, int pre_price, int price)
{
    tagAUCTION_DB_BUYER_HISTORY dbtr_buyer;
    dbtr_buyer.pre_buyer_id = pre_buyer_id;
    dbtr_buyer.buyer_id = buyer_id;
    dbtr_buyer.pre_price = pre_price;
    dbtr_buyer.price = price;
    dbtr_buyer.pre_buyer_postal_id = 0;
    dbtr_buyer.auction_id = auctionId;
    CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
    Message* pMsg = pPool->createMessage(3);
    CMsgCell* pNewCell = pMsg->getCellFromMessage();
    *pNewCell << &dbtr_buyer;
    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
}

void AuctionDictionary::PutDBUpdateItem(unsigned long long auctionId, int instant_price, int add_info)
{
    tagAUCTION_DB_UPDATE_ITEM dbtr_update_item;
    dbtr_update_item.instant_price = instant_price;
    dbtr_update_item.add_info = add_info;
    dbtr_update_item.auction_id = auctionId;
    CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
    Message* pMsg = pPool->createMessage(3);
    CMsgCell* pNewCell = pMsg->getCellFromMessage();
    *pNewCell << &dbtr_update_item;
    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
}

void AuctionDictionary::PutDBDeleteItem(unsigned long long auctionId)
{
    tagAUCTION_DB_DELETE_ITEM dbtr_delete_item;
    dbtr_delete_item.auction_id = auctionId;
    CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
    Message* pMsg = pPool->createMessage(3);
    CMsgCell* pNewCell = pMsg->getCellFromMessage();
    *pNewCell << &dbtr_delete_item;
    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
}

void AuctionDictionary::PutDBExpireHistory(
    unsigned long long auctionId, int buyerId, AuctionDictionaryData* pAucDicData, int price,
    int count, AUCTION_HISTORY_EVENT_TYPE event_type)
{
    tagAUCTION_DB_EXPIRE_HISTORY dbtr_history;
    dbtr_history.expire_time = pAucDicData->expire_time;
    dbtr_history.event_type = (unsigned char)event_type;
    dbtr_history.owner_id = pAucDicData->owner_id;
    dbtr_history.buyer_id = buyerId;
    dbtr_history.price = price;
    dbtr_history.item_info = pAucDicData->item_info;
    dbtr_history.unit_price = pAucDicData->unit_price;
    dbtr_history.item_info.add_info = count;
    dbtr_history.auction_id = auctionId;
    if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
    {
        dbtr_history.commission = 0;
        dbtr_history.owner_type = pAucDicData->owner_type;
    }
    dbtr_history.owner_postal_id = 0;
    dbtr_history.buyer_postal_id = 0;
    CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
    Message* pMsg = pPool->createMessage(3);
    CMsgCell* pNewCell = pMsg->getCellFromMessage();
    *pNewCell << &dbtr_history;
    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
}

int AuctionDictionary::GetRegistedItemInfo(int ownerId, int* pInOutItemNum,
                                           MyRegistedItemInfo* pOutMyRegistedItemInfoArray)
{
    static int index_cnt = 0;
    int error_code = 0;

    if (*pInOutItemNum == 0)
    {
        error_code = 0x29;
    }
    else
    {
        CharacterDictionary::CharacterDictionaryData* ptr_data;
        mRegisterDic.GetAuctionIdList(ownerId, ptr_data);
        if (ptr_data == NULL)
        {
            *pInOutItemNum = 0;
        }
        else
        {
            index_cnt = 0;
            for (std::vector<unsigned long long>::iterator id_list_iter =
                     ptr_data->auction_id_vector.begin();
                 id_list_iter != ptr_data->auction_id_vector.end(); ++id_list_iter)
            {
                if (*pInOutItemNum == index_cnt)
                {
                    error_code = 0x29;
                    break;
                }
                std::map<unsigned long long, AuctionDictionaryData*>::iterator auc_dic_iter =
                    mAuctionDicTable.find(*id_list_iter);
                if (auc_dic_iter == mAuctionDicTable.end())
                {
                    error_code = 0x24;
                    break;
                }
                AuctionDictionaryData* ptr_auc_data = auc_dic_iter->second;
                pOutMyRegistedItemInfoArray[index_cnt].auction_id = *id_list_iter;
                pOutMyRegistedItemInfoArray[index_cnt].price = ptr_auc_data->price;
                pOutMyRegistedItemInfoArray[index_cnt].instant_price =
                    ptr_auc_data->instant_price;
                if (ptr_auc_data->buyer_id == -1)
                {
                    strncpy(pOutMyRegistedItemInfoArray[index_cnt].buyer_name, "", 0xd);
                }
                else
                {
                    strncpy(pOutMyRegistedItemInfoArray[index_cnt].buyer_name,
                            getCharacterName(ptr_auc_data->buyer_id), 0xc);
                }
                pOutMyRegistedItemInfoArray[index_cnt].item_info = ptr_auc_data->item_info;
                pOutMyRegistedItemInfoArray[index_cnt].expire_time =
                    getExpiringTime(ptr_auc_data->expire_time, 0);
                pOutMyRegistedItemInfoArray[index_cnt].item_info.abilityType_ =
                    ptr_auc_data->item_info.getAbilityType();
                pOutMyRegistedItemInfoArray[index_cnt].item_info.abilityValue_ =
                    ptr_auc_data->item_info.getAbilityValue();
                index_cnt = index_cnt + 1;
            }
            *pInOutItemNum = index_cnt;
        }
    }
    return error_code;
}

int AuctionDictionary::makeSuccessfulBid(unsigned long long auctionId,
                                         AuctionDictionaryData* pAucDicData,
                                         bool isInstantBuying, int& charge_point)
{
    int error_code = 0;
    int commission = 0;
    if ((!isInstantBuying) && (pAucDicData->buyer_id != -1))
    {
        error_code = mBidderDic.SubAuctionId(pAucDicData->buyer_id, auctionId);
        if (error_code != 0)
        {
            return error_code;
        }
    }
    error_code = mRegisterDic.SubAuctionId(pAucDicData->owner_id, auctionId);
    if (error_code == 0)
    {
        PutDBDeleteItem(auctionId);
        if (pAucDicData->buyer_id == -1)
        {
            char _itemName[128];
            if (*(int*)&pAucDicData->_reg_roi_category_key.field_0 != 0 ||
                *(int*)((char*)&pAucDicData->_reg_roi_category_key.field_0 + 4) != 0)
            {
                GetRandomOptionName(&pAucDicData->item_info, _itemName, 0x7f);
            }
            tagGAME_DB_SEND_PACKAGE_BY_EXPIRE dbtr_expire_package;
            dbtr_expire_package.package_type = AUCTION_HISTORY_EXPIRE_EVENT;
            dbtr_expire_package.b_exist_buyer = false;
            dbtr_expire_package.send_to_owner.owner_id = pAucDicData->owner_id;
            dbtr_expire_package.send_to_owner.receiver = pAucDicData->owner_id;
            dbtr_expire_package.send_to_owner.money = 10000;
            dbtr_expire_package.send_to_owner.item_info = pAucDicData->item_info;
            dbtr_expire_package.send_to_owner.auction_id = auctionId;
            CNRDItemInfoList::STItemInfo* pSVar8 =
                mpAuction->GetItemInfo(pAucDicData->item_info.item_id);
            unsigned int item_category = (unsigned int)pSVar8->category_;
            if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
            {
                char* pacVar9 = mpAuction->IsAvatarCategory(item_category)
                    ? mpAuction->GetAvatarColorName(
                          dbtr_expire_package.send_to_buyer.item_info.add_info)
                    : LETTER_TEXT[7];
                snprintf(dbtr_expire_package.send_to_owner.letter_text, 0xff,
                         LETTER_TEXT[6], 0xff,
                         mpAuction->GetItemInfo(pAucDicData->item_info.item_id)
                             ->sName_.c_str(),
                         pacVar9);
            }
            else
            {
                char* pacVar9 = mpAuction->IsAvatarCategory(item_category)
                    ? mpAuction->GetAvatarColorName(
                          dbtr_expire_package.send_to_buyer.item_info.add_info)
                    : LETTER_TEXT[7];
                char* pcVar16;
                if (*(int*)&pAucDicData->_reg_roi_category_key.field_0 == 0 &&
                    *(int*)((char*)&pAucDicData->_reg_roi_category_key.field_0 + 4) == 0)
                {
                    pcVar16 = (char*)
                        mpAuction->GetItemInfo(pAucDicData->item_info.item_id)
                            ->sName_.c_str();
                }
                else
                {
                    pcVar16 = _itemName;
                }
                snprintf(dbtr_expire_package.send_to_owner.letter_text, 0xff,
                         LETTER_TEXT[6], 0xff, pcVar16, pacVar9);
            }
            dbtr_expire_package.send_to_owner.temp_item_id =
                pAucDicData->item_info.item_id;
            dbtr_expire_package.send_to_owner.letter_text_length =
                (unsigned short)strlen(dbtr_expire_package.send_to_owner.letter_text);
            if ((G_Auction()->GetPayType() == PAY_TYPE_POINT) &&
                (0x28d287 < pAucDicData->item_info.item_id) &&
                (pAucDicData->item_info.item_id < 0x28d294))
            {
                dbtr_expire_package.send_to_owner.money = 0;
            }
            if ((G_Auction()->GetPayType() == PAY_TYPE_POINT) &&
                (0x28d287 < pAucDicData->item_info.item_id) &&
                (pAucDicData->item_info.item_id < 0x28d29a))
            {
                dbtr_expire_package.send_to_owner.money = 0;
            }
            CommonDataPool* pPool =
                pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
            Message* pMsg = pPool->createMessage(3);
            CMsgCell* pNewCell = pMsg->getCellFromMessage();
            *pNewCell << &dbtr_expire_package;
            pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
        }
        else
        {
            int price = isInstantBuying ? pAucDicData->instant_price : pAucDicData->price;
            if (mpAuction->IsPrivateStoreOpen(pAucDicData->owner_id))
            {
                double dVar1 = (double)price / 100.0;
                if (pAucDicData->owner_type == '\x01')
                {
                    commission = (int)(mpAuction->GetVIPCommission() * dVar1);
                }
                else
                {
                    commission = (int)(mpAuction->GetCommission() * dVar1);
                }
            }
            else
            {
                if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
                {
                    double BasicCommission = 5.0;
                    double VipCommission = 2.0;
                    int AvePrice = 0;
                    ROI_AverageKey roiAverageKey;
                    char itemRefineValue = 0;
                    unsigned char uVar3 = pAucDicData->item_info.GetUpgradeValue();
                    unsigned long uVar15 = pAucDicData->item_info.GetItemId();
                    GetAveragePrice(uVar15, uVar3, roiAverageKey, itemRefineValue, &AvePrice);
                    if (AvePrice == 0)
                    {
                        commission = pAucDicData->owner_type == '\x01'
                            ? (int)VipCommission
                            : (int)BasicCommission;
                    }
                    else
                    {
                        double PriceRate = (double)price / (double)AvePrice;
                        if (PriceRate >= 1.5)
                        {
                            if ((PriceRate < 1.5) || (1.6 <= PriceRate))
                            {
                                if ((PriceRate < 1.6) || (1.7 <= PriceRate))
                                {
                                    if ((PriceRate < 1.7) || (1.8 <= PriceRate))
                                    {
                                        commission = 0x50;
                                    }
                                    else
                                    {
                                        commission = 0x32;
                                    }
                                }
                                else
                                {
                                    commission = 0x32;
                                }
                            }
                            else
                            {
                                commission = 0x32;
                            }
                        }
                        else
                        {
                            commission = pAucDicData->owner_type == '\x01'
                                ? (int)VipCommission
                                : (int)BasicCommission;
                        }
                    }
                    commission = (int)((double)commission * ((double)price / 100.0));
                }
                else if (pAucDicData->owner_type == '\x01')
                {
                    commission = (int)(mpAuction->GetVIPCommission() *
                                       ((double)price / 100.0));
                }
                else
                {
                    commission = (int)(mpAuction->GetCommission() *
                                       ((double)price / 100.0));
                }
            }
            int send_money = 0;
            if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
            {
                send_money = price - commission;
            }
            else
            {
                send_money = (price - commission) + 10000;
            }
            char _itemName[128];
            if (*(int*)&pAucDicData->_reg_roi_category_key.field_0 != 0 ||
                *(int*)((char*)&pAucDicData->_reg_roi_category_key.field_0 + 4) != 0)
            {
                GetRandomOptionName(&pAucDicData->item_info, _itemName, 0x7f);
            }
            tagGAME_DB_SEND_PACKAGE_BY_EXPIRE dbtr_expire_package;
            dbtr_expire_package.package_type = (unsigned char)isInstantBuying;
            dbtr_expire_package.b_exist_buyer = true;
            dbtr_expire_package.send_to_buyer.owner_id = pAucDicData->owner_id;
            dbtr_expire_package.send_to_buyer.receiver = pAucDicData->buyer_id;
            dbtr_expire_package.send_to_buyer.money = 0;
            dbtr_expire_package.send_to_buyer.item_info = pAucDicData->item_info;
            dbtr_expire_package.send_to_owner.auction_id = auctionId;
            CNRDItemInfoList::STItemInfo* pSVar8 =
                mpAuction->GetItemInfo(pAucDicData->item_info.item_id);
            unsigned int item_category = (unsigned int)pSVar8->category_;
            if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
            {
                char* pacVar9 = mpAuction->IsAvatarCategory(item_category)
                    ? mpAuction->GetAvatarColorName(
                          dbtr_expire_package.send_to_buyer.item_info.add_info)
                    : LETTER_TEXT[7];
                snprintf(dbtr_expire_package.send_to_buyer.letter_text, 0xff,
                         LETTER_TEXT[2], 0xff,
                         mpAuction->GetItemInfo(pAucDicData->item_info.item_id)
                             ->sName_.c_str(),
                         pacVar9);
            }
            else
            {
                char* pacVar9 = mpAuction->IsAvatarCategory(item_category)
                    ? mpAuction->GetAvatarColorName(
                          dbtr_expire_package.send_to_buyer.item_info.add_info)
                    : LETTER_TEXT[7];
                char* pcVar16;
                if (*(int*)&pAucDicData->_reg_roi_category_key.field_0 == 0 &&
                    *(int*)((char*)&pAucDicData->_reg_roi_category_key.field_0 + 4) == 0)
                {
                    pcVar16 = (char*)
                        mpAuction->GetItemInfo(pAucDicData->item_info.item_id)
                            ->sName_.c_str();
                }
                else
                {
                    pcVar16 = _itemName;
                }
                snprintf(dbtr_expire_package.send_to_buyer.letter_text, 0xff,
                         LETTER_TEXT[2], 0xff, pcVar16, pacVar9);
            }
            dbtr_expire_package.send_to_buyer.temp_item_id =
                pAucDicData->item_info.item_id;
            dbtr_expire_package.send_to_buyer.letter_text_length =
                (unsigned short)strlen(dbtr_expire_package.send_to_buyer.letter_text);
            dbtr_expire_package.send_to_owner.owner_id = pAucDicData->owner_id;
            dbtr_expire_package.send_to_owner.receiver = pAucDicData->owner_id;
            dbtr_expire_package.send_to_owner.money = (unsigned int)send_money;
            dbtr_expire_package.send_to_owner.item_info.seal = false;
            dbtr_expire_package.send_to_owner.item_info.item_id = 0;
            if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
            {
                dbtr_expire_package.send_to_owner.money = 0;
            }
            if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
            {
                dbtr_expire_package.send_to_owner.money = (unsigned int)send_money;
                dbtr_expire_package.send_to_owner.item_info.item_id = 0x28eba2;
            }
            else
            {
                dbtr_expire_package.send_to_owner.item_info.item_id = 0;
            }
            dbtr_expire_package.send_to_owner.item_info.uniItemAttr = '\0';
            dbtr_expire_package.send_to_owner.item_info.add_info = 0;
            dbtr_expire_package.send_to_owner.item_info.endurance = 0;
            dbtr_expire_package.send_to_owner.item_info.extendInfo = 0;
            dbtr_expire_package.send_to_owner.item_info.abilityType_ = '\0';
            dbtr_expire_package.send_to_owner.item_info.abilityValue_ = 0;
            if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
            {
                char* pacVar9 = mpAuction->IsAvatarCategory(item_category)
                    ? mpAuction->GetAvatarColorName(pAucDicData->item_info.add_info)
                    : LETTER_TEXT[7];
                char* pacVar12 = mpAuction->IsAvatarCategory(item_category)
                    ? mpAuction->GetAvatarColorName(pAucDicData->item_info.add_info)
                    : LETTER_TEXT[7];
                snprintf(dbtr_expire_package.send_to_owner.letter_text, 0xff,
                         LETTER_TEXT[8], 0xff,
                         mpAuction->GetItemInfo(pAucDicData->item_info.item_id)
                             ->sName_.c_str(),
                         pacVar12, price, commission,
                         mpAuction->GetItemInfo(pAucDicData->item_info.item_id)
                             ->sName_.c_str(),
                         pacVar9, send_money);
                charge_point = send_money;
            }
            else
            {
                char* pacVar9 = mpAuction->IsAvatarCategory(item_category)
                    ? mpAuction->GetAvatarColorName(pAucDicData->item_info.add_info)
                    : LETTER_TEXT[7];
                char* pcVar16;
                if (*(int*)&pAucDicData->_reg_roi_category_key.field_0 == 0 &&
                    *(int*)((char*)&pAucDicData->_reg_roi_category_key.field_0 + 4) == 0)
                {
                    pcVar16 = (char*)
                        mpAuction->GetItemInfo(pAucDicData->item_info.item_id)
                            ->sName_.c_str();
                }
                else
                {
                    pcVar16 = _itemName;
                }
                char* pacVar12 = mpAuction->IsAvatarCategory(item_category)
                    ? mpAuction->GetAvatarColorName(pAucDicData->item_info.add_info)
                    : LETTER_TEXT[7];
                char* pcVar14;
                if (*(int*)&pAucDicData->_reg_roi_category_key.field_0 == 0 &&
                    *(int*)((char*)&pAucDicData->_reg_roi_category_key.field_0 + 4) == 0)
                {
                    pcVar14 = (char*)
                        mpAuction->GetItemInfo(pAucDicData->item_info.item_id)
                            ->sName_.c_str();
                }
                else
                {
                    pcVar14 = _itemName;
                }
                snprintf(dbtr_expire_package.send_to_owner.letter_text, 0xff,
                         LETTER_TEXT[1], 0xff, pcVar14, pacVar12, price, 10000,
                         commission, pcVar16, pacVar9, send_money);
            }
            dbtr_expire_package.send_to_owner.letter_text_length =
                (unsigned short)strlen(dbtr_expire_package.send_to_owner.letter_text);
            dbtr_expire_package.send_to_owner.temp_item_id =
                pAucDicData->item_info.item_id;
            CommonDataPool* pPool =
                pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
            Message* pMsg = pPool->createMessage(3);
            CMsgCell* pNewCell = pMsg->getCellFromMessage();
            *pNewCell << &dbtr_expire_package;
            pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
            int price_00 = isInstantBuying
                ? pAucDicData->instant_price
                : pAucDicData->price;
            CNRDItemInfoList::STItemInfo* pSVar8b =
                mpAuction->GetItemInfo(pAucDicData->item_info.item_id);
            if (mpAuction->IsStackableCategory(pSVar8b->category_))
            {
                if (pAucDicData->item_info.add_info != 0)
                {
                    price_00 = price_00 / pAucDicData->item_info.add_info;
                }
                if (price_00 < 1)
                {
                    price_00 = 1;
                }
            }
            ROI_AverageKey _temp_roi_average_key;
            _temp_roi_average_key.baseItem_index = pAucDicData->item_info.GetItemId();
            _temp_roi_average_key.option_category.field_0._qw =
                *(unsigned long long*)&pAucDicData->_reg_roi_category_key.field_0;
            _temp_roi_average_key.option_category.field_1._low_category_key =
                pAucDicData->_reg_roi_category_key.field_1._low_category_key;
            for (int i = 0; i < 3; i = i + 1)
            {
                *(short*)((char*)&_temp_roi_average_key.option_index_key + i * 2) =
                    (short)pAucDicData->item_info.random_option_.option_[i].option_index_;
            }
            std::sort((short*)&_temp_roi_average_key.option_index_key,
                      (short*)((char*)&_temp_roi_average_key.option_index_key + 6));
            unsigned char itemRefineValue =
                pAucDicData->item_info.separate_info.GetUpgradeSeparate();
            unsigned char itemUpgradeValue = pAucDicData->item_info.GetUpgradeValue();
            unsigned long uVar15 = pAucDicData->item_info.GetItemId();
            error_code = mpAuction->AddItemAveragePrice(
                uVar15, itemUpgradeValue, price_00, 0, _temp_roi_average_key,
                itemRefineValue, false);
        }
        tagAUCTION_DB_EXPIRE_HISTORY dbtr_history;
        dbtr_history.expire_time = pAucDicData->expire_time;
        dbtr_history.event_type = (unsigned char)isInstantBuying;
        dbtr_history.owner_id = pAucDicData->owner_id;
        dbtr_history.buyer_id = pAucDicData->buyer_id;
        dbtr_history.price = isInstantBuying
            ? pAucDicData->instant_price
            : pAucDicData->price;
        dbtr_history.item_info = pAucDicData->item_info;
        dbtr_history.unit_price = pAucDicData->unit_price;
        dbtr_history._reg_roi_category_key = pAucDicData->_reg_roi_category_key;
        dbtr_history.auction_id = auctionId;
        if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
        {
            dbtr_history.commission = commission;
            dbtr_history.owner_type = pAucDicData->owner_type;
        }
        dbtr_history.owner_postal_id = 0;
        dbtr_history.buyer_postal_id = 0;
        CommonDataPool* pPool =
            pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        Message* pMsg = pPool->createMessage(3);
        CMsgCell* pNewCell = pMsg->getCellFromMessage();
        *pNewCell << &dbtr_history;
        pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
        error_code = mpAuction->mSearch.Delete(auctionId);
        G_TraceLog()->sysLog(5, "Delete at Search module result:%s, auction_id:%llu",
                             GetErrorStr(error_code), auctionId);
        if (error_code == 0)
        {
            size_t sVar18 = mAuctionDicTable.erase(auctionId);
            if (sVar18 == 0)
            {
                error_code = 0xb;
            }
            else
            {
                G_TraceLog()->sysLog(
                    5, "makeSuccessfulBid, Auction ID : %llu, Item ID : %hu is deleted.",
                    auctionId, (unsigned short)pAucDicData->item_info.item_id);
                int item_type =
                    mpAuction->CheckItemType(pAucDicData->item_info.item_id);
                if (item_type == 3)
                {
                    mpAuction->UnregistChkMapForAvatarCreature(
                        false, pAucDicData->item_info.add_info);
                }
                else if (item_type == 2)
                {
                    mpAuction->UnregistChkMapForAvatarCreature(
                        true, pAucDicData->item_info.add_info);
                }
                CNRDItemInfoList::STItemInfo* pSVar8 =
                    mpAuction->GetItemInfo(pAucDicData->item_info.item_id);
                if (mpAuction->IsAvatarCategory(pSVar8->category_))
                {
                    mpAuction->SubAvatarEmblemInfo(pAucDicData->item_info.add_info);
                    mpAuction->SubAvatarExpansionInfo(pAucDicData->item_info.add_info);
                }
                mAuctionDicDataPool.free(pAucDicData);
                error_code = 0;
            }
        }
        else
        {
            G_TraceLog()->sysLog(
                7, "Error occured while delete auction info from search module, %s",
                GetErrorStr(error_code));
        }
    }
    else if ((!isInstantBuying) && (pAucDicData->buyer_id != -1))
    {
        mBidderDic.AddAuctionId(pAucDicData->buyer_id, auctionId);
    }
    return error_code;
}

int AuctionDictionary::ProcessMostRecentExpireItem(bool& one_processing, int auction_type)
{
    int error_code = 0;
    ExpireTimeDictionary::ExpireTimeDictionaryData* ptr_etdata = NULL;
    if (auction_type == ENUM_AUCTION_USER)
    {
        error_code = mExprTimeDic.PeekMostRecentExpireItem(&ptr_etdata);
    }
    else if (auction_type == ENUM_AUCTION_SYSTEM)
    {
        error_code = mExprTimeDicForSystemAuction.PeekMostRecentExpireItem(&ptr_etdata);
    }
    else
    {
        error_code = 0x32;
    }
    if (error_code != 0)
    {
        return error_code;
    }
    long time_now = (long)time(NULL);
    if (ptr_etdata->expire_time < time_now)
    {
        one_processing = true;
        unsigned long long au_id = ptr_etdata->auction_id;
        if (auction_type == ENUM_AUCTION_USER)
        {
            mExprTimeDic.PopMostRecentExpireItem();
        }
        else
        {
            if (auction_type != ENUM_AUCTION_SYSTEM)
            {
                return 0x32;
            }
            mExprTimeDicForSystemAuction.PopMostRecentExpireItem();
        }
        if (au_id != 0)
        {
            std::map<unsigned long long, AuctionDictionaryData*>::iterator iter =
                mAuctionDicTable.find(au_id);
            if (iter == mAuctionDicTable.end())
            {
                return 0x24;
            }
            AuctionDictionaryData* ptr_addata = iter->second;
            int charge_point = 0;
            error_code = makeSuccessfulBid(au_id, ptr_addata, false, charge_point);
            if (error_code != 0)
            {
                G_TraceLog()->sysLog(7, "makeSuccessfulBid() failed. %s",
                                     GetErrorStr(error_code));
                return error_code;
            }
        }
    }
    return 0;
}

void AuctionDictionary::PutDBSendPackageByExpire(
    unsigned long long auctionId, int buyerId, AuctionDictionaryData* pAucDicData, int money,
    int commission, int send_money, int count, AUCTION_HISTORY_EVENT_TYPE package_type,
    bool entire)
{
    CNRDItemInfoList::STItemInfo* pItemInfo =
        mpAuction->GetItemInfo(pAucDicData->item_info.item_id);
    if (pItemInfo != NULL)
    {
        tagGAME_DB_SEND_PACKAGE_BY_EXPIRE dbtr_expire_package;
        dbtr_expire_package.package_type = (unsigned char)package_type;
        dbtr_expire_package.b_exist_buyer = true;
        dbtr_expire_package.send_to_buyer.owner_id = pAucDicData->owner_id;
        dbtr_expire_package.send_to_buyer.receiver = buyerId;
        dbtr_expire_package.send_to_buyer.money = 0;
        dbtr_expire_package.send_to_buyer.item_info = pAucDicData->item_info;
        dbtr_expire_package.send_to_owner.auction_id = auctionId;
        if (mpAuction->IsStackableCategory(pItemInfo->category_))
        {
            dbtr_expire_package.send_to_buyer.item_info.add_info = count;
        }
        snprintf(dbtr_expire_package.send_to_buyer.letter_text, 0xff, LETTER_TEXT[2],
                 pItemInfo->sName_.c_str(), LETTER_TEXT[7]);
        dbtr_expire_package.send_to_buyer.temp_item_id =
            pAucDicData->item_info.item_id;
        dbtr_expire_package.send_to_buyer.letter_text_length =
            (unsigned short)strlen(dbtr_expire_package.send_to_buyer.letter_text);
        dbtr_expire_package.send_to_owner.owner_id = pAucDicData->owner_id;
        dbtr_expire_package.send_to_owner.receiver = pAucDicData->owner_id;
        dbtr_expire_package.send_to_owner.money = (unsigned int)send_money;
        dbtr_expire_package.send_to_owner.item_info.seal = false;
        dbtr_expire_package.send_to_owner.item_info.item_id = 0;
        dbtr_expire_package.send_to_owner.item_info.uniItemAttr = '\0';
        dbtr_expire_package.send_to_owner.item_info.add_info = 0;
        dbtr_expire_package.send_to_owner.item_info.endurance = 0;
        dbtr_expire_package.send_to_owner.item_info.extendInfo = 0;
        dbtr_expire_package.send_to_owner.item_info.abilityType_ = '\0';
        dbtr_expire_package.send_to_owner.item_info.abilityValue_ = 0;
        char _itemName[128];
        if (*(int*)&pAucDicData->_reg_roi_category_key.field_0 != 0 ||
            *(int*)((char*)&pAucDicData->_reg_roi_category_key.field_0 + 4) != 0)
        {
            GetRandomOptionName(&pAucDicData->item_info, _itemName, 0x7f);
        }
        char* first_color_name = mpAuction->IsAvatarCategory(pItemInfo->category_)
            ? mpAuction->GetAvatarColorName(pAucDicData->item_info.add_info)
            : LETTER_TEXT[7];
        char* pcVar4;
        if (*(int*)&pAucDicData->_reg_roi_category_key.field_0 == 0 &&
            *(int*)((char*)&pAucDicData->_reg_roi_category_key.field_0 + 4) == 0)
        {
            pcVar4 = (char*)pItemInfo->sName_.c_str();
        }
        else
        {
            pcVar4 = _itemName;
        }
        unsigned int uVar2 = entire ? 10000 : 0;
        char* second_color_name = mpAuction->IsAvatarCategory(pItemInfo->category_)
            ? mpAuction->GetAvatarColorName(pAucDicData->item_info.add_info)
            : LETTER_TEXT[7];
        char* pcVar6;
        if (*(int*)&pAucDicData->_reg_roi_category_key.field_0 == 0 &&
            *(int*)((char*)&pAucDicData->_reg_roi_category_key.field_0 + 4) == 0)
        {
            pcVar6 = (char*)pItemInfo->sName_.c_str();
        }
        else
        {
            pcVar6 = _itemName;
        }
        snprintf(dbtr_expire_package.send_to_owner.letter_text, 0xff, LETTER_TEXT[6],
                 pcVar6, second_color_name, count, money, uVar2, commission, pcVar4,
                 first_color_name, send_money);
        dbtr_expire_package.send_to_owner.letter_text_length =
            (unsigned short)strlen(dbtr_expire_package.send_to_owner.letter_text);
        dbtr_expire_package.send_to_owner.temp_item_id =
            pAucDicData->item_info.item_id;
        CommonDataPool* pPool =
            pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        Message* pNewMsg = pPool->createMessage(3);
        CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
        *pNewCell << &dbtr_expire_package;
        pApp->super_Threads.getDBThread(0)->PushTransaction(pNewMsg);
    }
}

int AuctionDictionary::Bidding(const unsigned long long& auctionId, int buyerId, const char* buyerName, int price,
                               char* owner_id, int& charge_point)
{
    int error_code = 0;
    unsigned int prev_buyer_id = 0xffffffff;
    unsigned int prev_bidding_price = 0;
    std::map<unsigned long long, AuctionDictionaryData*>::iterator iter =
        mAuctionDicTable.find(auctionId);
    if (iter != mAuctionDicTable.end())
    {
        AuctionDictionaryData* ptr_data = iter->second;
        if (ptr_data->owner_id == buyerId)
        {
            return 0x2d;
        }
        if (ptr_data->buyer_id == buyerId)
        {
            if (ptr_data->instant_price == -1)
            {
                return 0x2e;
            }
            if (ptr_data->instant_price != price)
            {
                return 0x2e;
            }
        }
        if (ptr_data->price == -1)
        {
            if (ptr_data->instant_price != price)
            {
                return 0x2f;
            }
        }
        else
        {
            int minimum_bidding_price = (int)(1.05 * (double)ptr_data->price);
            if (ptr_data->price == minimum_bidding_price)
            {
                minimum_bidding_price = minimum_bidding_price + 1;
            }
            if (ptr_data->instant_price == -1)
            {
                if (price < minimum_bidding_price)
                {
                    return 0x2f;
                }
            }
            else
            {
                if (mpAuction->GetPayType() == PAY_TYPE_POINT)
                {
                    return 0x25;
                }
                if (ptr_data->instant_price < minimum_bidding_price)
                {
                    minimum_bidding_price = ptr_data->instant_price;
                }
                if (ptr_data->instant_price < price)
                {
                    return 0x25;
                }
                if (price < minimum_bidding_price)
                {
                    return 0x2f;
                }
            }
        }
        std::map<const int, CharacterNameStruct*>::iterator char_name_iter =
            mCharacterNameTable.find(buyerId);
        if (char_name_iter == mCharacterNameTable.end())
        {
            CharacterNameStruct* new_character_name = mCharacterNamePool.malloc();
            strncpy(new_character_name->char_name, buyerName, 0xc);
            mCharacterNameTable.insert(
                std::make_pair<const int&, CharacterNameStruct*&>(
                    buyerId, new_character_name));
        }
        else
        {
            strncpy(char_name_iter->second->char_name, buyerName, 0xc);
        }
        prev_bidding_price = (unsigned int)ptr_data->price;
        prev_buyer_id = (unsigned int)ptr_data->buyer_id;
        if (prev_buyer_id != 0xffffffff)
        {
            error_code = mBidderDic.SubAuctionId(ptr_data->buyer_id, auctionId);
            if (error_code != 0)
            {
                return error_code;
            }
        }
        if ((ptr_data->instant_price == -1) ||
            (ptr_data->instant_price != price))
        {
            ptr_data->buyer_id = buyerId;
            ptr_data->price = price;
            error_code = mBidderDic.AddAuctionId(buyerId, auctionId);
            if (error_code != 0)
            {
                if (prev_buyer_id == 0xffffffff)
                {
                    return error_code;
                }
                mBidderDic.AddAuctionId(ptr_data->buyer_id, auctionId);
                return error_code;
            }
            tagAUCTION_DB_UPPER_BIDDING dbtr_upper_bidding;
            dbtr_upper_bidding.auction_id = auctionId;
            dbtr_upper_bidding.buyer_id = buyerId;
            strncpy(dbtr_upper_bidding.buyer_name, buyerName, 0xd);
            dbtr_upper_bidding.price = price;
            CommonDataPool* pPool =
                pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
            Message* pMsg = pPool->createMessage(3);
            CMsgCell* pNewCell = pMsg->getCellFromMessage();
            *pNewCell << &dbtr_upper_bidding;
            pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
        }
        else
        {
            ptr_data->expire_table_ptr->auction_id = 0;
            ptr_data->buyer_id = buyerId;
            error_code = makeSuccessfulBid(auctionId, ptr_data, true,
                                           charge_point);
            if (error_code != 0)
            {
                if (prev_buyer_id == 0xffffffff)
                {
                    return error_code;
                }
                mBidderDic.AddAuctionId(ptr_data->buyer_id, auctionId);
                return error_code;
            }
            strncpy(owner_id, ptr_data->owner_nexon_id,
                    strlen(ptr_data->owner_nexon_id));
        }
        tagAUCTION_DB_BUYER_HISTORY dbtr_buyer;
        dbtr_buyer.auction_id = auctionId;
        dbtr_buyer.pre_buyer_id = (int)prev_buyer_id;
        dbtr_buyer.buyer_id = buyerId;
        dbtr_buyer.pre_price = (int)prev_bidding_price;
        dbtr_buyer.price = price;
        dbtr_buyer.pre_buyer_postal_id = 0;
        CommonDataPool* pPool =
            pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        Message* pMsg = pPool->createMessage(3);
        CMsgCell* pNewCell = pMsg->getCellFromMessage();
        *pNewCell << &dbtr_buyer;
        pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
        if (prev_buyer_id != 0xffffffff)
        {
            char _itemName[128];
            if (*(int*)&ptr_data->_reg_roi_category_key.field_0 != 0 ||
                *(int*)((char*)&ptr_data->_reg_roi_category_key.field_0 + 4) != 0)
            {
                GetRandomOptionName(&ptr_data->item_info, _itemName, 0x7f);
            }
            tagGAME_DB_SEND_PACKAGE dbtrSendPackage;
            dbtrSendPackage.auction_id = auctionId;
            dbtrSendPackage.owner_id = ptr_data->owner_id;
            dbtrSendPackage.receiver = (int)prev_buyer_id;
            dbtrSendPackage.money = prev_bidding_price;
            dbtrSendPackage.item_info.seal = false;
            dbtrSendPackage.item_info.item_id = 0;
            dbtrSendPackage.item_info.uniItemAttr = '\0';
            dbtrSendPackage.item_info.add_info = 0;
            dbtrSendPackage.item_info.endurance = 0;
            dbtrSendPackage.item_info.extendInfo = 0;
            dbtrSendPackage.item_info.abilityType_ = '\0';
            dbtrSendPackage.item_info.abilityValue_ = 0;
            CNRDItemInfoList::STItemInfo* pSVar9 =
                mpAuction->GetItemInfo(ptr_data->item_info.item_id);
            unsigned int item_category = (unsigned int)pSVar9->category_;
            char* pacVar10 = mpAuction->IsAvatarCategory(item_category)
                ? mpAuction->GetAvatarColorName(ptr_data->item_info.add_info)
                : LETTER_TEXT[7];
            char* ptVar11;
            if (*(int*)&ptr_data->_reg_roi_category_key.field_0 == 0 &&
                *(int*)((char*)&ptr_data->_reg_roi_category_key.field_0 + 4) == 0)
            {
                ptVar11 = (char*)
                    mpAuction->GetItemInfo(ptr_data->item_info.item_id)
                        ->sName_.c_str();
            }
            else
            {
                ptVar11 = _itemName;
            }
            snprintf(dbtrSendPackage.letter_text, 0xff, LETTER_TEXT[4],
                     ptVar11, pacVar10);
            dbtrSendPackage.letter_text_length =
                (unsigned short)strlen(dbtrSendPackage.letter_text);
            dbtrSendPackage.temp_item_id = ptr_data->item_info.item_id;
            CommonDataPool* pPool2 =
                pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
            Message* pMsg2 = pPool2->createMessage(3);
            CMsgCell* pNewCell2 = pMsg2->getCellFromMessage();
            *pNewCell2 << &dbtrSendPackage;
            pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg2);
        }
        return 0;
    }
    else
    {
        return 0x24;
    }
}

int AuctionDictionary::GetBiddingInfo(int buyerId, int* pInOutItemNum,
                                      MyBiddingItemInfo* pOutMyBiddingItemInfoArray)
{
    static int index_cnt = 0;
    int error_code = 0;

    if (*pInOutItemNum == 0)
    {
        error_code = 0x29;
    }
    else
    {
        CharacterDictionary::CharacterDictionaryData* ptr_data;
        mBidderDic.GetAuctionIdList(buyerId, ptr_data);
        if (ptr_data == NULL)
        {
            *pInOutItemNum = 0;
        }
        else
        {
            index_cnt = 0;
            for (std::vector<unsigned long long>::iterator id_list_iter =
                     ptr_data->auction_id_vector.begin();
                 (id_list_iter != ptr_data->auction_id_vector.end()) &&
                     (*pInOutItemNum != index_cnt);
                 ++id_list_iter)
            {
                std::map<unsigned long long, AuctionDictionaryData*>::iterator auc_dic_iter =
                    mAuctionDicTable.find(*id_list_iter);
                if (auc_dic_iter == mAuctionDicTable.end())
                {
                    break;
                }
                AuctionDictionaryData* ptr_auc_data = auc_dic_iter->second;
                pOutMyBiddingItemInfoArray[index_cnt].auction_id = *id_list_iter;
                pOutMyBiddingItemInfoArray[index_cnt].price = ptr_auc_data->price;
                pOutMyBiddingItemInfoArray[index_cnt].instant_price =
                    ptr_auc_data->instant_price;
                strncpy(pOutMyBiddingItemInfoArray[index_cnt].owner_name,
                        getCharacterName(ptr_auc_data->owner_id), 0xc);
                pOutMyBiddingItemInfoArray[index_cnt].owner_reliability = 1.0;
                pOutMyBiddingItemInfoArray[index_cnt].expire_time =
                    getExpiringTime(ptr_auc_data->expire_time, 0);
                pOutMyBiddingItemInfoArray[index_cnt].item_info = ptr_auc_data->item_info;
                index_cnt = index_cnt + 1;
            }
            *pInOutItemNum = index_cnt;
        }
    }
    return error_code;
}

int AuctionDictionary::BuyItemApiece(unsigned long long& auctionId, int buyerId, int price, int count)
{
    std::map<unsigned long long, AuctionDictionaryData*>::iterator iter =
        mAuctionDicTable.find(auctionId);
    if (iter == mAuctionDicTable.end())
    {
        return 0x24;
    }
    AuctionDictionaryData* ptr_data = iter->second;
    if ((price < 1) || (count < 1))
    {
        return 0x26;
    }
    if (ptr_data->owner_id == buyerId)
    {
        return 0x2d;
    }
    if (ptr_data->instant_price == -1)
    {
        return 0x26;
    }
    if (ptr_data->unit_price * count != price)
    {
        return 0x26;
    }
    if (ptr_data->item_info.add_info < count)
    {
        return 0x2f;
    }
    if ((ptr_data->buyer_id == -1) && (ptr_data->price == -1))
    {
        CNRDItemInfoList::STItemInfo* ptr_item_info =
            mpAuction->GetItemInfo(ptr_data->item_info.item_id);
        if (ptr_item_info == NULL)
        {
            return 0x2b;
        }
        if (mpAuction->IsStackableCategory(ptr_item_info->category_))
        {
            bool entire = (ptr_data->item_info.add_info == count);
            int error_code = Purchase(auctionId, buyerId, ptr_data, price, count, entire);
            if (error_code == 0)
            {
                PutDBBuyerHistory(auctionId, -1, buyerId, 0, price);
                error_code = 0;
            }
            return error_code;
        }
        return 0x2b;
    }
    return 0x2e;
}

int AuctionDictionary::Purchase(unsigned long long auctionId, int buyerId,
                                AuctionDictionaryData* pAucDicData, int price, int count,
                                bool entire)
{
    int error_code = 0;
    PutDBExpireHistory(auctionId, buyerId, pAucDicData, price, count,
                       AUCTION_HISTORY_APIECE_EVENT);
    int commission = 0;
    double dVar9;
    if (pAucDicData->owner_type == '\x01')
    {
        dVar9 = mpAuction->GetVIPCommission() * ((double)price / 100.0);
    }
    else
    {
        dVar9 = mpAuction->GetCommission() * ((double)price / 100.0);
    }
    commission = (int)dVar9;
    if (entire)
    {
        int send_money = (price - commission) + 10000;
        error_code = mRegisterDic.SubAuctionId(pAucDicData->owner_id, auctionId);
        if (error_code != 0)
        {
            return error_code;
        }
        PutDBDeleteItem(auctionId);
        PutDBSendPackageByExpire(auctionId, buyerId, pAucDicData, price, commission,
                                 send_money, count, AUCTION_HISTORY_APIECE_EVENT, true);
        error_code = mpAuction->mSearch.Delete(auctionId);
        G_TraceLog()->sysLog(5, "Delete at Search module result:%s, auction_id:%llu",
                             GetErrorStr(error_code), auctionId);
        size_t sVar7 = mAuctionDicTable.erase(auctionId);
        if (sVar7 == 0)
        {
            return 0xb;
        }
        G_TraceLog()->sysLog(5, "Purchase entire, Auction ID : %llu, Item ID : %hu is deleted.",
                             auctionId, (unsigned short)pAucDicData->item_info.item_id);
        pAucDicData->expire_table_ptr->auction_id = 0;
        mAuctionDicDataPool.free(pAucDicData);
    }
    else
    {
        int send_money = price - commission;
        int remain_price = pAucDicData->instant_price - price;
        int remain_count = pAucDicData->item_info.add_info - count;
        if ((remain_price < 1) || (remain_count < 1))
        {
            return 0x22;
        }
        pAucDicData->instant_price = remain_price;
        pAucDicData->item_info.add_info = remain_count;
        PutDBUpdateItem(auctionId, remain_price, remain_count);
        PutDBSendPackageByExpire(auctionId, buyerId, pAucDicData, price, commission,
                                 send_money, count, AUCTION_HISTORY_APIECE_EVENT, false);
        G_TraceLog()->sysLog(5,
            "Purchase apiece, Auction ID : %llu, Item ID : %hu count : %d is deleted.",
            auctionId, (unsigned short)pAucDicData->item_info.item_id, count);
    }
    ROI_AverageKey _temp_roi_average_key;
    _temp_roi_average_key.baseItem_index = pAucDicData->item_info.GetItemId();
    _temp_roi_average_key.option_category.field_0._qw =
        *(unsigned long long*)&pAucDicData->_reg_roi_category_key.field_0;
    _temp_roi_average_key.option_category.field_1._low_category_key =
        pAucDicData->_reg_roi_category_key.field_1._low_category_key;
    for (int i = 0; i < 3; i = i + 1)
    {
        *(short*)((char*)&_temp_roi_average_key.option_index_key + i * 2) =
            (short)pAucDicData->item_info.random_option_.option_[i].option_index_;
    }
    std::sort((short*)&_temp_roi_average_key.option_index_key,
              (short*)((char*)&_temp_roi_average_key.option_index_key + 6));
    unsigned char itemRefineValue =
        pAucDicData->item_info.separate_info.GetUpgradeSeparate();
    int price_00 = pAucDicData->unit_price;
    unsigned char itemUpgradeValue = pAucDicData->item_info.GetUpgradeValue();
    unsigned long uVar8 = pAucDicData->item_info.GetItemId();
    mpAuction->AddItemAveragePrice(uVar8, itemUpgradeValue, price_00, 0,
                                   _temp_roi_average_key, itemRefineValue, false);
    G_TraceLog()->sysLog(5, "Purchase Success, Auction ID : %llu, Item ID : %hu ",
                         auctionId, (unsigned short)pAucDicData->item_info.item_id);
    return 0;
}

int AuctionDictionary::RegistItem(
    unsigned long long auctionId, int ownerId, const char* ownerName, int price, int instantPrice,
    unsigned int blackPoint, int unitPrice, DnfItemInfo& itemInfo, long expireTime,
    int buyerId, const char* buyerName, const ROI_Category& _roi_category_key, char userType,
    char* owner_nexon_id)
{
    int error_code = 0;
    price = isInvalidPrice(price) ? price : -1;
    instantPrice = isInvalidPrice(instantPrice) ? instantPrice : -1;
    if ((instantPrice < -1) || (price < -1))
    {
        return 0x26;
    }
    else if ((instantPrice == -1) && (price == -1))
    {
        return 0x26;
    }
    else if ((instantPrice != -1) && (instantPrice < price))
    {
        return 0x1f;
    }
    else
    {
        if (auctionId == 0)
        {
            return 4;
        }
        else
        {
            AuctionDictionaryData* ptr_data = mAuctionDicDataPool.malloc();
            memset(ptr_data, 0, 0x84);
            if (ptr_data == NULL)
            {
                return 9;
            }
            else
            {
                ptr_data->buyer_id = buyerId;
                ptr_data->instant_price = instantPrice;
                ptr_data->item_info = itemInfo;
                ptr_data->owner_id = ownerId;
                ptr_data->price = price;
                ptr_data->expire_time = expireTime;
                ptr_data->owner_type = userType;
                ptr_data->black_point = blackPoint;
                ptr_data->unit_price = unitPrice;
                ptr_data->_reg_roi_category_key = _roi_category_key;
                strncpy(ptr_data->owner_nexon_id, owner_nexon_id,
                        strlen(owner_nexon_id));
                std::pair<std::map<unsigned long long, AuctionDictionaryData*>::iterator, bool> in_pair;
                in_pair = mAuctionDicTable.insert(
                    std::make_pair<const unsigned long long&, AuctionDictionaryData*&>(
                        auctionId, ptr_data));
                if (in_pair.second == false)
                {
                    return 10;
                }
                if (ownerId == -1)
                {
                    error_code = mExprTimeDicForSystemAuction.PushRegistedItem(
                        ptr_data->expire_time, auctionId,
                        &ptr_data->expire_table_ptr);
                }
                else
                {
                    error_code = mExprTimeDic.PushRegistedItem(
                        ptr_data->expire_time, auctionId,
                        &ptr_data->expire_table_ptr);
                }
                if (error_code != 0)
                {
                    mAuctionDicTable.erase(auctionId);
                    return error_code;
                }
                error_code = mRegisterDic.AddAuctionId(ownerId, auctionId);
                if (error_code != 0)
                {
                    ptr_data->expire_table_ptr->auction_id = 0;
                    mAuctionDicTable.erase(auctionId);
                    return error_code;
                }
                std::map<const int, CharacterNameStruct*>::iterator
                    char_name_iter = mCharacterNameTable.find(ownerId);
                if (char_name_iter == mCharacterNameTable.end())
                {
                    CharacterNameStruct* new_character_name =
                        mCharacterNamePool.malloc();
                    if (ownerId == -1)
                    {
                        strncpy(new_character_name->char_name,
                                SENDER_NPC_NAME, 0xd);
                    }
                    else
                    {
                        strncpy(new_character_name->char_name,
                                ownerName, 0xc);
                        new_character_name->char_name[0xd] = '\0';
                    }
                    mCharacterNameTable.insert(
                        std::make_pair<const int&, CharacterNameStruct*&>(
                            ownerId, new_character_name));
                }
                else if (ownerId == -1)
                {
                    strncpy(char_name_iter->second->char_name,
                            SENDER_NPC_NAME, 0xd);
                }
                else
                {
                    strncpy(char_name_iter->second->char_name,
                            ownerName, 0xc);
                    char_name_iter->second->char_name[0xd] = '\0';
                }
                if ((buyerId != -1) && (buyerName != NULL))
                {
                    char_name_iter = mCharacterNameTable.find(buyerId);
                    if (char_name_iter == mCharacterNameTable.end())
                    {
                        CharacterNameStruct* new_character_name =
                            mCharacterNamePool.malloc();
                        strncpy(new_character_name->char_name,
                                buyerName, 0xc);
                        new_character_name->char_name[0xd] = '\0';
                        mCharacterNameTable.insert(
                            std::make_pair<const int&,
                                           CharacterNameStruct*&>(
                                buyerId, new_character_name));
                    }
                    else
                    {
                        strncpy(char_name_iter->second->char_name,
                                buyerName, 0xc);
                        char_name_iter->second->char_name[0xd] = '\0';
                    }
                    error_code = mBidderDic.AddAuctionId(buyerId, auctionId);
                    if (error_code != 0)
                    {
                        ptr_data->expire_table_ptr->auction_id = 0;
                        mRegisterDic.SubAuctionId(ownerId, auctionId);
                        mAuctionDicTable.erase(auctionId);
                        return error_code;
                    }
                }
                int instancePricePerUnit = 0;
                if (instantPrice != -1)
                {
                    instancePricePerUnit = instantPrice;
                }
                CNRDItemInfoList::STItemInfo* pItemInfo =
                    mpAuction->GetItemInfo(itemInfo.item_id);
                if (pItemInfo != NULL)
                {
                    if (mpAuction->IsStackableCategory(
                            pItemInfo->category_) &&
                        (1 < itemInfo.add_info))
                    {
                        instancePricePerUnit =
                            instancePricePerUnit / itemInfo.add_info;
                    }
                }
                if (instantPrice == -1)
                {
                    instancePricePerUnit = 0x7fffffff;
                }
                unsigned char refine =
                    itemInfo.separate_info.GetUpgradeSeparate();
                unsigned char uVar5 = itemInfo.GetUpgradeValue();
                int bVar4 = uVar5 != 0;
                unsigned long long auctionId_00 = auctionId;
                unsigned char upgrade = itemInfo.GetUpgradeValue();
                unsigned long itemId = itemInfo.GetItemId();
                error_code = mpAuction->mSearch.Insert(
                    itemId, upgrade, auctionId_00, bVar4, instancePricePerUnit,
                    refine);
                if (error_code != 0)
                {
                    mRegisterDic.SubAuctionId(ownerId, auctionId);
                    ptr_data->expire_table_ptr->auction_id = 0;
                    mAuctionDicTable.erase(auctionId);
                    return error_code;
                }
            }
        }
    }
    return 0;
}

int AuctionDictionary::RegistCancel(int ownerId, unsigned long long auctionId)
{
    AuctionDictionaryData* ptr_data = NULL;
    int error_code = 0;
    std::map<unsigned long long, AuctionDictionaryData*>::iterator iter =
        mAuctionDicTable.find(auctionId);
    if (iter != mAuctionDicTable.end())
    {
        ptr_data = iter->second;
    }
    if (ptr_data == NULL)
    {
        error_code = 0x24;
    }
    else if (ptr_data->owner_id == ownerId)
    {
        error_code = mpAuction->mSearch.Delete(auctionId);
        if (error_code == 0)
        {
            error_code = mRegisterDic.SubAuctionId(ptr_data->owner_id, auctionId);
            if (error_code == 0)
            {
                size_t sVar4 = mAuctionDicTable.erase(auctionId);
                if (sVar4 == 0)
                {
                    error_code = 0xb;
                }
                else
                {
                    tagAUCTION_DB_EXPIRE_HISTORY dbtr_history;
                    dbtr_history.expire_time = ptr_data->expire_time;
                    dbtr_history.event_type = AUCTION_HISTORY_REGIST_CANCEL_EVENT;
                    dbtr_history.owner_id = ptr_data->owner_id;
                    dbtr_history.buyer_id = ptr_data->buyer_id;
                    dbtr_history.price = ptr_data->price;
                    dbtr_history.item_info = ptr_data->item_info;
                    dbtr_history.unit_price = ptr_data->unit_price;
                    dbtr_history._reg_roi_category_key = ptr_data->_reg_roi_category_key;
                    dbtr_history.auction_id = auctionId;
                    if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
                    {
                        dbtr_history.commission = 0;
                        dbtr_history.owner_type = ptr_data->owner_type;
                    }
                    dbtr_history.owner_postal_id = 0;
                    dbtr_history.buyer_postal_id = 0;
                    CommonDataPool* pPool =
                        pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
                    Message* pMsg = pPool->createMessage(3);
                    CMsgCell* pNewCell = pMsg->getCellFromMessage();
                    *pNewCell << &dbtr_history;
                    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
                    tagAUCTION_DB_DELETE_ITEM dbtr_delete_item;
                    dbtr_delete_item.auction_id = auctionId;
                    pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
                    pMsg = pPool->createMessage(3);
                    pNewCell = pMsg->getCellFromMessage();
                    *pNewCell << &dbtr_delete_item;
                    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
                    tagGAME_DB_SEND_PACKAGE_BY_EXPIRE dbtr_expire_package;
                    dbtr_expire_package.package_type = AUCTION_HISTORY_REGIST_CANCEL_EVENT;
                    dbtr_expire_package.send_to_owner.auction_id = auctionId;
                    char _itemName[128];
                    if (*(int*)&ptr_data->_reg_roi_category_key.field_0 != 0 ||
                        *(int*)((char*)&ptr_data->_reg_roi_category_key.field_0 + 4) != 0)
                    {
                        GetRandomOptionName(&ptr_data->item_info, _itemName, 0x7f);
                    }
                    if (ptr_data->buyer_id == -1)
                    {
                        dbtr_expire_package.b_exist_buyer = false;
                    }
                    else
                    {
                        dbtr_expire_package.b_exist_buyer = true;
                        error_code = mBidderDic.SubAuctionId(ptr_data->buyer_id, auctionId);
                        if (error_code != 0)
                        {
                            return error_code;
                        }
                        dbtr_expire_package.send_to_buyer.owner_id = ptr_data->owner_id;
                        dbtr_expire_package.send_to_buyer.receiver = ptr_data->buyer_id;
                        dbtr_expire_package.send_to_buyer.money = ptr_data->price;
                        dbtr_expire_package.send_to_buyer.item_info.seal = false;
                        dbtr_expire_package.send_to_buyer.item_info.item_id = 0;
                        dbtr_expire_package.send_to_buyer.item_info.uniItemAttr = '\0';
                        dbtr_expire_package.send_to_buyer.item_info.add_info = 0;
                        dbtr_expire_package.send_to_buyer.item_info.endurance = 0;
                        dbtr_expire_package.send_to_buyer.item_info.extendInfo = 0;
                        dbtr_expire_package.send_to_buyer.item_info.abilityType_ = '\0';
                        dbtr_expire_package.send_to_buyer.item_info.abilityValue_ = 0;
                        CNRDItemInfoList::STItemInfo* pSVar9 =
                            mpAuction->GetItemInfo(ptr_data->item_info.item_id);
                        unsigned int item_category = (unsigned int)pSVar9->category_;
                        if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
                        {
                            char* pacVar10 = mpAuction->IsAvatarCategory(item_category)
                                ? mpAuction->GetAvatarColorName(
                                      ptr_data->item_info.add_info)
                                : LETTER_TEXT[7];
                            snprintf(dbtr_expire_package.send_to_buyer.letter_text, 0xff,
                                     LETTER_TEXT[6],
                                     mpAuction->GetItemInfo(ptr_data->item_info.item_id)
                                         ->sName_.c_str(),
                                     pacVar10);
                        }
                        else
                        {
                            char* pacVar10 = mpAuction->IsAvatarCategory(item_category)
                                ? mpAuction->GetAvatarColorName(
                                      ptr_data->item_info.add_info)
                                : LETTER_TEXT[7];
                            char* ptVar13;
                            if (*(int*)&ptr_data->_reg_roi_category_key.field_0 == 0 &&
                                *(int*)((char*)&ptr_data->_reg_roi_category_key.field_0 + 4) == 0)
                            {
                                ptVar13 = (char*)
                                    mpAuction->GetItemInfo(ptr_data->item_info.item_id)
                                        ->sName_.c_str();
                            }
                            else
                            {
                                ptVar13 = _itemName;
                            }
                            snprintf(dbtr_expire_package.send_to_buyer.letter_text, 0xff,
                                     LETTER_TEXT[6], ptVar13, pacVar10);
                        }
                        dbtr_expire_package.send_to_buyer.letter_text_length =
                            (unsigned short)strlen(
                                dbtr_expire_package.send_to_buyer.letter_text);
                        dbtr_expire_package.send_to_buyer.temp_item_id =
                            ptr_data->item_info.item_id;
                    }
                    dbtr_expire_package.send_to_owner.owner_id = ptr_data->owner_id;
                    dbtr_expire_package.send_to_owner.receiver = ptr_data->owner_id;
                    dbtr_expire_package.send_to_owner.money = 0;
                    dbtr_expire_package.send_to_owner.item_info = ptr_data->item_info;
                    if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
                    {
                        CNRDItemInfoList::STItemInfo* pSVar9 =
                            mpAuction->GetItemInfo(ptr_data->item_info.item_id);
                        unsigned int item_category = (unsigned int)pSVar9->category_;
                        char* pacVar10 = mpAuction->IsAvatarCategory(item_category)
                            ? mpAuction->GetAvatarColorName(
                                  dbtr_expire_package.send_to_owner.item_info.add_info)
                            : LETTER_TEXT[7];
                        snprintf(dbtr_expire_package.send_to_owner.letter_text, 0xff,
                                 LETTER_TEXT[7],
                                 mpAuction->GetItemInfo(ptr_data->item_info.item_id)
                                     ->sName_.c_str(),
                                 pacVar10);
                    }
                    else
                    {
                        CNRDItemInfoList::STItemInfo* pSVar9 =
                            mpAuction->GetItemInfo(ptr_data->item_info.item_id);
                        unsigned int item_category = (unsigned int)pSVar9->category_;
                        char* pacVar10 = mpAuction->IsAvatarCategory(item_category)
                            ? mpAuction->GetAvatarColorName(
                                  dbtr_expire_package.send_to_owner.item_info.add_info)
                            : LETTER_TEXT[7];
                        char* ptVar13;
                        if (*(int*)&ptr_data->_reg_roi_category_key.field_0 == 0 &&
                            *(int*)((char*)&ptr_data->_reg_roi_category_key.field_0 + 4) == 0)
                        {
                            ptVar13 = (char*)
                                mpAuction->GetItemInfo(ptr_data->item_info.item_id)
                                    ->sName_.c_str();
                        }
                        else
                        {
                            ptVar13 = _itemName;
                        }
                        snprintf(dbtr_expire_package.send_to_owner.letter_text, 0xff,
                                 LETTER_TEXT[7], ptVar13, pacVar10);
                    }
                    dbtr_expire_package.send_to_owner.temp_item_id =
                        ptr_data->item_info.item_id;
                    dbtr_expire_package.send_to_owner.letter_text_length =
                        (unsigned short)strlen(
                            dbtr_expire_package.send_to_owner.letter_text);
                    CommonDataPool* pPool2 =
                        pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
                    Message* pMsg2 = pPool2->createMessage(3);
                    CMsgCell* pNewCell2 = pMsg2->getCellFromMessage();
                    *pNewCell2 << &dbtr_expire_package;
                    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg2);
                    int item_type =
                        mpAuction->CheckItemType(ptr_data->item_info.item_id);
                    if (item_type == 2)
                    {
                        mpAuction->UnregistChkMapForAvatarCreature(
                            false, ptr_data->item_info.add_info);
                    }
                    else if (item_type == 3)
                    {
                        mpAuction->UnregistChkMapForAvatarCreature(
                            true, ptr_data->item_info.add_info);
                    }
                    CNRDItemInfoList::STItemInfo* pSVar9 =
                        mpAuction->GetItemInfo(ptr_data->item_info.item_id);
                    if (mpAuction->IsAvatarCategory(pSVar9->category_))
                    {
                        mpAuction->SubAvatarEmblemInfo(ptr_data->item_info.add_info);
                        mpAuction->SubAvatarExpansionInfo(ptr_data->item_info.add_info);
                    }
                    ptr_data->expire_table_ptr->auction_id = 0;
                    mAuctionDicDataPool.free(ptr_data);
                    error_code = 0;
                }
            }
        }
    }
    else
    {
        error_code = 0x2b;
    }
    return error_code;
}
