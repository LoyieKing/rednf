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
    if (IsPointServer())
    {
        pCell->GetPacket();
        GameDataPool* pPool = (GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        auction::Character* pCharacter = pPool->createCharacter();
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
            unsigned int characKey = pArea->GetSpareKey();
            pCharacter->setCharacKey(characKey);
            pCharacter->setArea(0);
            ackPck.ack = 0x36;
        }
        CommonDataPool* pSendPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        Message* pNewMsg = pSendPool->getSendMessage(u);
        CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
        *pNewCell << &ackPck;
        pNewCell->PAD();
        sendTCP_->PushSendMsg(pNewMsg);
        u->setBindedSession(true);
        G_TraceLog()->sysLog(5, "Out  onAUCTION_REGIST_GP");
        return 0;
    }
    else
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_REGIST_GP:this is not point server");
        return 1;
    }
}

unsigned int HandlerFor_GP_::onAUCTION_ASK_AVERAGE_PRICE_GP(nsl::CMsgCell* pCell,
                                                            nsl::TCPUser* u)
{
    PCK_AUCTION_ASK_AVERAGE_PRICE_PG packet;
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_ASK_AVERAGE_PRICE_GP");
    if (IsPointServer())
    {
        PCK_AUCTION_ASK_AVERAGE_PRICE_GA* pPck =
            (PCK_AUCTION_ASK_AVERAGE_PRICE_GA*)pCell->GetPacket();
        ROI_AverageKey roiAverageKey;
        char itemRefineValue = 0;
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
        CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        Message* pNewMsg = pPool->getSendMessage(u);
        CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
        *pNewCell << &packet;
        pNewCell->PAD();
        sendTCP_->PushSendMsg(pNewMsg);
        G_TraceLog()->sysLog(5, "Out onAUCTION_ASK_AVERAGE_PRICE_GP");
        return_code = 0;
    }
    else
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_ASK_AVERAGE_PRICE_GP:this is not point server");
        return_code = 1;
    }
    return return_code;
}

unsigned int HandlerFor_GP_::onAUCTION_REGIST_CANCEL_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    PCK_AUCTION_REGIST_CANCEL_RESULT_PG cancel_result_packet;
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_REGIST_CANCEL_GP");
    if (IsPointServer())
    {
        G_StatisticsCollector()->IncTryCnt(1);
        PCK_AUCTION_REGIST_CANCEL_GA* pPck =
            (PCK_AUCTION_REGIST_CANCEL_GA*)pCell->GetPacket();
        G_TraceLog()->sysLog(5, "onAUCTION_REGIST_CANCEL_GP(), before RegistCancel(), o_id : %d, au_id : %llu",
                             pPck->owner_id, pPck->auction_id);
        cancel_result_packet.char_idx = pPck->char_idx;
        cancel_result_packet.owner_id = pPck->owner_id;
        return_code = G_Auction()->RegistCancel(pPck->owner_id, pPck->auction_id);
        if (return_code == 0)
        {
            cancel_result_packet.cancel_result = 1;
        }
        else
        {
            cancel_result_packet.cancel_result = 0;
            G_StatisticsCollector()->IncFailCnt(1, return_code);
            cancel_result_packet.result_because =
                (unsigned char)G_Auction()->TransErrToReason(return_code);
        }
        CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        Message* pNewMsg = pPool->getSendMessage(u);
        CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
        *pNewCell << &cancel_result_packet;
        pNewCell->PAD();
        sendTCP_->PushSendMsg(pNewMsg);
        if (return_code == 0)
        {
            G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_CANCEL_GP");
        }
        else
        {
            G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_CANCEL_GP, owner:%d, auId:%llu, %s",
                                 pPck->owner_id, pPck->auction_id,
                                 GetErrorStr(return_code));
        }
        return_code = 0;
    }
    else
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_REGIST_CANCEL_GP:this is not point server");
        return_code = 1;
    }
    return return_code;
}

