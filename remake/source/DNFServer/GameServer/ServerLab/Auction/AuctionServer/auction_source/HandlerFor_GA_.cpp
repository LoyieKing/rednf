// Rebuilt from df_auction_r DWARF (HandlerFor_GA_.cpp CU), 2026-08-08
#include <algorithm>
#include <stdio.h>
#include <string.h>

#include "HandlerFor_GA_.h"
#include "TraceLog.h"
#include "Script.h"
#include "Thread.h"
#include "ServiceFactory.h"
#include "DataPools.h"
#include "CommonDataPool.h"
#include "NSLDBThread.h"
#include "Message.h"
#include "TCPSendThread.h"
#include "Zone.h"
#include "GameDataPool.h"
#include "Character.h"
#include "StatisticsCollector.h"
#include "ServiceError.h"

using namespace nsl;

static int gmList[5] = {0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae};

void PrintDnfItemInfo(DnfItemInfo& itemInfo, char* out);

unsigned int HandlerFor_GA_::onAUCTION_REGIST_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    G_TraceLog()->sysLog(5, "In  onAUCTION_REGIST_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_REGIST_GA:this is not gold server");
        return 1;
    }
    // ORIG：无 pPool 命名局部，getCommonDataPool 结果直喂 createCharacter（eax 直传）
    auction::Character* pCharacter =
        ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))->createCharacter();
    pCharacter->setPassiveTCPUser(u);
    PCK_AUCTION_REGIST_ACK_AG ackPck;
    GSArea* pArea = (GSArea*)G_Zone()->mArea[0];
    if (pCharacter->setArea(0))
    {
        ackPck.ack = 0;
        if (!pArea->IsServiceRunning())
        {
            ackPck.ack = 2;
        }
    }
    else
    {
        pCharacter->setCharacKey(pArea->GetSpareKey());
        pCharacter->setArea(0);
        ackPck.ack = 1;
    }
    // ORIG DWARF：pNewMsg/pNewCell（声明序在 pArea 之后）
    Message* pNewMsg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
    *pNewCell << &ackPck;
    pNewCell->PAD();
    sendTCP_->PushSendMsg(pNewMsg);
    u->setBindedSession(true);
    G_TraceLog()->sysLog(5, "Out  onAUCTION_REGIST_GA");
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_ASK_AVERAGE_PRICE_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    int result = 0;
    PCK_AUCTION_ASK_AVERAGE_PRICE_AG packet;

    G_TraceLog()->sysLog(5, "In  onAUCTION_ASK_AVERAGE_PRICE_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_ASK_AVERAGE_PRICE_GA:this is not gold server");
        return 1;
    }
    PCK_AUCTION_ASK_AVERAGE_PRICE_GA* pPck =
        (PCK_AUCTION_ASK_AVERAGE_PRICE_GA*)pCell->GetPacket();
    ROI_AverageKey _temp_roi_average;
    _temp_roi_average.baseItem_index = pPck->item_id;
    _temp_roi_average.option_category.field_0._high_category_key =
        *(unsigned long long*)&pPck->roi_search_key;
    _temp_roi_average.option_category.field_1._low_category_key =
        *(int*)((char*)&pPck->roi_search_key + 8);
    for (int i = 0; i <= 2; i = i + 1)
    {
        _temp_roi_average._oiv.option_index_value[i] = (short)pPck->randomOption_index[i];
    }
    std::sort(_temp_roi_average._oiv.option_index_value,
              _temp_roi_average._oiv.option_index_value + 3);
    result = G_Auction()->GetAveragePrice(pPck->item_id, pPck->item_upgrade_value,
                                          _temp_roi_average, pPck->item_refine_value,
                                          &packet.average_price);
    packet.char_idx = pPck->char_idx;
    packet.character_id = pPck->character_id;
    PCK_AUCTION_SEARCH_BY_ITEMKEY_GA searchPck;
    PCK_AUCTION_ITEM_LIST_AG listPck;
    searchPck.tSearchByItemId.startIndex = 0;
    searchPck.tSearchByItemId.itemIdNum = 1;
    searchPck.tSearchByItemId.category = 0;
    searchPck.tSearchByItemId.upgradeStart = pPck->item_upgrade_value;
    searchPck.tSearchByItemId.upgradeEnd = pPck->item_upgrade_value;
    searchPck.item_id_array[0] = pPck->item_id;
    *(int*)((char*)&searchPck.tSearchByItemId.ROI_Search_category_key + 0) =
        *(int*)((char*)&pPck->roi_search_key + 0);
    *(int*)((char*)&searchPck.tSearchByItemId.ROI_Search_category_key + 4) =
        *(int*)((char*)&pPck->roi_search_key + 4);
    *(int*)((char*)&searchPck.tSearchByItemId.ROI_Search_category_key + 8) =
        *(int*)((char*)&pPck->roi_search_key + 8);
    result = G_Auction()->SearchByItemId(&searchPck.tSearchByItemId,
                                         searchPck.item_id_array,
                                         &listPck.total_item_num, &listPck.item_num,
                                         listPck.item_info);
    if (result != 0)
    {
        return result;
    }
    for (int i = 0; (i < (int)(unsigned int)listPck.item_num) && (i <= 2); i = i + 1)
    {
        // ORIG: reads unit_price (+0x85) and item_info.add_info (+0x30),
        // NOT average_price/price (DWARF: AuctionItemInfo.unit_price@133,
        // DnfItemInfo.add_info@6; verified in both auction and point ORIG).
        packet.min_pirce[i] = listPck.item_info[i].unit_price;
        packet.min_count[i] = listPck.item_info[i].item_info.add_info;
    }
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    for (int i = 0; i < 5; i = i + 1)
    {
        if (gmList[i] == pPck->character_id)
        {
            G_TraceLog()->sysLog(5, "Trace Auction Delay, %s(%d)",
                                 "ACK ASK_AVERAGE_PRICE_GA", pPck->character_id);
            break;
        }
    }
    G_TraceLog()->sysLog(5, "Out onAUCTION_ASK_AVERAGE_PRICE_GA");
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_ASK_REGISTED_ITEM_NUM_GA(nsl::CMsgCell* pCell,
                                                                nsl::TCPUser* u)
{
    int return_code = 0;
    PCK_AUCTION_ASK_REGISTED_ITEM_NUM_AG packet;

    G_TraceLog()->sysLog(5, "In  onAUCTION_ASK_REGISTED_ITEM_NUM_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_ASK_REGISTED_ITEM_NUM_GA:this is not gold server");
        return 1;
    }
    PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA* pPck =
        (PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA*)pCell->GetPacket();
    {
        // ORIG：inline 比较（无 limit 局部，cmp %eax,%ebx+setle 形态；-1 块 fall-through）
        if (G_Script()->findIntValue(1, 0xd) <= pApp->super_Threads.getDBThread(0)->mQueueSize)
        {
            packet.now_registed_num = -1;
        }
        else
        {
            packet.now_registed_num =
                (short)G_Auction()->GetNowRegistedItemNum(pPck->character_id);
        }
    }
    if (G_Auction()->GetItemInfo(pPck->item_id) == NULL)
    {
        packet.now_registed_num = -2;
    }
    packet.char_idx = pPck->char_idx;
    packet.character_id = pPck->character_id;
    packet.inven_type = pPck->inven_type;
    packet.slot_no = pPck->slot_no;
    packet.item_id = pPck->item_id;
    packet.add_info = pPck->add_info;
    packet.price = pPck->price;
    packet.instant_price = pPck->instant_price;
    packet.unit_price = pPck->unit_price;
    packet.ROI_Category_Key = pPck->ROI_Category_Key;
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    for (int i = 0; i < 5; i = i + 1)
    {
        if (gmList[i] == pPck->character_id)
        {
            G_TraceLog()->sysLog(5, "Trace Auction Delay, %s(%d)",
                                 "ACK ASK_REGISTED_ITEM_NUM_GA", pPck->character_id);
            break;
        }
    }
    G_TraceLog()->sysLog(5, "Out onAUCTION_ASK_REGISTED_ITEM_NUM_GA");
    // ORIG：直接 return 0
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_REGIST_CANCEL_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_REGIST_CANCEL_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_REGIST_CANCEL_GA:this is not gold server");
        return 1;
    }
    G_StatisticsCollector()->IncTryCnt(1);
    PCK_AUCTION_REGIST_CANCEL_GA* pPck =
        (PCK_AUCTION_REGIST_CANCEL_GA*)pCell->GetPacket();
    G_TraceLog()->sysLog(5, "onAUCTION_REGIST_CANCEL_GA(), before RegistCancel(), o_id : %d, au_id : %llu",
                         pPck->owner_id, pPck->auction_id);
    PCK_AUCTION_REGIST_CANCEL_RESULT_AG cancel_result_packet;
    cancel_result_packet.char_idx = pPck->char_idx;
    cancel_result_packet.owner_id = pPck->owner_id;
    return_code = G_Auction()->RegistCancel(pPck->owner_id, pPck->auction_id);
    // ORIG：if (return_code != 0) {失败块} else {成功块} —— else 在 fall-through
    if (return_code != 0)
    {
        cancel_result_packet.cancel_result = 0;
        G_StatisticsCollector()->IncFailCnt(1, return_code);
        cancel_result_packet.result_because =
            (unsigned char)G_Auction()->TransErrToReason(return_code);
    }
    else
    {
        cancel_result_packet.cancel_result = 1;
    }
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &cancel_result_packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    for (int i = 0; i < 5; i = i + 1)
    {
        if (gmList[i] == pPck->owner_id)
        {
            G_TraceLog()->sysLog(5, "Trace Auction Delay, %s(%d)",
                                 "ACK REGIST_ITEM_GA", pPck->owner_id);
            break;
        }
    }
    if (return_code != 0)
    {
        G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_CANCEL_GA, owner:%d, auId:%llu, %s",
                             pPck->owner_id, pPck->auction_id, GetErrorStr(return_code));
    }
    else
    {
        G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_CANCEL_GA");
    }
    // ORIG：直接 return 0（mov $0,%eax 直出，无 return_code 存储）
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_BIDDING_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    // ORIG DWARF 声明序：return_code, packet, pPck, log_packet, ptr_data,
    // temp_result_price, temp_id(块), newMsg/newCell(块), msg/cell, i
    int return_code = 0;
    int temp_result_price;

    G_TraceLog()->sysLog(5, "In  onAUCTION_BIDDING_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_BIDDING_GA:this is not gold server");
        return 1;
    }
    G_StatisticsCollector()->IncTryCnt(2);
    PCK_AUCTION_BIDDING_RESULT_AG packet;
    PCK_AUCTION_BIDDING_GA* pPck = (PCK_AUCTION_BIDDING_GA*)pCell->GetPacket();
    G_TraceLog()->sysLog(5, "onAUCTION_BIDDING_GA(), before Bidding(), b_id : %d , price : %d, au_id : %llu",
                         pPck->buyer_id, pPck->price, pPck->auction_id);
    PCK_AUCTION_LOG_MESSAGE_AG log_packet;
    AuctionDictionary::AuctionDictionaryData* ptr_data;
    log_packet.char_idx = pPck->char_idx;
    log_packet.auction_log_type = 0x02;
    log_packet.price = pPck->price;
    log_packet.auction_id = pPck->auction_id;
    ptr_data = G_Auction()->GetAuctionDicData(pPck->auction_id);
    if (ptr_data != NULL)
    {
        log_packet.character_id = ptr_data->owner_id;
        log_packet.item_id = ptr_data->item_info.item_id;
    }
    temp_result_price = 0;
    {
        char temp_id[32];
        // ORIG：inline 比较（无 limit 局部；setle 物化，0x31 块 fall-through）
        if (G_Script()->findIntValue(1, 0xd) <= pApp->super_Threads.getDBThread(0)->mQueueSize)
        {
            return_code = 0x31;
        }
        else
        {
            return_code = G_Auction()->Bidding(pPck->buyer_id, pPck->buyer_name,
                                               pPck->auction_id, pPck->price,
                                               temp_id, temp_result_price);
        }
    }
    if (return_code == 0)
    {
        packet.bidding_result = 1;
    }
    else
    {
        packet.bidding_result = 0;
        log_packet.auction_log_type = 0x03;
        G_TraceLog()->sysLog(5, "onAUCTION_BIDDING_GA, G_Auction()->Bidding() %s",
                             GetErrorStr(return_code));
        G_StatisticsCollector()->IncFailCnt(2, return_code);
        packet.result_because =
            (unsigned char)G_Auction()->TransErrToReason(return_code);
    }
    packet.char_idx = pPck->char_idx;
    packet.buyer_id = pPck->buyer_id;
    packet.auction_id = pPck->auction_id;
    packet.price = pPck->price;
    // ORIG DWARF：newMsg/newCell 先声明（槽位 -0x2c/-0x28），msg/cell 后声明（-0x24/-0x20）；
    // 执行序是 msg/cell（log_packet 先发）在前、newMsg/newCell（packet 后发）在后
    Message* newMsg;
    CMsgCell* newCell;
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &log_packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    newMsg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    newCell = newMsg->getCellFromMessage();
    *newCell << &packet;
    newCell->PAD();
    sendTCP_->PushSendMsg(newMsg);
    for (int i = 0; i < 5; i = i + 1)
    {
        if (gmList[i] == pPck->buyer_id)
        {
            G_TraceLog()->sysLog(5, "Trace Auction Delay, %s(%d)",
                                 "ACK BIDDING_GA", pPck->buyer_id);
            break;
        }
    }
    G_TraceLog()->sysLog(5, "Out onAUCTION_BIDDING_GA");
    // ORIG：直接 return 0（mov $0 直出）
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_SEARCH_BY_ITEMKEY_GA(nsl::CMsgCell* pCell,
                                                            nsl::TCPUser* u)
{
    unsigned int result;

    G_TraceLog()->sysLog(5, "In  onAUCTION_SEARCH_BY_ITEMKEY_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_SEARCH_BY_ITEMKEY_GA:this is not gold server");
        return 1;
    }
    PCK_AUCTION_SEARCH_BY_ITEMKEY_GA* pPck =
        (PCK_AUCTION_SEARCH_BY_ITEMKEY_GA*)pCell->GetPacket();
    PCK_AUCTION_ITEM_LIST_AG packet;
    result = G_Auction()->SearchByItemId(&pPck->tSearchByItemId,
                                         pPck->item_id_array,
                                         &packet.total_item_num, &packet.item_num,
                                         packet.item_info);
    if (result != 0)
    {
        return result;
    }
    {
        packet.char_idx = pPck->char_idx;
        packet.character_id = pPck->character_id;
        packet.pay_type = (unsigned char)G_Auction()->GetPayType();
        for (int i = 0; i < (int)(unsigned int)packet.item_num; i = i + 1)
        {
            CNRDItemInfoList::STItemInfo* pItemInfo =
                G_Auction()->GetItemInfo(packet.item_info[i].item_info.item_id);
            if (pItemInfo != NULL)
            {
                int item_category = (int)pItemInfo->category_;
                if (G_Auction()->IsAvatarCategory(item_category))
                {
                    G_Auction()->GetAvatarEmblemInfo(
                        packet.item_info[i].item_info.add_info,
                        &packet.item_info[i].emblem_info);
                    G_Auction()->GetAvatarExpansionInfo(
                        packet.item_info[i].item_info.add_info,
                        &packet.item_info[i].expansion);
                    packet.item_info[i].item_info.add_info = 0;
                }
            }
        }
        packet.setSize(0x203d - (0x3c - (unsigned int)packet.item_num) * 0x89);
        // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
        Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
        CMsgCell* cell = msg->getCellFromMessage();
        *cell << &packet;
        cell->PAD();
        sendTCP_->PushSendMsg(msg);
        for (int i = 0; i < 5; i = i + 1)
        {
            if (gmList[i] == pPck->character_id)
            {
                G_TraceLog()->sysLog(5, "Trace Auction Delay, %s(%d)",
                                     "ACK SEARCH_BY_ITEMKEY_GA", pPck->character_id);
                break;
            }
        }
        G_TraceLog()->sysLog(5, "Out onAUCTION_SEARCH_BY_ITEMKEY_GA");
    }
    return result;
}

