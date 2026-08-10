// Rebuilt from df_auction_r DWARF (HandlerFor_GP_JPN.cpp CU), 2026-08-08
#include <stdio.h>
#include <string.h>

#include "HandlerFor_GP_JPN.h"
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

void PrintDnfItemInfo(DnfItemInfo& itemInfo, char* out);

unsigned int HandlerFor_GP_::onAUCTION_REGIST_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    G_TraceLog()->sysLog(5, "In  onAUCTION_REGIST_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_REGIST_GP:this is not point server");
        return 1;
    }

    nsl::PACKET_HEADER* pPck = pCell->GetPacket();
    (void)pPck;
    auction::Character* pCharacter =
        ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
            ->createCharacter();
    pCharacter->setPassiveTCPUser(u);
    PCK_AUCTION_REGIST_ACK_PG ackPck;
    GSArea* pArea = (GSArea*)G_Zone()->mArea[0];
    if (pCharacter->setArea(0))
    {
        ackPck.ack = 0;
        if (!pArea->IsServiceRunning())
        {
            ackPck.ack = 0x35;
        }
    }
    else
    {
        pCharacter->setCharacKey(pArea->GetSpareKey());
        pCharacter->setArea(0);
        ackPck.ack = 0x36;
    }
    Message* pNewMsg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
    *pNewCell << &ackPck;
    pNewCell->PAD();
    sendTCP_->PushSendMsg(pNewMsg);
    u->setBindedSession(true);
    G_TraceLog()->sysLog(5, "Out  onAUCTION_REGIST_GP");
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_ASK_AVERAGE_PRICE_GP(nsl::CMsgCell* pCell,
                                                            nsl::TCPUser* u)
{
    int return_code = 0;
    PCK_AUCTION_ASK_AVERAGE_PRICE_PG packet;

    G_TraceLog()->sysLog(5, "In  onAUCTION_ASK_AVERAGE_PRICE_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_ASK_AVERAGE_PRICE_GP:this is not point server");
        return 1;
    }

    PCK_AUCTION_ASK_AVERAGE_PRICE_GA* pPck =
        (PCK_AUCTION_ASK_AVERAGE_PRICE_GA*)pCell->GetPacket();
    ROI_AverageKey roiAverageKey;
    char itemRefineValue = 0;
    int AvePrice = 0;
    (void)AvePrice;
    return_code = G_Auction()->GetAveragePrice(pPck->item_id,
                                               pPck->item_upgrade_value,
                                               roiAverageKey, itemRefineValue,
                                               &packet.average_price);
    if (packet.average_price < 0)
    {
        packet.average_price = 0;
    }
    packet.char_idx = pPck->char_idx;
    packet.character_id = pPck->character_id;
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    G_TraceLog()->sysLog(5, "Out onAUCTION_ASK_AVERAGE_PRICE_GP");
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_REGIST_CANCEL_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_REGIST_CANCEL_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_REGIST_CANCEL_GP:this is not point server");
        return 1;
    }

    G_StatisticsCollector()->IncTryCnt(1);
    PCK_AUCTION_REGIST_CANCEL_GA* pPck =
        (PCK_AUCTION_REGIST_CANCEL_GA*)pCell->GetPacket();
    G_TraceLog()->sysLog(5, "onAUCTION_REGIST_CANCEL_GP(), before RegistCancel(), o_id : %d, au_id : %llu",
                         pPck->owner_id, pPck->auction_id);
    PCK_AUCTION_REGIST_CANCEL_RESULT_PG cancel_result_packet;
    cancel_result_packet.char_idx = pPck->char_idx;
    cancel_result_packet.owner_id = pPck->owner_id;
    return_code = G_Auction()->RegistCancel(pPck->owner_id, pPck->auction_id);
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
    if (return_code != 0)
    {
        G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_CANCEL_GP, owner:%d, auId:%llu, %s",
                             pPck->owner_id, pPck->auction_id,
                             GetErrorStr(return_code));
    }
    else
    {
        G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_CANCEL_GP");
    }
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_BIDDING_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_BIDDING_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_BIDDING_GP:this is not point server");
        return 1;
    }

    G_StatisticsCollector()->IncTryCnt(2);
    PCK_AUCTION_BIDDING_RESULT_PG packet;
    PCK_AUCTION_BIDDING_GA* pPck = (PCK_AUCTION_BIDDING_GA*)pCell->GetPacket();
    G_TraceLog()->sysLog(5, "onAUCTION_BIDDING_GP(), before Bidding(), b_id : %d , price : %d, au_id : %llu",
                         pPck->buyer_id, pPck->price, pPck->auction_id);
    PCK_AUCTION_LOG_MESSAGE_PG log_packet;
    log_packet.char_idx = pPck->char_idx;
    log_packet.auction_log_type = 0x02;
    log_packet.character_id = pPck->buyer_id;
    log_packet.price = pPck->price;
    log_packet.auction_id = pPck->auction_id;
    {
        // ORIG：register maxQueue（ebx 常驻）
        register int maxQueue = G_Script()->findIntValue(1, 0xd);
        if (maxQueue <= pApp->super_Threads.getDBThread(0)->mQueueSize)
        {
            return_code = 0x31;
        }
        else
        {
            return_code = G_Auction()->Bidding(pPck->buyer_id, pPck->buyer_name,
                                               pPck->auction_id, pPck->price,
                                               packet.owner_nexon_id,
                                               packet.charge_point);
        }
    }
    G_TraceLog()->sysLog(7, "onAUCTION_BIDDING_GP return code : %d", return_code);
    // ORIG：if (return_code == 0) {success} else {fail} —— success 在 fall-through
    if (return_code == 0)
    {
        packet.bidding_result = 1;
    }
    else
    {
        packet.bidding_result = 0;
        log_packet.auction_log_type = 0x03;
        G_TraceLog()->sysLog(5, "onAUCTION_BIDDING_GP, G_Auction()->Bidding() %s",
                             GetErrorStr(return_code));
        G_StatisticsCollector()->IncFailCnt(2, return_code);
        packet.result_because =
            (unsigned char)G_Auction()->TransErrToReason(return_code);
    }
    packet.char_idx = pPck->char_idx;
    packet.buyer_id = pPck->buyer_id;
    packet.auction_id = pPck->auction_id;
    packet.price = pPck->price;
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &log_packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    G_TraceLog()->sysLog(5, "Out onAUCTION_BIDDING_GP");
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_SEARCH_BY_ITEMKEY_GP(nsl::CMsgCell* pCell,
                                                            nsl::TCPUser* u)
{
    unsigned int result;

    G_TraceLog()->sysLog(5, "In  onAUCTION_SEARCH_BY_ITEMKEY_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_SEARCH_BY_ITEMKEY_GP:this is not point server");
        return 1;
    }

    PCK_AUCTION_SEARCH_BY_ITEMKEY_GA* pPck =
        (PCK_AUCTION_SEARCH_BY_ITEMKEY_GA*)pCell->GetPacket();
    PCK_AUCTION_ITEM_LIST_PG packet;
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
                    packet.item_info[i].item_info.add_info = 0;
                }
            }
        }
        packet.setSize((0x3c - (unsigned int)packet.item_num) * -0x89 + 0x203d);
        // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
        Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
        CMsgCell* cell = msg->getCellFromMessage();
        *cell << &packet;
        cell->PAD();
        sendTCP_->PushSendMsg(msg);
        G_TraceLog()->sysLog(5, "Out onAUCTION_SEARCH_BY_ITEMKEY_GP");
    }
    return result;
}