unsigned int HandlerFor_GP_::onAUCTION_BIDDING_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    PCK_AUCTION_BIDDING_RESULT_PG packet;
    PCK_AUCTION_LOG_MESSAGE_PG log_packet;
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_BIDDING_GP");
    if (IsPointServer())
    {
        G_StatisticsCollector()->IncTryCnt(2);
        PCK_AUCTION_BIDDING_GA* pPck = (PCK_AUCTION_BIDDING_GA*)pCell->GetPacket();
        G_TraceLog()->sysLog(5, "onAUCTION_BIDDING_GP(), before Bidding(), b_id : %d , price : %d, au_id : %llu",
                             pPck->buyer_id, pPck->price, pPck->auction_id);
        log_packet.char_idx = pPck->char_idx;
        log_packet.auction_log_type = 0x02;
        log_packet.character_id = pPck->buyer_id;
        log_packet.price = pPck->price;
        log_packet.auction_id = pPck->auction_id;
        if (pApp->super_Threads.getDBThread(0)->mQueueSize <
            G_Script()->findIntValue(1, 0xd))
        {
            return_code = G_Auction()->Bidding(pPck->buyer_id, pPck->buyer_name,
                                               pPck->auction_id, pPck->price,
                                               packet.owner_nexon_id,
                                               packet.charge_point);
        }
        else
        {
            return_code = 0x31;
        }
        G_TraceLog()->sysLog(7, "onAUCTION_BIDDING_GP return code : %d", return_code);
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
        CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        Message* pNewMsg = pPool->getSendMessage(u);
        CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
        *pNewCell << &log_packet;
        pNewCell->PAD();
        sendTCP_->PushSendMsg(pNewMsg);
        pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        pNewMsg = pPool->getSendMessage(u);
        pNewCell = pNewMsg->getCellFromMessage();
        *pNewCell << &packet;
        pNewCell->PAD();
        sendTCP_->PushSendMsg(pNewMsg);
        G_TraceLog()->sysLog(5, "Out onAUCTION_BIDDING_GP");
        return_code = 0;
    }
    else
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_BIDDING_GP:this is not point server");
        return_code = 1;
    }
    return return_code;
}

unsigned int HandlerFor_GP_::onAUCTION_SEARCH_BY_ITEMKEY_GP(nsl::CMsgCell* pCell,
                                                            nsl::TCPUser* u)
{
    PCK_AUCTION_ITEM_LIST_PG packet;
    unsigned int result;

    G_TraceLog()->sysLog(5, "In  onAUCTION_SEARCH_BY_ITEMKEY_GP");
    if (IsPointServer())
    {
        PCK_AUCTION_SEARCH_BY_ITEMKEY_GA* pPck =
            (PCK_AUCTION_SEARCH_BY_ITEMKEY_GA*)pCell->GetPacket();
        result = G_Auction()->SearchByItemId(&pPck->tSearchByItemId,
                                             pPck->item_id_array,
                                             &packet.total_item_num, &packet.item_num,
                                             packet.item_info);
        if (result == 0)
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
                    unsigned int item_category = (unsigned int)pItemInfo->category_;
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
            CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
            Message* pNewMsg = pPool->getSendMessage(u);
            CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
            *pNewCell << &packet;
            pNewCell->PAD();
            sendTCP_->PushSendMsg(pNewMsg);
            G_TraceLog()->sysLog(5, "Out onAUCTION_SEARCH_BY_ITEMKEY_GP");
        }
    }
    else
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_SEARCH_BY_ITEMKEY_GP:this is not point server");
        result = 1;
    }
    return result;
}

unsigned int HandlerFor_GP_::onAUCTION_SEARCH_BY_NOITEMKEY_GP(nsl::CMsgCell* pCell,
                                                              nsl::TCPUser* u)
{
    PCK_AUCTION_ITEM_LIST_PG packet;
    unsigned int result;

    G_TraceLog()->sysLog(5, "In  onAUCTION_SEARCH_BY_NOITEMKEY_GP");
    if (IsPointServer())
    {
        PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA* pPck =
            (PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA*)pCell->GetPacket();
        result = G_Auction()->SearchByCategory(&pPck->tSearchByCategory,
                                               &packet.total_item_num, &packet.item_num,
                                               packet.item_info);
        if (result == 0)
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
                    unsigned int item_category = (unsigned int)pItemInfo->category_;
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
            CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
            Message* pNewMsg = pPool->getSendMessage(u);
            CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
            *pNewCell << &packet;
            pNewCell->PAD();
            sendTCP_->PushSendMsg(pNewMsg);
            G_TraceLog()->sysLog(5, "Out onAUCTION_SEARCH_BY_NOITEMKEY_GP");
        }
    }
    else
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_SEARCH_BY_NOITEMKEY_GP:this is not point server");
        result = 1;
    }
    return result;
}