unsigned int HandlerFor_GA_::onAUCTION_MY_REGISTED_ITEM_INFO_GA(nsl::CMsgCell* pCell,
                                                                nsl::TCPUser* u)
{
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_MY_REGISTED_ITEM_INFO_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_MY_REGISTED_ITEM_INFO_GA:this is not gold server");
        return 1;
    }
    PCK_AUCTION_MY_REGISTED_ITEM_INFO_AG packet;
    int packet_array_size = 0x1e;
    PCK_AUCTION_MY_REGISTED_ITEM_INFO_GA* pPck =
        (PCK_AUCTION_MY_REGISTED_ITEM_INFO_GA*)pCell->GetPacket();
    return_code = G_Auction()->GetMyRegistedItemInfo(
        pPck->owner_id, &packet_array_size, packet.my_registed_item_info);
    if (return_code != 0)
    {
        G_TraceLog()->sysLog(5, "Out onAUCTION_MY_REGISTED_ITEM_INFO_GA, %s",
                             GetErrorStr(return_code));
        return 0;
    }
    packet.char_idx = pPck->char_idx;
    packet.owner_id = pPck->owner_id;
    packet.pay_type = (unsigned char)G_Auction()->GetPayType();
    packet.item_num = (unsigned char)packet_array_size;
    for (int i = 0; i < (int)(unsigned int)packet.item_num; i = i + 1)
    {
        CNRDItemInfoList::STItemInfo* pItemInfo =
            G_Auction()->GetItemInfo(
                packet.my_registed_item_info[i].item_info.item_id);
        if (pItemInfo != NULL)
        {
            int item_category = (int)pItemInfo->category_;
            if (G_Auction()->IsAvatarCategory(item_category))
            {
                G_Auction()->GetAvatarEmblemInfo(
                    packet.my_registed_item_info[i].item_info.add_info,
                    &packet.my_registed_item_info[i].emblem_info);
                G_Auction()->GetAvatarExpansionInfo(
                    packet.my_registed_item_info[i].item_info.add_info,
                    &packet.my_registed_item_info[i].expansion);
                packet.my_registed_item_info[i].item_info.add_info = 0;
            }
        }
    }
    packet.setSize(0xdd2 - (0x1e - (unsigned int)packet_array_size) * 0x75);
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    for (int i = 0; i < 5; i = i + 1)
    {
        if (gmList[i] == pPck->owner_id)
        {
            G_TraceLog()->sysLog(5, "Trace Auction Delay, %s(%d)",
                                 "ACK MY_REGISTED_ITEM_INFO_GA", pPck->owner_id);
            break;
        }
    }
    G_TraceLog()->sysLog(5, "Out onAUCTION_MY_REGISTED_ITEM_INFO_GA");
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_MY_BIDDING_INFO_GA(nsl::CMsgCell* pCell,
                                                          nsl::TCPUser* u)
{
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_MY_BIDDING_INFO_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_MY_BIDDING_INFO_GA:this is not gold server");
        return 1;
    }
    PCK_AUCTION_MY_BIDDING_INFO_AG packet;
    int packet_array_size = 0x3c;
    PCK_AUCTION_MY_BIDDING_INFO_GA* pPck =
        (PCK_AUCTION_MY_BIDDING_INFO_GA*)pCell->GetPacket();
    return_code = G_Auction()->GetMyBiddingInfo(
        pPck->buyer_id, &packet_array_size, packet.my_bidding_item_info);
    if (return_code != 0)
    {
        G_TraceLog()->sysLog(5, "Out onAUCTION_MY_BIDDING_INFO_GA, %s",
                             GetErrorStr(return_code));
        return 0;
    }
    packet.char_idx = pPck->char_idx;
    packet.buyer_id = pPck->buyer_id;
    packet.pay_type = (unsigned char)G_Auction()->GetPayType();
    packet.item_num = (unsigned short)packet_array_size;
    for (int i = 0; i < (int)(unsigned int)packet.item_num; i = i + 1)
    {
        CNRDItemInfoList::STItemInfo* pItemInfo =
            G_Auction()->GetItemInfo(
                packet.my_bidding_item_info[i].item_info.item_id);
        if (pItemInfo != NULL)
        {
            int item_category = (int)pItemInfo->category_;
            if (G_Auction()->IsAvatarCategory(item_category))
            {
                G_Auction()->GetAvatarEmblemInfo(
                    packet.my_bidding_item_info[i].item_info.add_info,
                    &packet.my_bidding_item_info[i].emblem_info);
                G_Auction()->GetAvatarExpansionInfo(
                    packet.my_bidding_item_info[i].item_info.add_info,
                    &packet.my_bidding_item_info[i].expansion);
                packet.my_bidding_item_info[i].item_info.add_info = 0;
            }
        }
    }
    packet.setSize(0x1d69 - (0x3c - (unsigned int)packet_array_size) * 0x7d);
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    for (int i = 0; i < 5; i = i + 1)
    {
        if (gmList[i] == pPck->buyer_id)
        {
            G_TraceLog()->sysLog(5, "Trace Auction Delay, %s(%d)",
                                 "ACK MY_BIDDING_INFO_GA", pPck->buyer_id);
            break;
        }
    }
    G_TraceLog()->sysLog(5, "Out onAUCTION_MY_BIDDING_INFO_GA");
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_MY_AUCTION_HISTORY_GA(nsl::CMsgCell* pCell,
                                                             nsl::TCPUser* u)
{
    G_TraceLog()->sysLog(5, "In  onAUCTION_MY_AUCTION_HISTORY_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_MY_AUCTION_HISTORY_GA:this is not gold server");
        return 1;
    }
    G_TraceLog()->sysLog(5, "Out onAUCTION_MY_AUCTION_HISTORY_GA");
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_OPEN_PRIVATE_STORE_GA(nsl::CMsgCell* pCell,
                                                             nsl::TCPUser* u)
{
    G_TraceLog()->sysLog(5, "In  onAUCTION_OPEN_PRIVATE_STORE_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_OPEN_PRIVATE_STORE_GA:this is not gold server");
        return 1;
    }
    PCK_AUCTION_OPEN_PRIVATE_STORE_GA* pPck =
        (PCK_AUCTION_OPEN_PRIVATE_STORE_GA*)pCell->GetPacket();
    G_Auction()->OpenPrivateStore(pPck->m_id, pPck->character_num,
                                  pPck->character_ids);
    G_TraceLog()->sysLog(5, "Out onAUCTION_OPEN_PRIVATE_STORE_GA");
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_CLOSE_PRIVATE_STORE_GA(nsl::CMsgCell* pCell,
                                                              nsl::TCPUser* u)
{
    G_TraceLog()->sysLog(5, "In  onAUCTION_CLOSE_PRIVATE_STORE_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_CLOSE_PRIVATE_STORE_GA:this is not gold server");
        return 1;
    }
    PCK_AUCTION_CLOSE_PRIVATE_STORE_GA* pPck =
        (PCK_AUCTION_CLOSE_PRIVATE_STORE_GA*)pCell->GetPacket();
    // ORIG：DWARF 局部含死局部 mId（pPck->m_id 先存栈，调用处仍重读）；
    // 缺它会少 3 条指令、pPck 槽位偏移 -4。
    memberIdentificationNumber_t mId = pPck->m_id;
    G_Auction()->ClosePrivateStore(pPck->m_id);
    G_TraceLog()->sysLog(5, "Out onAUCTION_CLOSE_PRIVATE_STORE_GA");
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_CHECK_AUCION_READY_GA(nsl::CMsgCell* pCell,
                                                             nsl::TCPUser* u)
{
    G_TraceLog()->sysLog(5, "In  onAUCTION_CHECK_AUCION_READY_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_CHECK_AUCION_READY_GA:this is not gold server");
        return 1;
    }
    PCK_AUCTION_CHECK_AUCTION_READY_AG packet;
    GSArea* pArea = (GSArea*)G_Zone()->mArea[0];
    // ORIG：if/else 显式赋 bool（test+je; movb 1; jmp; movb 0），直赋只有 mov %al。
    if (pArea->IsServiceRunning())
    {
        packet.db_work_done = true;
    }
    else
    {
        packet.db_work_done = false;
    }
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    G_TraceLog()->sysLog(5, "Out onAUCTION_CHECK_AUCION_READY_GA");
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_BUY_ITEM_APIECE_GA(nsl::CMsgCell* pCell,
                                                          nsl::TCPUser* u)
{
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_BUY_ITEM_APIECE_GA");
    G_StatisticsCollector()->IncTryCnt(2);
    PCK_AUCTION_BUY_ITEM_APIECE_GA* pPck =
        (PCK_AUCTION_BUY_ITEM_APIECE_GA*)pCell->GetPacket();
    G_TraceLog()->sysLog(5, "onAUCTION_BUY_ITEM_APIECE_GA(), before Bidding(), b_id : %d , price : %d, au_id : %llu, count : %d",
                         pPck->buyer_id, pPck->price, pPck->auction_id, pPck->count);
    PCK_AUCTION_BUY_ITEM_APIECE_AG packet;
    packet.char_idx = pPck->char_idx;
    packet.buyer_id = pPck->buyer_id;
    packet.auction_id = pPck->auction_id;
    packet.price = pPck->price;
    packet.bidding_result = 1;
    PCK_AUCTION_LOG_MESSAGE_AG log_packet;
    log_packet.char_idx = pPck->char_idx;
    log_packet.auction_log_type = 0x04;
    log_packet.price = pPck->price;
    log_packet.auction_id = pPck->auction_id;
    log_packet.add_info = pPck->count;
    AuctionDictionary::AuctionDictionaryData* ptr_data;
    ptr_data = G_Auction()->GetAuctionDicData(pPck->auction_id);
    if (ptr_data != NULL)
    {
        log_packet.character_id = ptr_data->owner_id;
        log_packet.item_id = ptr_data->item_info.item_id;
        log_packet.uniItemAttr = ptr_data->item_info.uniItemAttr;
    }
    // ORIG：inline 比较（cmp+setle 物化，0x31 块 fall-through，BuyItemApiece 在跳转目标）
    {
        if (G_Script()->findIntValue(1, 0xd) <= pApp->super_Threads.getDBThread(0)->mQueueSize)
        {
            return_code = 0x31;
        }
        else
        {
            return_code = G_Auction()->BuyItemApiece(pPck->auction_id, pPck->buyer_id,
                                                     pPck->price, pPck->count);
        }
    }
    if (return_code != 0)
    {
        packet.bidding_result = 0;
        log_packet.auction_log_type = 0x03;
        G_TraceLog()->sysLog(5, "onAUCTION_BIDDING_GA, G_Auction()->Bidding() %s",
                             GetErrorStr(return_code));
        G_StatisticsCollector()->IncFailCnt(2, return_code);
        packet.result_because =
            (unsigned char)G_Auction()->TransErrToReason(return_code);
    }
    // ORIG DWARF：msg/cell（log_packet 先发）槽位 -0x28/-0x24 深于 newMsg/newCell
    // （-0x20/-0x1c）；按执行序声明。
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &log_packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    Message* newMsg;
    CMsgCell* newCell;
    newMsg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    newCell = newMsg->getCellFromMessage();
    *newCell << &packet;
    newCell->PAD();
    sendTCP_->PushSendMsg(newMsg);
    G_TraceLog()->sysLog(5, "Out onAUCTION_BUY_ITEM_APIECE_GA");
    return 0;
}