unsigned int HandlerFor_GP_::onAUCTION_SEARCH_BY_NOITEMKEY_GP(nsl::CMsgCell* pCell,
                                                              nsl::TCPUser* u)
{
    unsigned int result;

    G_TraceLog()->sysLog(5, "In  onAUCTION_SEARCH_BY_NOITEMKEY_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_SEARCH_BY_NOITEMKEY_GP:this is not point server");
        return 1;
    }

    PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA* pPck =
        (PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA*)pCell->GetPacket();
    PCK_AUCTION_ITEM_LIST_PG packet;
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
                    packet.item_info[i].item_info.add_info = 0;
                }
            }
        }
        packet.setSize((0x3c - (unsigned int)packet.item_num) * -0x89 + 0x203d);
        // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
        Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
        CMsgCell* cell = msg->getCellFromMessage();
        *cell << &packet;
        cell->PAD();
        sendTCP_->PushSendMsg(msg);
        G_TraceLog()->sysLog(5, "Out onAUCTION_SEARCH_BY_NOITEMKEY_GP");
    }
    return result;
}

unsigned int HandlerFor_GP_::onAUCTION_MY_REGISTED_ITEM_INFO_GP(nsl::CMsgCell* pCell,
                                                                nsl::TCPUser* u)
{
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_MY_REGISTED_ITEM_INFO_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_MY_REGISTED_ITEM_INFO_GP:this is not point server");
        return 1;
    }

    PCK_AUCTION_MY_REGISTED_ITEM_INFO_PG packet;
    int packet_array_size = 0x1e;
    PCK_AUCTION_MY_REGISTED_ITEM_INFO_GA* pPck =
        (PCK_AUCTION_MY_REGISTED_ITEM_INFO_GA*)pCell->GetPacket();
    return_code = G_Auction()->GetMyRegistedItemInfo(
        pPck->owner_id, &packet_array_size, packet.my_registed_item_info);
    if (return_code != 0)
    {
        G_TraceLog()->sysLog(5, "Out onAUCTION_MY_REGISTED_ITEM_INFO_GP, %s",
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
                packet.my_registed_item_info[i].item_info.add_info = 0;
            }
        }
    }
    packet.setSize((0x1e - packet_array_size) * -0x75 + 0xdd2);
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    G_TraceLog()->sysLog(5, "Out onAUCTION_MY_REGISTED_ITEM_INFO_GP");
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_MY_BIDDING_INFO_GP(nsl::CMsgCell* pCell,
                                                          nsl::TCPUser* u)
{
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_MY_BIDDING_INFO_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_MY_BIDDING_INFO_GP:this is not point server");
        return 1;
    }

    PCK_AUCTION_MY_BIDDING_INFO_PG packet;
    int packet_array_size = 0x3c;
    PCK_AUCTION_MY_BIDDING_INFO_GA* pPck =
        (PCK_AUCTION_MY_BIDDING_INFO_GA*)pCell->GetPacket();
    return_code = G_Auction()->GetMyBiddingInfo(
        pPck->buyer_id, &packet_array_size, packet.my_bidding_item_info);
    if (return_code != 0)
    {
        G_TraceLog()->sysLog(5, "Out onAUCTION_MY_BIDDING_INFO_GP, %s",
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
                packet.my_bidding_item_info[i].item_info.add_info = 0;
            }
        }
    }
    // ORIG：0x1d69 - (0x3c - n) * 0x7d（§66：减号分发不可复现，保持内联）
    packet.setSize(0x1d69 - (0x3c - packet_array_size) * 0x7d);
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    G_TraceLog()->sysLog(5, "Out onAUCTION_MY_BIDDING_INFO_GP");
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_MY_AUCTION_HISTORY_GP(nsl::CMsgCell* pCell,
                                                             nsl::TCPUser* u)
{
    G_TraceLog()->sysLog(5, "In  onAUCTION_MY_AUCTION_HISTORY_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_MY_AUCTION_HISTORY_GP:this is not point server");
        return 1;
    }
    G_TraceLog()->sysLog(5, "Out onAUCTION_MY_AUCTION_HISTORY_GP");
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_OPEN_PRIVATE_STORE_GP(nsl::CMsgCell* pCell,
                                                             nsl::TCPUser* u)
{
    G_TraceLog()->sysLog(5, "In  onAUCTION_OPEN_PRIVATE_STORE_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_OPEN_PRIVATE_STORE_GP:this is not point server");
        return 1;
    }
    PCK_AUCTION_OPEN_PRIVATE_STORE_GA* pPck =
        (PCK_AUCTION_OPEN_PRIVATE_STORE_GA*)pCell->GetPacket();
    G_Auction()->OpenPrivateStore(pPck->m_id, pPck->character_num,
                                  pPck->character_ids);
    G_TraceLog()->sysLog(5, "Out onAUCTION_OPEN_PRIVATE_STORE_GP");
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_CLOSE_PRIVATE_STORE_GP(nsl::CMsgCell* pCell,
                                                              nsl::TCPUser* u)
{
    G_TraceLog()->sysLog(5, "In  onAUCTION_CLOSE_PRIVATE_STORE_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_CLOSE_PRIVATE_STORE_GP:this is not point server");
        return 1;
    }
    PCK_AUCTION_CLOSE_PRIVATE_STORE_GA* pPck =
        (PCK_AUCTION_CLOSE_PRIVATE_STORE_GA*)pCell->GetPacket();
    unsigned int m_id = pPck->m_id;
    (void)m_id;
    G_Auction()->ClosePrivateStore(pPck->m_id);
    G_TraceLog()->sysLog(5, "Out onAUCTION_CLOSE_PRIVATE_STORE_GP");
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_CHECK_AUCION_READY_GP(nsl::CMsgCell* pCell,
                                                             nsl::TCPUser* u)
{
    G_TraceLog()->sysLog(5, "In  onAUCTION_CHECK_AUCION_READY_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_CHECK_AUCION_READY_GP:this is not point server");
        return 1;
    }
    PCK_AUCTION_CHECK_AUCTION_READY_PG packet;
    GSArea* pArea = (GSArea*)G_Zone()->mArea[0];
    if (pArea->IsServiceRunning())
    {
        packet.db_work_done = 1;
    }
    else
    {
        packet.db_work_done = 0;
    }
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    G_TraceLog()->sysLog(5, "Out onAUCTION_CHECK_AUCION_READY_GP");
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_ASK_OWNER_IS_VIP_GP(nsl::CMsgCell* pCell,
                                                           nsl::TCPUser* u)
{
    G_TraceLog()->sysLog(5, "In  onAUCTION_ASK_OWNER_IS_VIP_GP");
    PCK_AUCTION_ASK_OWNER_IS_VIP_GP* pPck =
        (PCK_AUCTION_ASK_OWNER_IS_VIP_GP*)pCell->GetPacket();
    OwnerInfo ownerInfo;
    G_Auction()->IsOwnerVIP(pPck->auction_id, ownerInfo);
    ROI_AverageKey roiAverageKey;
    char itemRefineValue = 0;
    int AvePrice = 0;
    int return_code;
    return_code = G_Auction()->GetAveragePrice(pPck->item_id, 0, roiAverageKey,
                                               itemRefineValue,
                                               &ownerInfo.average_price);
    (void)AvePrice;
    (void)return_code;
    if (ownerInfo.average_price < 0)
    {
        ownerInfo.average_price = 0;
    }
    PCK_AUCTION_ASK_OWNER_IS_VIP_PG packet;
    packet.char_idx = pPck->char_idx;
    packet.auction_id = pPck->auction_id;
    packet.is_vip = ownerInfo.is_vip;
    packet.average_price = ownerInfo.average_price;
    packet.buyer_id = pPck->buyer_id;
    strcpy(packet.buyer_name, pPck->buyer_name);
    packet.price = pPck->price;
    packet.item_id = pPck->item_id;
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_ASK_REGISTED_ITEM_NUM_GP(nsl::CMsgCell* pCell,
                                                                nsl::TCPUser* u)
{
    int return_code = 0;
    PCK_AUCTION_ASK_REGISTED_ITEM_NUM_PG packet;

    G_TraceLog()->sysLog(5, "In  onAUCTION_ASK_REGISTED_ITEM_NUM_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_ASK_REGISTED_ITEM_NUM_GP:this is not point server");
        return 1;
    }

    PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA* pPck =
        (PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA*)pCell->GetPacket();
    {
        // ORIG：register maxQueue（ebx 常驻）
        register int maxQueue = G_Script()->findIntValue(1, 0xd);
        if (maxQueue <= pApp->super_Threads.getDBThread(0)->mQueueSize)
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
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
    CMsgCell* cell = msg->getCellFromMessage();
    *cell << &packet;
    cell->PAD();
    sendTCP_->PushSendMsg(msg);
    G_TraceLog()->sysLog(5, "Out onAUCTION_ASK_REGISTED_ITEM_NUM_GP");
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_REGIST_ITEM_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    unsigned int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_REGIST_ITEM_GP");
    if (!IsPointServer())
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_REGIST_ITEM_GP:this is not point server");
        return 1;
    }

    G_StatisticsCollector()->IncTryCnt(0);
    PCK_AUCTION_REGIST_ITEM_GA* pPck = (PCK_AUCTION_REGIST_ITEM_GA*)pCell->GetPacket();
    PrintDnfItemInfo(pPck->item_info, mpSzBuffer);
    G_TraceLog()->sysLog(5, "onAUCTION_REGIST_ITEM_GP(), before RegistItem(), o_id : %d , item : %s",
                         pPck->owner_id, mpSzBuffer);
    PCK_AUCTION_LOG_MESSAGE_PG log_packet;
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
    PCK_AUCTION_REGIST_RESULT_PG result_packet;
    result_packet.char_idx = pPck->char_idx;
    result_packet.owner_id = pPck->owner_id;
    if (return_code != 0)
    {
        result_packet.regist_result = 0;
        log_packet.auction_log_type = 1;
        G_StatisticsCollector()->IncFailCnt(0, return_code);
        result_packet.result_because =
            (unsigned char)G_Auction()->TransErrToReason(return_code);
    }
    else
    {
        result_packet.regist_result = 1;
        CNRDItemInfoList::STItemInfo* pItemInfo =
            G_Auction()->GetItemInfo(pPck->item_info.item_id);
        if (pItemInfo != NULL)
        {
            int item_category = (int)pItemInfo->category_;
            if (G_Auction()->IsAvatarCategory(item_category))
            {
                G_Auction()->AddAvatarEmblemInfo(pPck->item_info.add_info,
                                                 &pPck->jewel_info);
            }
        }
    }
    // ORIG：getCommonDataPool 结果直喂 getSendMessage（无 pPool 命名局部）
    {
        Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
        CMsgCell* cell = msg->getCellFromMessage();
        *cell << &log_packet;
        cell->PAD();
        sendTCP_->PushSendMsg(msg);
    }
    {
        Message* msg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
        CMsgCell* cell = msg->getCellFromMessage();
        *cell << &result_packet;
        cell->PAD();
        sendTCP_->PushSendMsg(msg);
    }
    if (return_code != 0)
    {
        if (return_code == 9)
        {
            G_TraceLog()->sysLog(7, "Out onAUCTION_REGIST_ITEM_GP , auction_id : %llu, %s",
                                 log_packet.auction_id, GetErrorStr(return_code));
            return return_code;
        }
        G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_ITEM_GP , auction_id : %llu, %s",
                             log_packet.auction_id, GetErrorStr(return_code));
    }
    else
    {
        G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_ITEM_GP, auction_id : %llu",
                             log_packet.auction_id);
    }
    return 0;
}