unsigned int HandlerFor_GP_::onAUCTION_MY_REGISTED_ITEM_INFO_GP(nsl::CMsgCell* pCell,
                                                                nsl::TCPUser* u)
{
    PCK_AUCTION_MY_REGISTED_ITEM_INFO_PG packet;
    int packet_array_size = 0x1e;
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_MY_REGISTED_ITEM_INFO_GP");
    if (IsPointServer())
    {
        PCK_AUCTION_MY_REGISTED_ITEM_INFO_GA* pPck =
            (PCK_AUCTION_MY_REGISTED_ITEM_INFO_GA*)pCell->GetPacket();
        return_code = G_Auction()->GetMyRegistedItemInfo(
            pPck->owner_id, &packet_array_size, packet.my_registed_item_info);
        if (return_code == 0)
        {
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
                    unsigned int item_category = (unsigned int)pItemInfo->category_;
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
            CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
            Message* pNewMsg = pPool->getSendMessage(u);
            CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
            *pNewCell << &packet;
            pNewCell->PAD();
            sendTCP_->PushSendMsg(pNewMsg);
            G_TraceLog()->sysLog(5, "Out onAUCTION_MY_REGISTED_ITEM_INFO_GP");
            return_code = 0;
        }
        else
        {
            G_TraceLog()->sysLog(5, "Out onAUCTION_MY_REGISTED_ITEM_INFO_GP, %s",
                                 GetErrorStr(return_code));
            return_code = 0;
        }
    }
    else
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_MY_REGISTED_ITEM_INFO_GP:this is not point server");
        return_code = 1;
    }
    return return_code;
}

unsigned int HandlerFor_GP_::onAUCTION_MY_BIDDING_INFO_GP(nsl::CMsgCell* pCell,
                                                          nsl::TCPUser* u)
{
    PCK_AUCTION_MY_BIDDING_INFO_PG packet;
    int packet_array_size = 0x3c;
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_MY_BIDDING_INFO_GP");
    if (IsPointServer())
    {
        PCK_AUCTION_MY_BIDDING_INFO_GA* pPck =
            (PCK_AUCTION_MY_BIDDING_INFO_GA*)pCell->GetPacket();
        return_code = G_Auction()->GetMyBiddingInfo(
            pPck->buyer_id, &packet_array_size, packet.my_bidding_item_info);
        if (return_code == 0)
        {
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
                    unsigned int item_category = (unsigned int)pItemInfo->category_;
                    if (G_Auction()->IsAvatarCategory(item_category))
                    {
                        G_Auction()->GetAvatarEmblemInfo(
                            packet.my_bidding_item_info[i].item_info.add_info,
                            &packet.my_bidding_item_info[i].emblem_info);
                        packet.my_bidding_item_info[i].item_info.add_info = 0;
                    }
                }
            }
            packet.setSize((0x3c - packet_array_size) * -0x7d + 0x1d69);
            CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
            Message* pNewMsg = pPool->getSendMessage(u);
            CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
            *pNewCell << &packet;
            pNewCell->PAD();
            sendTCP_->PushSendMsg(pNewMsg);
            G_TraceLog()->sysLog(5, "Out onAUCTION_MY_BIDDING_INFO_GP");
            return_code = 0;
        }
        else
        {
            G_TraceLog()->sysLog(5, "Out onAUCTION_MY_BIDDING_INFO_GP, %s",
                                 GetErrorStr(return_code));
            return_code = 0;
        }
    }
    else
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_MY_BIDDING_INFO_GP:this is not point server");
        return_code = 1;
    }
    return return_code;
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
    packet.db_work_done = pArea->IsServiceRunning();
    CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
    Message* pNewMsg = pPool->getSendMessage(u);
    CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
    *pNewCell << &packet;
    pNewCell->PAD();
    sendTCP_->PushSendMsg(pNewMsg);
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
    G_Auction()->GetAveragePrice(pPck->item_id, 0, roiAverageKey, itemRefineValue,
                                 &ownerInfo.average_price);
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
    CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
    Message* pNewMsg = pPool->getSendMessage(u);
    CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
    *pNewCell << &packet;
    pNewCell->PAD();
    sendTCP_->PushSendMsg(pNewMsg);
    return 0;
}