unsigned int HandlerFor_GA_::onAUCTION_SEARCH_BY_NOITEMKEY_GA(nsl::CMsgCell* pCell,
                                                              nsl::TCPUser* u)
{
    unsigned int result;

    G_TraceLog()->sysLog(5, "In  onAUCTION_SEARCH_BY_NOITEMKEY_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_SEARCH_BY_NOITEMKEY_GA:this is not gold server");
        return 1;
    }
    PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA* pPck =
        (PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA*)pCell->GetPacket();
    PCK_AUCTION_ITEM_LIST_AG packet;
    result = G_Auction()->SearchByCategory(&pPck->tSearchByCategory,
                                           &packet.total_item_num, &packet.item_num,
                                           packet.item_info);
    if (result != 0)
    {
        return result;
    }
    {
        packet.char_idx = pPck->char_idx;
        packet.character_id = pPck->character_id;
        packet.pay_type = (unsigned char)G_Auction()->GetPayType();
        for (int i = 0; i < (int)(unsigned int)packet.item_num; i = i + 1)
        {
            CNRDItemInfoList::STItemInfo* pItemInfo =
                G_Auction()->GetItemInfo(packet.item_info[i].item_info.item_id);
            if (pItemInfo != NULL)
            {
                int item_category = (int)pItemInfo->category_;
                if (G_Auction()->IsAvatarCategory(item_category))
                {
                    G_Auction()->GetAvatarEmblemInfo(
                        packet.item_info[i].item_info.add_info,
                        &packet.item_info[i].emblem_info);
                    G_Auction()->GetAvatarExpansionInfo(
                        packet.item_info[i].item_info.add_info,
                        &packet.item_info[i].expansion);
                    packet.item_info[i].item_info.add_info = 0;
                }
            }
        }
        packet.setSize(0x203d - (0x3c - (unsigned int)packet.item_num) * 0x89);
        // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
        Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
        CMsgCell* cell = msg->getCellFromMessage();
        *cell << &packet;
        cell->PAD();
        sendTCP_->PushSendMsg(msg);
        for (int i = 0; i < 5; i = i + 1)
        {
            if (gmList[i] == pPck->character_id)
            {
                G_TraceLog()->sysLog(5, "Trace Auction Delay, %s(%d)",
                                     "ACK SEARCH_BY_NOITEMKEY_GA", pPck->character_id);
                break;
            }
        }
        G_TraceLog()->sysLog(5, "Out onAUCTION_SEARCH_BY_NOITEMKEY_GA");
    }
    return result;
}