unsigned int HandlerFor_GP_::onAUCTION_ASK_REGISTED_ITEM_NUM_GP(nsl::CMsgCell* pCell,
                                                                nsl::TCPUser* u)
{
    PCK_AUCTION_ASK_REGISTED_ITEM_NUM_PG packet;
    int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_ASK_REGISTED_ITEM_NUM_GP");
    if (IsPointServer())
    {
        PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA* pPck =
            (PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA*)pCell->GetPacket();
        if (pApp->super_Threads.getDBThread(0)->mQueueSize <
            G_Script()->findIntValue(1, 0xd))
        {
            packet.now_registed_num =
                (short)G_Auction()->GetNowRegistedItemNum(pPck->character_id);
        }
        else
        {
            packet.now_registed_num = -1;
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
        CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        Message* pNewMsg = pPool->getSendMessage(u);
        CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
        *pNewCell << &packet;
        pNewCell->PAD();
        sendTCP_->PushSendMsg(pNewMsg);
        G_TraceLog()->sysLog(5, "Out onAUCTION_ASK_REGISTED_ITEM_NUM_GP");
        return_code = 0;
    }
    else
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_ASK_REGISTED_ITEM_NUM_GP:this is not point server");
        return_code = 1;
    }
    return return_code;
}

unsigned int HandlerFor_GP_::onAUCTION_REGIST_ITEM_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u)
{
    PCK_AUCTION_LOG_MESSAGE_PG log_packet;
    PCK_AUCTION_REGIST_RESULT_PG result_packet;
    unsigned int return_code = 0;

    G_TraceLog()->sysLog(5, "In  onAUCTION_REGIST_ITEM_GP");
    if (IsPointServer())
    {
        G_StatisticsCollector()->IncTryCnt(0);
        PCK_AUCTION_REGIST_ITEM_GA* pPck = (PCK_AUCTION_REGIST_ITEM_GA*)pCell->GetPacket();
        PrintDnfItemInfo(pPck->item_info, mpSzBuffer);
        G_TraceLog()->sysLog(5, "onAUCTION_REGIST_ITEM_GP(), before RegistItem(), o_id : %d , item : %s",
                             pPck->owner_id, mpSzBuffer);
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
        result_packet.char_idx = pPck->char_idx;
        result_packet.owner_id = pPck->owner_id;
        if (return_code == 0)
        {
            result_packet.regist_result = 1;
            CNRDItemInfoList::STItemInfo* pItemInfo =
                G_Auction()->GetItemInfo(pPck->item_info.item_id);
            if (pItemInfo != NULL)
            {
                unsigned int item_category = (unsigned int)pItemInfo->category_;
                if (G_Auction()->IsAvatarCategory(item_category))
                {
                    G_Auction()->AddAvatarEmblemInfo(pPck->item_info.add_info,
                                                     &pPck->jewel_info);
                }
            }
        }
        else
        {
            result_packet.regist_result = 0;
            log_packet.auction_log_type = 1;
            G_StatisticsCollector()->IncFailCnt(0, return_code);
            result_packet.result_because =
                (unsigned char)G_Auction()->TransErrToReason(return_code);
        }
        CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        Message* pNewMsg = pPool->getSendMessage(u);
        CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
        *pNewCell << &log_packet;
        pNewCell->PAD();
        sendTCP_->PushSendMsg(pNewMsg);
        pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
        pNewMsg = pPool->getSendMessage(u);
        pNewCell = pNewMsg->getCellFromMessage();
        *pNewCell << &result_packet;
        pNewCell->PAD();
        sendTCP_->PushSendMsg(pNewMsg);
        if (return_code == 0)
        {
            G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_ITEM_GP, auction_id : %llu",
                                 log_packet.auction_id);
        }
        else
        {
            if (return_code == 9)
            {
                G_TraceLog()->sysLog(7, "Out onAUCTION_REGIST_ITEM_GP , auction_id : %llu, %s",
                                     log_packet.auction_id, GetErrorStr(9));
                return return_code;
            }
            G_TraceLog()->sysLog(5, "Out onAUCTION_REGIST_ITEM_GP , auction_id : %llu, %s",
                                 log_packet.auction_id, GetErrorStr(return_code));
        }
        return_code = 0;
    }
    else
    {
        G_TraceLog()->sysLog(7, "Out  onAUCTION_REGIST_ITEM_GP:this is not point server");
        return_code = 1;
    }
    return return_code;
}