unsigned int HandlerFor_GA_::onAUCTION_REGIST_ITEM_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_REGIST_ITEM_GA");
    if (!IsGoldServer())
    {
        G_TraceLog()->sysLog(7, "Out onAUCTION_REGIST_ITEM_GA:this is not gold server");
        return 1;
    }
    G_StatisticsCollector()->IncTryCnt(0);
    PCK_AUCTION_REGIST_ITEM_GA* pPck = (PCK_AUCTION_REGIST_ITEM_GA*)pCell->GetPacket();
    pPck->_reg_roi_category_key._sort();
    PrintDnfItemInfo(pPck->item_info, mpSzBuffer);
    G_TraceLog()->sysLog(5, "onAUCTION_REGIST_ITEM_GA(), before RegistItem(), o_id : %d , item : %s",
                         pPck->owner_id, mpSzBuffer);
    PCK_AUCTION_LOG_MESSAGE_AG log_packet;
    log_packet.char_idx = pPck->char_idx;
    log_packet.auction_id = 0;
    log_packet.auction_log_type = 0;
    log_packet.character_id = pPck->owner_id;
    log_packet.price = pPck->price;
    log_packet.item_id = pPck->item_info.item_id;
    log_packet.uniItemAttr = pPck->item_info.uniItemAttr;
    log_packet.add_info = pPck->item_info.add_info;
    log_packet.endurance = pPck->item_info.endurance;
    log_packet.extendInfo = pPck->item_info.extendInfo;
    return_code = G_Auction()->RegistItem(
        pPck->owner_id, pPck->owner_name, pPck->user_type, pPck->price,
        pPck->instant_price, pPck->black_point, pPck->unit_price,
        pPck->_reg_roi_category_key, pPck->owner_nexon_id, pPck->item_info,
        log_packet.auction_id, 0, -1, NULL, false);
    PCK_AUCTION_REGIST_RESULT_AG result_packet;
    result_packet.char_idx = pPck->char_idx;
    result_packet.owner_id = pPck->owner_id;
    int item_category = -1;
    if (return_code != 0)
    {
        result_packet.regist_result = 0;
        log_packet.auction_log_type = 1;
        G_StatisticsCollector()->IncFailCnt(0, return_code);
        result_packet.result_because =
            (unsigned char)G_Auction()->TransErrToReason(return_code);
        log_packet.result_because =
            (unsigned char)G_Auction()->TransErrToReason(return_code);
    }
    else
    {
        result_packet.regist_result = 1;
        CNRDItemInfoList::STItemInfo* pItemInfo =
            G_Auction()->GetItemInfo(pPck->item_info.item_id);
        if (pItemInfo != NULL)
        {
            item_category = (int)pItemInfo->category_;
            if (G_Auction()->IsAvatarCategory(item_category))
            {
                G_Auction()->AddAvatarEmblemInfo(pPck->item_info.add_info,
                                                 &pPck->jewel_info);
                G_Auction()->AddAvatarExpansionInfo(pPck->item_info.add_info,
                                                    &pPck->expansion);
            }
        }
    }
    // ORIG DWARF：两次发送各占独立词法块（msg/cell 各一对，槽位 -0x2c/-0x28、-0x24/-0x20）
    {
        Message* msg =
            pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
        CMsgCell* cell = msg->getCellFromMessage();
        *cell << &log_packet;
        cell->PAD();
        sendTCP_->PushSendMsg(msg);
    }
    {
        Message* msg =
            pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
        CMsgCell* cell = msg->getCellFromMessage();
        *cell << &result_packet;
        cell->PAD();
        sendTCP_->PushSendMsg(msg);
    }
    for (int i = 0; i < 5; i = i + 1)
    {
        if (gmList[i] == pPck->owner_id)
        {
            G_TraceLog()->sysLog(5, "Trace Auction Delay, %s(%d)",
                                 "ACK REGIST_ITEM_GA", pPck->owner_id);
            break;
        }
    }
    if (return_code != 0)
    {
        if (return_code == 9)
        {
            G_TraceLog()->sysLog(7, "Out onAUCTION_REGIST_ITEM_GA , auction_id : %llu, %s",
                                 log_packet.auction_id, GetErrorStr(return_code));
            return return_code;
        }
        {
            G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_ITEM_GA , auction_id : %llu, %s",
                                 log_packet.auction_id, GetErrorStr(return_code));
        }
    }
    else
    {
        G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_ITEM_GA, auction_id : %llu",
                             log_packet.auction_id);
    }
    // ORIG：直接 return 0
    return 0;
}
