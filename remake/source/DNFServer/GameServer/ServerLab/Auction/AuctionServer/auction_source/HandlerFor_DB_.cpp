// Rebuilt from df_auction_r DWARF (HandlerFor_DB_.cpp CU), 2026-08-08
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "HandlerFor_DB_.h"
#include "TraceLog.h"
#include "Script.h"
#include "Thread.h"
#include "DBConnections.h"
#include "ServiceFactory.h"
#include "PacketDesign.h"
#include "Auction.h"
#include "Zone.h"
#include "IArea.h"
#include "ISession.h"
#include "DataPools.h"
#include "CommonDataPool.h"
#include "Message.h"
#include "TCPSendThread.h"
#include "DNFFunctionLib.h"
#include "DNFFunctionLibWrapper.h"
#include "ServiceError.h"
#include "TEA.h"

using namespace nsl;

extern char SENDER_NAME[0xff];
extern char SENDER_NAME_GOLD[0xff];

HandlerFor_DB_::~HandlerFor_DB_()
{
}

void HandlerFor_DB_::DecryptPassword(const char* pInput, char* pOutput)
{
    unsigned char p_binary_decrypt_result[64];
    char p_decrypt_result[72];

    memset(p_decrypt_result, 0, 0x40);
    memset(p_binary_decrypt_result, 0, 0x40);
    memset(pOutput, 0, 0x15);
    if (!DNFFLibWrapper::Hex2Binary(pInput, p_binary_decrypt_result, 0x18))
    {
        G_TraceLog()->sysLog(7, "Fail to decrypt DB Password for %s", pInput);
        printf("Fail to decrypt DB Password for %s\n", pInput);
        exit(1);
    }
    mTEA.Decrypt((char*)p_binary_decrypt_result, p_decrypt_result, 0x18);
    strncpy(pOutput, p_decrypt_result, 0x14);
}

char* HandlerFor_DB_::GetYYYYMM()
{
    static char yyyymm[8];
    time_t current_time;
    tm* now_tm;

    time(&current_time);
    now_tm = localtime(&current_time);
    snprintf(yyyymm, 8, "%d%02d", now_tm->tm_year + 0x76c, now_tm->tm_mon + 1);
    return yyyymm;
}

void HandlerFor_DB_::init()
{
    char p_db_password[21];
    char ret = 0;

    nsl::IHandler::init();
    for (int i = 0; i < 0x7e; i = i + 1)
    {
        mArrayFunc[i] = NULL;
    }
    registFuncMap();
    TeaInitialize();
    puts("\n=====================DB config check=========================");
    printf("Game DB IP      : %s\n", G_Script()->findCharValue(1, 3));
    printf("Game DB PORT    : %s\n", G_Script()->findCharValue(1, 4));
    printf("Game DB Account : %s\n", G_Script()->findCharValue(1, 5));
    printf("Game DB Password: %s\n", G_Script()->findCharValue(1, 6));
    printf("Game DB Name    : %s\n", G_Script()->findCharValue(1, 7));
    printf("Auction DB IP      : %s\n", G_Script()->findCharValue(1, 8));
    printf("Auction DB PORT    : %s\n", G_Script()->findCharValue(1, 9));
    printf("Auction DB Account : %s\n", G_Script()->findCharValue(1, 10));
    printf("Auction DB Password: %s\n", G_Script()->findCharValue(1, 0xb));
    printf("Auction DB Name    : %s\n", G_Script()->findCharValue(1, 0xc));
    puts("=====================DB config check=========================\n");

    ret = 0;
    DecryptPassword(G_Script()->findCharValue(1, 6), p_db_password);
    pApp->super_DBConnections.getDBConnection(0)->set_charset_name_option();
    unsigned int port = (unsigned int)atoi(G_Script()->findCharValue(1, 4));
    ret = pApp->super_DBConnections.getDBConnection(0)->open(
        G_Script()->findCharValue(1, 3), G_Script()->findCharValue(1, 7),
        G_Script()->findCharValue(1, 5), p_db_password, port);
    if (!ret)
    {
        G_TraceLog()->sysLog(7, "Fail to open GameDB. process exits.");
        puts("***************Fail to open GameDB. process exits.***************");
        exit(1);
    }

    DecryptPassword(G_Script()->findCharValue(1, 0xb), p_db_password);
    pApp->super_DBConnections.getDBConnection(1)->set_charset_name_option();
    port = (unsigned int)atoi(G_Script()->findCharValue(1, 9));
    ret = pApp->super_DBConnections.getDBConnection(1)->open(
        G_Script()->findCharValue(1, 8), G_Script()->findCharValue(1, 0xc),
        G_Script()->findCharValue(1, 10), p_db_password, port);
    if (!ret)
    {
        G_TraceLog()->sysLog(7, "Fail to open AuctionDB. process exits.");
        puts("***************Fail to open AuctionDB. process exits.************");
        exit(2);
    }

    for (int i = 0; i < 2; i = i + 1)
    {
        char b_result = pApp->super_DBConnections.getDBConnection(i)->set_reconnect_option();
        if (!b_result)
        {
            G_TraceLog()->errorLog("DB Category no.%d : set MYSQL_OPT_RECONNECT Error", i);
        }
    }
}

void HandlerFor_DB_::registFuncMap()
{
    mArrayFunc[0] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_GET_REGISTED_ITEM;
    mArrayFunc[1] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_REGIST_ITEM;
    mArrayFunc[2] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_UPPER_BIDDING;
    mArrayFunc[3] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_INSERT_AVERAGE_PRICE;
    mArrayFunc[4] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_GET_AVERAGE_PRICE;
    mArrayFunc[5] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_UPDATE_AVERAGE_PRICE;
    mArrayFunc[6] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_DELETE_ITEM;
    mArrayFunc[0x15] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onGAME_DB_SEND_PACKAGE;
    mArrayFunc[0x16] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onGAME_DB_SEND_PACKAGE_BY_EXPIRE;
    mArrayFunc[7] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_EXPIRE_HISTORY;
    mArrayFunc[8] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_BUYER_HISTORY;
    mArrayFunc[9] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_PING;
    mArrayFunc[0x11] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_UPDATE_ITEM;
    mArrayFunc[0x12] =
        (nsl::IHandler::DBHandlerFunc)&HandlerFor_DB_::onAUCTION_DB_GET_ROI_AVERAGE_PRICE;
}

nsl::IHandler::DBHandlerFunc HandlerFor_DB_::SearchDBHandlerFunc(int typeId)
{
    return mArrayFunc[typeId];
}

bool HandlerFor_DB_::GetAuctionMainFetchResult(nsl::DBConnection* db,
                                               tagAUCTION_DB_GET_REGISTED_ITEM* pContext)
{
    bool bRet = true;
    int i = 1;
    unsigned char upgrade;
    unsigned char seal_cnt;

    bRet = db->get_int64(0, pContext->auction_id);
    if (bRet)
    {
        int col = i + 1;
        i = i + 2;
        bRet = db->get_long(col, pContext->expire_time);
        if (bRet)
        {
            col = i;
            i = i + 1;
            bRet = db->get_int(col, pContext->owner_id);
            if (bRet)
            {
                col = i;
                i = i + 1;
                bRet = db->get_str(col, pContext->owner_name, 0xc);
                if (bRet)
                {
                    *(char*)&pContext->buyer_id = 0;
                    col = i;
                    i = i + 1;
                    bRet = db->get_int(col, pContext->buyer_id);
                    if (bRet)
                    {
                        col = i;
                        i = i + 1;
                        bRet = db->get_str(col, pContext->buyer_name, 0xc);
                        if (bRet)
                        {
                            *(char*)&pContext->price = 0;
                            col = i;
                            i = i + 1;
                            bRet = db->get_int(col, pContext->price);
                            if (bRet)
                            {
                                col = i;
                                i = i + 1;
                                bRet = db->get_int(col, pContext->instant_price);
                                if (bRet)
                                {
                                    col = i;
                                    i = i + 1;
                                    bRet = db->get_bool(col, pContext->item_info.seal);
                                    if (bRet)
                                    {
                                        col = i;
                                        i = i + 1;
                                        bRet = db->get_ulong(col, pContext->item_info.item_id);
                                        if (bRet)
                                        {
                                            col = i;
                                            i = i + 1;
                                            bRet = db->get_int(col, pContext->item_info.add_info);
                                            if (bRet)
                                            {
                                                col = i;
                                                i = i + 1;
                                                bRet = db->get_ubyte(col, upgrade);
                                                if (bRet)
                                                {
                                                    col = i;
                                                    i = i + 1;
                                                    bRet = db->get_ubyte(col, pContext->item_info.abilityType_);
                                                    if (bRet)
                                                    {
                                                        col = i;
                                                        i = i + 1;
                                                        bRet = db->get_ushort(col, pContext->item_info.abilityValue_);
                                                        if (bRet)
                                                        {
                                                            col = i;
                                                            i = i + 1;
                                                            bRet = db->get_ubyte(col, seal_cnt);
                                                            if (bRet)
                                                            {
                                                                col = i;
                                                                i = i + 1;
                                                                bRet = db->get_ushort(col, pContext->item_info.endurance);
                                                                if (bRet)
                                                                {
                                                                    col = i;
                                                                    i = i + 1;
                                                                    bRet = db->get_ulong(col, pContext->item_info.extendInfo);
                                                                    if (bRet)
                                                                    {
                                                                        col = i;
                                                                        i = i + 1;
                                                                        bRet = db->get_ubyte(col, pContext->owner_type);
                                                                        if (bRet)
                                                                        {
                                                                            col = i;
                                                                            i = i + 1;
                                                                            bRet = db->get_uint(col, pContext->black_point);
                                                                            if (bRet)
                                                                            {
                                                                                col = i;
                                                                                i = i + 1;
                                                                                bRet = db->get_int(col, pContext->unit_price);
                                                                                if (bRet)
                                                                                {
                                                                                    col = i;
                                                                                    i = i + 1;
                                                                                    bRet = db->get_binary(col, &pContext->item_info.guid_, 10);
                                                                                    if (bRet)
                                                                                    {
                                                                                        col = i;
                                                                                        i = i + 1;
                                                                                        bRet = db->get_binary(col, &pContext->item_info.random_option_, 0xe);
                                                                                        if (bRet)
                                                                                        {
                                                                                            pContext->item_info.uniItemAttr =
                                                                                                (pContext->item_info.uniItemAttr & 0xe0) | (upgrade & 0x1f);
                                                                                            pContext->item_info.uniItemAttr =
                                                                                                (pContext->item_info.uniItemAttr & 0x1f) | (seal_cnt << 5);
                                                                                            col = i;
                                                                                            i = i + 1;
                                                                                            bRet = db->get_longlong(col, *(long long*)&pContext->_reg_roi_category_key);
                                                                                            if (bRet)
                                                                                            {
                                                                                                col = i;
                                                                                                i = i + 1;
                                                                                                bRet = db->get_int(col, pContext->_reg_roi_category_key.field_1._low_category_key);
                                                                                                if (bRet)
                                                                                                {
                                                                                                    col = i;
                                                                                                    i = i + 1;
                                                                                                    bRet = db->get_str(col, pContext->owner_nexon_id, 0x20);
                                                                                                    if (bRet)
                                                                                                    {
                                                                                                        col = i;
                                                                                                        i = i + 1;
                                                                                                        bRet = db->get_ubyte(col, upgrade);
                                                                                                        if (bRet)
                                                                                                        {
                                                                                                            pContext->item_info.separate_info.SetUpgradeSeparate(upgrade);
                                                                                                            bRet = true;
                                                                                                        }
                                                                                                        else
                                                                                                        {
                                                                                                            bRet = false;
                                                                                                        }
                                                                                                    }
                                                                                                    else
                                                                                                    {
                                                                                                        bRet = false;
                                                                                                    }
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    bRet = false;
                                                                                                }
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                                bRet = false;
                                                                                            }
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            bRet = false;
                                                                                        }
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        bRet = false;
                                                                                    }
                                                                                }
                                                                                else
                                                                                {
                                                                                    bRet = false;
                                                                                }
                                                                            }
                                                                            else
                                                                            {
                                                                                bRet = false;
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            bRet = false;
                                                                        }
                                                                    }
                                                                    else
                                                                    {
                                                                        bRet = false;
                                                                    }
                                                                }
                                                                else
                                                                {
                                                                    bRet = false;
                                                                }
                                                            }
                                                            else
                                                            {
                                                                bRet = false;
                                                            }
                                                        }
                                                        else
                                                        {
                                                            bRet = false;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        bRet = false;
                                                    }
                                                }
                                                else
                                                {
                                                    bRet = false;
                                                }
                                            }
                                            else
                                            {
                                                bRet = false;
                                            }
                                        }
                                        else
                                        {
                                            bRet = false;
                                        }
                                    }
                                    else
                                    {
                                        bRet = false;
                                    }
                                }
                                else
                                {
                                    bRet = false;
                                }
                            }
                            else
                            {
                                bRet = false;
                            }
                        }
                        else
                        {
                            bRet = false;
                        }
                    }
                    else
                    {
                        bRet = false;
                    }
                }
                else
                {
                    bRet = false;
                }
            }
            else
            {
                bRet = false;
            }
        }
        else
        {
            bRet = false;
        }
    }
    else
    {
        bRet = false;
    }
    return bRet;
}

unsigned int HandlerFor_DB_::onAUCTION_DB_GET_REGISTED_ITEM(nsl::CMsgCell* pCell)
{
    stAvatarEmblemInfo_t emblem_info_struct;
    stAvatarExpansionInfo_t expansion_info_struct;
    DBConnection* db;
    DBConnection* game_db;
    unsigned int item_category_temp;
    LPDBTR_AUCTION_DB_GET_REGISTED_ITEM pContext;
    int ret;
    char bRet;
    char* yyyymm;
    unsigned int row;
    unsigned int history_db_count;
    unsigned long long history_max_auction_id;

    G_TraceLog()->sysLog(5, "In  onAUCTION_DB_GET_REGISTED_ITEM");
    db = pApp->super_DBConnections.getDBConnection(1);
    game_db = pApp->super_DBConnections.getDBConnection(0);
    pContext = (LPDBTR_AUCTION_DB_GET_REGISTED_ITEM)pCell->GetDBTr();
    db->set_query(
        "seLect auction_id, occ_time, expire_time, owner_id, owner_name, buyer_id, buyer_name, price, instant_price, seal_flag, item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, extend_info, owner_type, black_point, unit_price, item_guid, random_option, roi_high_key, roi_low_key, owner_nexon_id,seperate_upgrade from auction_main order by expire_time");
    ret = db->exec(true);
    bRet = 1;
    if (ret != 0)
    {
        G_TraceLog()->sysLog(7,
            "Fail to exec(select * from auction_main order by expire_time). process exits.");
        puts(
            "***************Fail to exec(select * from auction_main order by expire_time). process exits.************");
        exit(5);
    }
    row = 0;
    ret = 0;
    do
    {
        if (db->get_n_rows() <= row)
        {
fail_fetch:
            if (bRet != 1)
            {
                G_TraceLog()->sysLog(7, "onAUCTION_DB_GET_REGISTED_ITEM, Fail to fatch(). process exits.");
                puts(
                    "onAUCTION_DB_GET_REGISTED_ITEM, ***************Fail to fatch(). process exits.************");
                exit(4);
            }
            history_db_count = 0;
            yyyymm = GetYYYYMM();
            db->set_query("seLect count(*) from auction_history_%s", yyyymm);
            ret = db->exec(true);
            if (ret != 0)
            {
                G_TraceLog()->sysLog(7, "Fail to exec(select count(*) from auction_history). process exits.");
                puts(
                    "***************Fail to exec(select count(*) from auction_history). process exits.************");
                exit(9);
            }
            bRet = db->fetch();
            if (!bRet)
            {
                G_TraceLog()->sysLog(7, "ERROR, db->fetch(), select count(*) from auction_history");
                puts("***************ERROR, db->fetch(), select count(*) from auction_history************");
                exit(6);
            }
            db->get_uint(0, history_db_count);
            if (history_db_count != 0)
            {
                history_max_auction_id = 0;
                db->set_query("seLect max(auction_id) from auction_history_%s", yyyymm);
                ret = db->exec(true);
                if (ret != 0)
                {
                    G_TraceLog()->sysLog(7,
                        "Fail to exec(select max(auction_id) from auction_history). process exits.");
                    puts(
                        "***************Fail to exec(select max(auction_id) from auction_history). process exits.************");
                    exit(8);
                }
                if (db->get_n_rows() == 1)
                {
                    bRet = db->fetch();
                    if (!bRet)
                    {
                        G_TraceLog()->sysLog(7, "ERROR, db->fetch(), max(auction_id) from history");
                        puts(
                            "***************ERROR, db->fetch(), max(auction_id) from history************");
                        exit(7);
                    }
                    bRet = db->get_int64(0, history_max_auction_id);
                    if (!bRet)
                    {
                        return 1;
                    }
                    G_Auction()->SetMaxHistoryAuctionId(history_max_auction_id);
                }
            }
            GSArea* pArea = (GSArea*)G_Zone()->mArea[0];
            pArea->SetServiceRunning(true);
            if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
            {
                PCK_AUCTION_SERVICE_AVAILABLE_PG pck;
                nsl::MAP_OBJECTS_ITER iter = pArea->getBeginIter();
                while (!pArea->isIterEnd(iter))
                {
                    nsl::ISession* pSession = pArea->getValueFromIter(iter);
                    TCPUser* pTCPUser = pSession->getTCPUser();
                    CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
                    Message* pNewMsg = pPool->getSendMessage(pTCPUser);
                    CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
                    *pNewCell << &pck;
                    pNewCell->PAD();
                    sendTCP_->PushSendMsg(pNewMsg);
                    ++iter;
                }
            }
            else
            {
                PCK_AUCTION_SERVICE_AVAILABLE_AG pck;
                nsl::MAP_OBJECTS_ITER iter = pArea->getBeginIter();
                while (!pArea->isIterEnd(iter))
                {
                    nsl::ISession* pSession = pArea->getValueFromIter(iter);
                    TCPUser* pTCPUser = pSession->getTCPUser();
                    CommonDataPool* pPool = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId);
                    Message* pNewMsg = pPool->getSendMessage(pTCPUser);
                    CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
                    *pNewCell << &pck;
                    pNewCell->PAD();
                    sendTCP_->PushSendMsg(pNewMsg);
                    ++iter;
                }
            }
            G_TraceLog()->sysLog(5, "out onAUCTION_DB_GET_REGISTED_ITEM : paytype(%d)",
                                 G_Auction()->GetPayType());
            return 0;
        }
        bRet = db->fetch();
        if (!bRet)
        {
            G_TraceLog()->sysLog(7, "ERROR, db->fetch()");
            goto fail_fetch;
        }
        item_category_temp = 0;
        bRet = GetAuctionMainFetchResult(db, pContext);
        if (!bRet)
        {
            goto fail_fetch;
        }
        CNRDItemInfoList::STItemInfo* pItemInfo = G_Auction()->GetItemInfo(pContext->item_info.item_id);
        if (pItemInfo == NULL)
        {
            G_TraceLog()->sysLog(7, "ERROR, 0904patch_bug, au_id=%llu, item_id=%d, ",
                                 pContext->auction_id, pContext->item_info.item_id);
        }
        else
        {
            ret = G_Auction()->RegistItem(
                pContext->owner_id, pContext->owner_name, pContext->owner_type,
                pContext->price, pContext->instant_price, pContext->black_point,
                pContext->unit_price, pContext->_reg_roi_category_key,
                pContext->owner_nexon_id, pContext->item_info, pContext->auction_id,
                pContext->expire_time, pContext->buyer_id, pContext->buyer_name, true);
            if (ret != 0)
            {
                G_TraceLog()->sysLog(7, "Fail to RegistItem() from DB. process exits. %s(%d)",
                                     GetErrorStr(ret), ret);
                puts("***************Fail to RegistItem() from DB. process exits.************");
                exit(3);
            }
            item_category_temp = (unsigned int)G_Auction()->GetItemInfo(pContext->item_info.item_id)->category_;
            if (G_Auction()->IsAvatarCategory(item_category_temp))
            {
                game_db->set_query("seLect jewel_socket, color1, color2 from user_items where ui_id=%d",
                                   pContext->item_info.add_info);
                ret = game_db->exec(true);
                if (ret != 0)
                {
                    G_TraceLog()->sysLog(7,
                        "Fail to exec(seLect jewel_socket from user_items where ui_id=%d). process exits.",
                        pContext->item_info.add_info);
                    exit(6);
                }
                if (game_db->get_n_rows() == 0)
                {
                    memset(&emblem_info_struct, 0, 0x1e);
                    memset(&expansion_info_struct, 0, 4);
                }
                else
                {
                    short color1 = 0;
                    short color2 = 0;
                    bRet = game_db->fetch();
                    if (!bRet)
                    {
                        G_TraceLog()->sysLog(7, "ERROR, game_db->fetch(), avatar ui_id=%d",
                                             pContext->item_info.add_info);
                        goto fail_fetch;
                    }
                    bRet = game_db->get_binary(0, &emblem_info_struct, 0x1e);
                    if (!bRet)
                    {
                        G_TraceLog()->sysLog(7, "ERROR, game_db->get_binary(), avatar ui_id=%d",
                                             pContext->item_info.add_info);
                        goto fail_fetch;
                    }
                    color2 = 0;
                    bRet = game_db->get_short(1, color1);
                    if (!bRet)
                    {
                        G_TraceLog()->sysLog(7, "ERROR, game_db->get_short(), avatar ui_id=%d",
                                             pContext->item_info.add_info);
                        goto fail_fetch;
                    }
                    color1 = 0;
                    bRet = game_db->get_short(2, color2);
                    if (!bRet)
                    {
                        G_TraceLog()->sysLog(7, "ERROR, game_db->get_short(), avatar ui_id=%d",
                                             pContext->item_info.add_info);
                        goto fail_fetch;
                    }
                    expansion_info_struct.color[0] = color1;
                    expansion_info_struct.color[1] = color2;
                }
                ret = G_Auction()->AddAvatarEmblemInfo(pContext->item_info.add_info, &emblem_info_struct);
                if (ret != 0)
                {
                    G_TraceLog()->sysLog(7, "ERROR, AddAvatarEmblemInfo(), ret=%s", GetErrorStr(ret));
                    goto fail_fetch;
                }
                ret = G_Auction()->AddAvatarExpansionInfo(pContext->item_info.add_info, &expansion_info_struct);
                if (ret != 0)
                {
                    G_TraceLog()->sysLog(7, "ERROR, AddAvatarExpansionInfo(), ret=%s", GetErrorStr(ret));
                    goto fail_fetch;
                }
            }
        }
        row = row + 1;
    } while (true);
}

unsigned int HandlerFor_DB_::onAUCTION_DB_EXPIRE_HISTORY(nsl::CMsgCell* pCell)
{
    char randomOption[29];
    time_t time_now;
    DBConnection* db;
    LPDBTR_AUCTION_DB_EXPIRE_HISTORY pContext;
    int expire_time;
    int ret;

    G_TraceLog()->sysLog(6, "In  onAUCTION_DB_EXPIRE_HISTORY");
    db = pApp->super_DBConnections.getDBConnection(1);
    pContext = (LPDBTR_AUCTION_DB_EXPIRE_HISTORY)pCell->GetDBTr();
    time(&time_now);
    expire_time = G_Auction()->GetExpireTime();
    if (pContext->owner_id == -1)
    {
        expire_time = G_Auction()->GetSystemAuctionExpireTime();
    }
    memset(randomOption, 0, 29);
    DNFFLib::Binary2Hex((unsigned char*)&pContext->item_info.random_option_, 0xe, randomOption);
    unsigned char separate_upgrade = pContext->item_info.separate_info.GetUpgradeSeparate();
    char* guid_str = db->blob_to_str(0, &pContext->item_info.guid_, 10);
    snprintf(mSzBuffer, 0x6000,
        "inSert into auction_history_%s(auction_id,start_time,occ_time,event_type,owner_id,buyer_id,price,seal_flag,item_id,add_info,upgrade,seal_cnt,endurance,extend_info,owner_postal_id,buyer_postal_id,amplify_option,amplify_value,item_guid,unit_price,roi_high_key, roi_low_key, commission, owner_type,random_option,seperate_upgrade) values(%llu,from_unixtime(%d),from_unixtime(%d),%hhu,%d,%d,%d,%d,%u,%d,%hhu,%hhu,%hu,%hu,%u,%u,%hhu,%hu,\'%s\',%d,%llu, %d,%d,%d,0x%s,%d)",
        GetYYYYMM(), pContext->auction_id, pContext->expire_time - expire_time,
        time_now, (unsigned int)pContext->event_type, pContext->owner_id,
        pContext->buyer_id, pContext->price, (int)pContext->item_info.seal,
        pContext->item_info.item_id, pContext->item_info.add_info,
        (unsigned int)(pContext->item_info.uniItemAttr & 0x1f),
        (unsigned int)(pContext->item_info.uniItemAttr >> 5),
        pContext->item_info.endurance, pContext->item_info.extendInfo,
        pContext->owner_postal_id, pContext->buyer_postal_id,
        (unsigned int)pContext->item_info.abilityType_,
        pContext->item_info.abilityValue_, guid_str, pContext->unit_price,
        pContext->_reg_roi_category_key.field_0._qw,
        pContext->_reg_roi_category_key.field_1._low_category_key,
        pContext->commission, (unsigned int)pContext->owner_type,
        randomOption, (unsigned int)separate_upgrade);
    ret = db->exec(true);
    G_TraceLog()->sysLog(6, "out onAUCTION_DB_EXPIRE_HISTORY");
    return ret;
}

unsigned int HandlerFor_DB_::onAUCTION_DB_BUYER_HISTORY(nsl::CMsgCell* pCell)
{
    time_t time_now;
    DBConnection* db;
    LPDBTR_AUCTION_DB_BUYER_HISTORY pContext;
    char* yyyymm;
    int ret;

    G_TraceLog()->sysLog(6, "In  onAUCTION_DB_BUYER_HISTORY");
    db = pApp->super_DBConnections.getDBConnection(1);
    pContext = (LPDBTR_AUCTION_DB_BUYER_HISTORY)pCell->GetDBTr();
    time(&time_now);
    yyyymm = GetYYYYMM();
    db->set_query(
        "inSert into auction_history_buyer_%s(auction_id,occ_time,pre_buyer_id,buyer_id,pre_price,price,pre_buyer_postal_id) values(%llu,from_unixtime(%d),%d,%d,%d,%d,%u)",
        yyyymm, pContext->auction_id, time_now, pContext->pre_buyer_id,
        pContext->buyer_id, pContext->pre_price, pContext->price,
        pContext->pre_buyer_postal_id);
    ret = db->exec(true);
    G_TraceLog()->sysLog(6, "out onAUCTION_DB_BUYER_HISTORY");
    return ret;
}

unsigned int HandlerFor_DB_::onAUCTION_DB_PING(nsl::CMsgCell* pCell)
{
    DBConnection* db;
    int ret;

    db = pApp->super_DBConnections.getDBConnection(1);
    db->set_query("select now()");
    ret = db->exec(true);
    return ret;
}

unsigned int HandlerFor_DB_::onAUCTION_DB_UPDATE_ITEM(nsl::CMsgCell* pCell)
{
    DBConnection* db;
    LPDBTR_AUCTION_DB_UPDATE_ITEM pContext;
    int ret;

    G_TraceLog()->sysLog(6, "In  onAUCTION_DB_UPDATE_ITEM");
    db = pApp->super_DBConnections.getDBConnection(1);
    pContext = (LPDBTR_AUCTION_DB_UPDATE_ITEM)pCell->GetDBTr();
    snprintf(mSzBuffer, 0x6000,
        "update auction_main set instant_price=%d,add_info=%d where auction_id = %llu",
        pContext->instant_price, pContext->add_info, pContext->auction_id);
    G_TraceLog()->sysLog(6, "%s", mSzBuffer);
    db->set_query("%s", mSzBuffer);
    ret = db->exec(true);
    G_TraceLog()->sysLog(6, "Out  onAUCTION_DB_UPDATE_ITEM");
    return ret;
}

unsigned int HandlerFor_DB_::onAUCTION_DB_UPDATE_AVERAGE_PRICE(nsl::CMsgCell* pCell)
{
    DBConnection* db;
    LPDBTR_AUCTION_DB_UPDATE_AVERAGE_PRICE pContext;
    int ret;

    G_TraceLog()->sysLog(6, "In  onAUCTION_DB_UPDATE_AVERAGE_PRICE");
    db = pApp->super_DBConnections.getDBConnection(1);
    pContext = (LPDBTR_AUCTION_DB_UPDATE_AVERAGE_PRICE)pCell->GetDBTr();
    if (pContext->_roi_average_key.option_category.isEmpty())
    {
        db->set_query(
            "upDate auction_average_price set average_price=%d where item_id=%u and upgrade=%hhu and seperate_upgrade=%hhu",
            pContext->average_price, pContext->item_id, (unsigned int)pContext->upgrade,
            (unsigned int)pContext->seperate_upgrade);
    }
    else
    {
        db->set_query(
            "upDate auction_roi_average_price set average_price=%d, real_purchase_count=%d where item_id=%u and upgrade=%hhu and roi_high_key=%lld and roi_low_key=%d and roi_index1=%d and roi_index2=%d and roi_index3=%d and seperate_upgrade=%hhu",
            pContext->average_price, pContext->purchase_cnt, pContext->item_id,
            (unsigned int)pContext->upgrade,
            pContext->_roi_average_key.option_category.field_0._qw,
            pContext->_roi_average_key.option_category.field_1._low_category_key,
            (int)pContext->_roi_average_key._oiv.option_index_value[0],
            (int)pContext->_roi_average_key._oiv.option_index_value[1],
            (int)pContext->_roi_average_key._oiv.option_index_value[2],
            (unsigned int)pContext->seperate_upgrade);
    }
    ret = db->exec(true);
    G_TraceLog()->sysLog(6, "out onAUCTION_DB_UPDATE_AVERAGE_PRICE");
    return ret;
}

unsigned int HandlerFor_DB_::onAUCTION_DB_DELETE_ITEM(nsl::CMsgCell* pCell)
{
    DBConnection* db;
    LPDBTR_AUCTION_DB_DELETE_ITEM pContext;
    int ret;

    G_TraceLog()->sysLog(6, "In  onAUCTION_DB_DELETE_ITEM");
    db = pApp->super_DBConnections.getDBConnection(1);
    pContext = (LPDBTR_AUCTION_DB_DELETE_ITEM)pCell->GetDBTr();
    snprintf(mSzBuffer, 0x6000, "deLete from auction_main where auction_id = %llu",
             pContext->auction_id);
    G_TraceLog()->sysLog(6, "%s", mSzBuffer);
    db->set_query("%s", mSzBuffer);
    ret = db->exec(true);
    G_TraceLog()->sysLog(6, "Out  onAUCTION_DB_DELETE_ITEM");
    return ret;
}

unsigned int HandlerFor_DB_::insertPackage(nsl::DBConnection* db, int charac_no,
                                           int send_charac_no, const char* send_charac_name,
                                           const char* letter_text, unsigned int& letter_id,
                                           time_t insertTime)
{
    char letter_text_ex[0x200];
    char send_charac_name_ex[27];
    int ret;

    memset(letter_text_ex, 0, 0x1ff);
    db->escape_string(letter_text_ex, letter_text);
    memset(send_charac_name_ex, 0, 0x1b);
    db->escape_string(send_charac_name_ex, send_charac_name);
    snprintf(mSzBuffer, 0x6000,
        "inSert into letter(charac_no,send_charac_no,send_charac_name,letter_text,reg_date,stat) values(%d,%d,\'%s\',\'%s\',from_unixtime(%d),%d)",
        charac_no, send_charac_no, send_charac_name_ex, letter_text_ex, insertTime, 1);
    G_TraceLog()->sysLog(6, "%s", mSzBuffer);
    db->set_query(mSzBuffer);
    ret = db->exec(true);
    if (ret == 0)
    {
        letter_id = 0;
        db->set_query("seLect @@identity");
        ret = db->exec(true);
        if (ret == 0)
        {
            if (db->fetch())
            {
                if (!db->get_uint(0, letter_id))
                {
                    letter_id = 0;
                    ret = 2;
                }
            }
            else
            {
                ret = 2;
            }
        }
    }
    return ret;
}

unsigned int HandlerFor_DB_::insertPackageData(nsl::DBConnection* db,
                                               tagGAME_DB_SEND_PACKAGE* sendPackageInfo,
                                               unsigned int& postal_id, int mailType)
{
    char strSenderName[87];
    char randomOption[29];
    time_t nInsertTime;
    tm* tmCurTime;
    unsigned char isUnlimit;
    unsigned char isAvatar;
    unsigned char isCreature;
    unsigned int letter_id;
    int ret;

    time(&nInsertTime);
    tmCurTime = localtime(&nInsertTime);
    tmCurTime->tm_hour = tmCurTime->tm_hour + 1;
    tmCurTime->tm_min = 0;
    tmCurTime->tm_sec = 0;
    nInsertTime = mktime(tmCurTime);
    isUnlimit = 1;
    isAvatar = 0;
    isCreature = 0;
    if (sendPackageInfo->letter_text_length < 0x100)
    {
        sendPackageInfo->letter_text[sendPackageInfo->letter_text_length] = '\0';
    }
    else
    {
        sendPackageInfo->letter_text[0xff] = '\0';
    }
    if (sendPackageInfo->item_info.item_id != 0)
    {
        unsigned int check_category =
            (unsigned int)G_Auction()->GetItemInfo(sendPackageInfo->item_info.item_id)->category_;
        if (G_Auction()->IsAvatarCategory(check_category))
        {
            isAvatar = 1;
            isCreature = 0;
        }
        else if (((int)check_category < 0x36b1) || (0x36b4 < (int)check_category))
        {
            isAvatar = 0;
            isCreature = 0;
        }
        else
        {
            isAvatar = 0;
            isCreature = 1;
        }
        if ((isAvatar != 0) &&
            (sendPackageInfo->owner_id != (int)sendPackageInfo->receiver))
        {
            ret = changeAvatarOwner(db, sendPackageInfo->item_info.add_info,
                                    sendPackageInfo->owner_id, sendPackageInfo->receiver);
            if (ret != 0)
            {
                G_TraceLog()->sysLog(7, "ERROR, HandlerFor_DB_::changeAvatarOwner()");
                return ret;
            }
        }
    }
    letter_id = 0;
    if (sendPackageInfo->letter_text_length != 0)
    {
        if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
        {
            ret = insertPackage(db, sendPackageInfo->receiver, 0, SENDER_NAME_GOLD,
                                sendPackageInfo->letter_text, letter_id, nInsertTime);
        }
        else
        {
            ret = insertPackage(db, sendPackageInfo->receiver, 0, SENDER_NAME,
                                sendPackageInfo->letter_text, letter_id, nInsertTime);
        }
        if (ret != 0)
        {
            G_TraceLog()->sysLog(7, "ERROR, HandlerFor_DB_::insertPackage()");
        }
        if ((sendPackageInfo->item_info.item_id == 0) && (sendPackageInfo->money == 0))
        {
            G_TraceLog()->sysLog(7, "ERROR, HandlerFor_DB_::insertPackage() no Item, no Money");
            return ret;
        }
    }
    memset(strSenderName, 0, 87);
    if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
    {
        db->escape_string(strSenderName, SENDER_NAME_GOLD);
    }
    else
    {
        db->escape_string(strSenderName, SENDER_NAME);
    }
    if (isAvatar != 0)
    {
        sendPackageInfo->item_info.uniItemAttr = '\0';
    }
    memset(randomOption, 0, 29);
    DNFFLib::Binary2Hex((unsigned char*)&sendPackageInfo->item_info.random_option_, 0xe,
                        randomOption);
    unsigned char separate_upgrade =
        sendPackageInfo->item_info.separate_info.GetUpgradeSeparate();
    char* guid_str = db->blob_to_str(0, &sendPackageInfo->item_info.guid_, 10);
    snprintf(mSzBuffer, 0x6000,
        "inSert into postal(occ_time,send_charac_no,receive_charac_no,seal_flag,item_id,add_info,endurance,upgrade,amplify_option,amplify_value,gold,send_charac_name,creature_flag,avata_flag,letter_id,extend_info,unlimit_flag,item_guid,random_option,seperate_upgrade, type) values(from_unixtime(%d),%d,%d,%d,%u,%d,%d,%d,%d,%d,%d,\'%s\',%d,%d,%d,%d,%d,\'%s\',0x%s,%d,%d)",
        nInsertTime, sendPackageInfo->owner_id, sendPackageInfo->receiver,
        (int)sendPackageInfo->item_info.seal, sendPackageInfo->item_info.item_id,
        sendPackageInfo->item_info.add_info, (int)sendPackageInfo->item_info.endurance,
        (int)sendPackageInfo->item_info.uniItemAttr,
        (int)sendPackageInfo->item_info.abilityType_,
        (int)sendPackageInfo->item_info.abilityValue_, sendPackageInfo->money,
        strSenderName, (int)isCreature, (int)isAvatar, letter_id,
        sendPackageInfo->item_info.extendInfo, (int)isUnlimit, guid_str, randomOption,
        (int)separate_upgrade, mailType);
    ret = db->exec(true);
    if (ret == 0)
    {
        postal_id = 0;
        db->set_query("seLect @@identity");
        ret = db->exec(true);
        if (ret == 0)
        {
            if (db->fetch())
            {
                if (!db->get_uint(0, postal_id))
                {
                    postal_id = 0;
                    G_TraceLog()->sysLog(7,
                        "ERROR, HandlerFor_DB_::insertPackage(), get_uint(seLect @@identity)");
                    ret = 2;
                }
            }
            else
            {
                G_TraceLog()->sysLog(7,
                    "ERROR, HandlerFor_DB_::insertPackage(), fetch(seLect @@identity)");
                ret = 2;
            }
        }
    }
    else
    {
        G_TraceLog()->sysLog(7, "ERROR, HandlerFor_DB_::insertPackage(), inSert into postal");
    }
    return ret;
}

unsigned int HandlerFor_DB_::changeAvatarOwner(nsl::DBConnection* db,
                                               unsigned int src_item_key,
                                               unsigned int src_charac_no,
                                               unsigned int trg_charac_no)
{
    db->set_query("upDate user_items set charac_no=%u,slot=%d where ui_id=%d && charac_no=%u",
                  trg_charac_no, 10, src_item_key, src_charac_no);
    return db->exec(true);
}

unsigned int HandlerFor_DB_::onGAME_DB_SEND_PACKAGE(nsl::CMsgCell* pCell)
{
    DBConnection* db;
    LPDBTR_GAME_DB_SEND_PACKAGE pContext;
    unsigned int postal_id;
    int ret;
    DBConnection* db_auction;
    char* yyyymm;

    G_TraceLog()->sysLog(6, "In  onGAME_DB_SEND_PACKAGE");
    db = pApp->super_DBConnections.getDBConnection(0);
    pContext = (LPDBTR_GAME_DB_SEND_PACKAGE)pCell->GetDBTr();
    postal_id = 0;
    G_TraceLog()->sysLog(6, "In  onGAME_DB_SEND_PACKAGE, insertPackageData(owner)");
    ret = insertPackageData(db, pContext, postal_id, 4);
    G_TraceLog()->sysLog(6, "Out  onGAME_DB_SEND_PACKAGE, insertPackageData(owner) (%d)", ret);
    if (ret == 0)
    {
        G_TraceLog()->sysLog(6, "In  onGAME_DB_SEND_PACKAGE, update_history(owner)", ret);
        db_auction = pApp->super_DBConnections.getDBConnection(1);
        yyyymm = GetYYYYMM();
        snprintf(mSzBuffer, 0x6000,
            "upDate auction_history_buyer_%s set pre_buyer_postal_id=%d where auction_id=%llu and pre_price=%d",
            yyyymm, postal_id, pContext->auction_id, pContext->money);
        G_TraceLog()->sysLog(6, "%s", mSzBuffer);
        db_auction->set_query("%s", mSzBuffer);
        ret = db_auction->exec(true);
        G_TraceLog()->sysLog(6, "Out  onGAME_DB_SEND_PACKAGE, update_history(owner) (%d)", ret);
        G_Auction()->SendMessageToMonitor(pContext->receiver, 0, pContext->temp_item_id,
                                          pContext->item_info.random_option_);
        G_TraceLog()->sysLog(6, "Out  onGAME_DB_SEND_PACKAGE (%d)", ret);
    }
    else
    {
        G_TraceLog()->sysLog(6, "Out  onGAME_DB_SEND_PACKAGE (%d)", ret);
    }
    return ret;
}

unsigned int HandlerFor_DB_::onGAME_DB_SEND_PACKAGE_BY_EXPIRE(nsl::CMsgCell* pCell)
{
    DBConnection* db_game;
    LPDBTR_GAME_DB_SEND_PACKAGE_BY_EXPIRE pContext;
    unsigned int owner_postal_id;
    unsigned int buyer_postal_id;
    int ret;
    DBConnection* db_auction;
    char* yyyymm;

    G_TraceLog()->sysLog(6, "In  onGAME_DB_SEND_PACKAGE_BY_EXPIRE");
    db_game = pApp->super_DBConnections.getDBConnection(0);
    pContext = (LPDBTR_GAME_DB_SEND_PACKAGE_BY_EXPIRE)pCell->GetDBTr();
    owner_postal_id = 0;
    buyer_postal_id = 0;
    if (pContext->b_exist_buyer)
    {
        G_TraceLog()->sysLog(6, "In  onGAME_DB_SEND_PACKAGE_BY_EXPIRE, insertPackageData(buyer:%d)",
                             pContext->send_to_buyer.receiver);
        ret = insertPackageData(db_game, &pContext->send_to_buyer, buyer_postal_id, 4);
        G_TraceLog()->sysLog(6,
            "Out  onGAME_DB_SEND_PACKAGE_BY_EXPIRE, insertPackageData(buyer) (%d)", ret);
        if (ret != 0)
        {
            G_TraceLog()->sysLog(6, "Out  onGAME_DB_SEND_PACKAGE_BY_EXPIRE (%d)", ret);
            return ret;
        }
    }
    G_TraceLog()->sysLog(6, "In  onGAME_DB_SEND_PACKAGE_BY_EXPIRE, insertPackageData(owner:%d)",
                         pContext->send_to_owner.owner_id);
    if (pContext->send_to_owner.owner_id == -1)
    {
        ret = 0;
    }
    else
    {
        ret = insertPackageData(db_game, &pContext->send_to_owner, owner_postal_id, 5);
    }
    G_TraceLog()->sysLog(6,
        "Out  onGAME_DB_SEND_PACKAGE_BY_EXPIRE, insertPackageData(owner) (%d)", ret);
    if (ret == 0)
    {
        G_TraceLog()->sysLog(6, "In  onGAME_DB_SEND_PACKAGE_BY_EXPIRE, update_history(owner)", ret);
        db_auction = pApp->super_DBConnections.getDBConnection(1);
        yyyymm = GetYYYYMM();
        snprintf(mSzBuffer, 0x6000,
            "upDate auction_history_%s set owner_postal_id=%d,buyer_postal_id=%d where auction_id=%llu and owner_postal_id = 0 and buyer_postal_id = 0",
            yyyymm, owner_postal_id, buyer_postal_id, pContext->send_to_owner.auction_id);
        G_TraceLog()->sysLog(6, "%s", mSzBuffer);
        db_auction->set_query("%s", mSzBuffer);
        ret = db_auction->exec(true);
        G_TraceLog()->sysLog(6, "Out  onGAME_DB_SEND_PACKAGE_BY_EXPIRE, update_history(owner) (%d)",
                             ret);
        unsigned char package_type = (unsigned char)pContext->package_type;
        if (package_type == 1)
        {
            G_Auction()->SendMessageToMonitor(pContext->send_to_buyer.receiver, 0x02,
                pContext->send_to_buyer.temp_item_id,
                pContext->send_to_buyer.item_info.random_option_);
            if (pContext->send_to_owner.owner_id != -1)
            {
                G_Auction()->SendMessageToMonitor(pContext->send_to_owner.receiver, 0x01,
                    pContext->send_to_owner.temp_item_id,
                    pContext->send_to_buyer.item_info.random_option_);
            }
        }
        else if (package_type < 2)
        {
            if (package_type == 0)
            {
                if (pContext->b_exist_buyer == 0)
                {
                    if (pContext->send_to_owner.owner_id != -1)
                    {
                        G_Auction()->SendMessageToMonitor(pContext->send_to_owner.receiver, 0x03,
                            pContext->send_to_owner.temp_item_id,
                            pContext->send_to_owner.item_info.random_option_);
                    }
                }
                else
                {
                    G_Auction()->SendMessageToMonitor(pContext->send_to_buyer.receiver, 0x02,
                        pContext->send_to_buyer.temp_item_id,
                        pContext->send_to_buyer.item_info.random_option_);
                    if (pContext->send_to_owner.owner_id != -1)
                    {
                        G_Auction()->SendMessageToMonitor(pContext->send_to_owner.receiver, 0x01,
                            pContext->send_to_owner.temp_item_id,
                            pContext->send_to_buyer.item_info.random_option_);
                    }
                }
            }
        }
        else if (package_type == 2)
        {
            if (pContext->b_exist_buyer != 0)
            {
                G_Auction()->SendMessageToMonitor(pContext->send_to_buyer.receiver, 0x04,
                    pContext->send_to_buyer.temp_item_id,
                    pContext->send_to_owner.item_info.random_option_);
            }
            if (pContext->send_to_owner.owner_id != -1)
            {
                G_Auction()->SendMessageToMonitor(pContext->send_to_owner.receiver, 0x05,
                    pContext->send_to_owner.temp_item_id,
                    pContext->send_to_owner.item_info.random_option_);
            }
        }
        else if (package_type == 3)
        {
            G_Auction()->SendMessageToMonitor(pContext->send_to_buyer.receiver, 0x02,
                pContext->send_to_buyer.temp_item_id,
                pContext->send_to_buyer.item_info.random_option_);
            if (pContext->send_to_owner.owner_id != -1)
            {
                G_Auction()->SendMessageToMonitor(pContext->send_to_owner.receiver, 0x01,
                    pContext->send_to_owner.temp_item_id,
                    pContext->send_to_buyer.item_info.random_option_);
            }
        }
        G_TraceLog()->sysLog(6, "Out  onGAME_DB_SEND_PACKAGE_BY_EXPIRE (%d)", ret);
    }
    else
    {
        G_TraceLog()->sysLog(6, "Out  onGAME_DB_SEND_PACKAGE_BY_EXPIRE (%d)", ret);
    }
    return ret;
}
unsigned int HandlerFor_DB_::onAUCTION_DB_REGIST_ITEM(nsl::CMsgCell* pCell)
{
    char owner_name[26];
    char buyer_name[26];
    int expire_time;
    char randomOption[29];
    DBConnection* db;
    LPDBTR_AUCTION_DB_REGIST_ITEM pContext;
    int ret;

    G_TraceLog()->sysLog(6, "In  onAUCTION_DB_REGIST_ITEM");
    db = pApp->super_DBConnections.getDBConnection(1);
    pContext = (LPDBTR_AUCTION_DB_REGIST_ITEM)pCell->GetDBTr();
    memset(owner_name, 0, 0x1a);
    memset(buyer_name, 0, 0x1a);
    db->escape_string(owner_name, pContext->owner_name);
    db->escape_string(buyer_name, pContext->buyer_name);
    expire_time = 0;
    expire_time = G_Auction()->GetExpireTime();
    if (pContext->owner_id == -1)
    {
        expire_time = G_Auction()->GetSystemAuctionExpireTime();
    }
    memset(randomOption, 0, 29);
    DNFFLib::Binary2Hex((unsigned char*)&pContext->item_info.random_option_, 0xe, randomOption);
    unsigned char separate_upgrade = pContext->item_info.separate_info.GetUpgradeSeparate();
    char* guid_str = db->blob_to_str(0, &pContext->item_info.guid_, 10);
    snprintf(mSzBuffer, 0x6000,
        "inSert into auction_main(auction_id,occ_time,expire_time,owner_id,owner_name,buyer_id,buyer_name,price,instant_price,seal_flag,item_id,add_info,upgrade,seal_cnt,endurance,extend_info,amplify_option,amplify_value,owner_type,item_guid,unit_price,black_point,random_option,roi_high_key, roi_low_key, owner_nexon_id,seperate_upgrade) values(%llu,from_unixtime(%d),%d,%d,\'%s\',%d,\'%s\',%d,%d,%d,%u,%d,%hhu,%hhu,%hu,%u,%hhu,%hu,%hhu,\'%s\',%d,%d,0x%s,%lld,%d,\'%s\',%hhu)",
        pContext->auction_id, pContext->expire_time - expire_time, pContext->expire_time,
        pContext->owner_id, owner_name, pContext->buyer_id, buyer_name, pContext->price,
        pContext->instant_price, (int)pContext->item_info.seal, pContext->item_info.item_id,
        pContext->item_info.add_info, (unsigned int)(pContext->item_info.uniItemAttr & 0x1f),
        (unsigned int)(pContext->item_info.uniItemAttr >> 5), pContext->item_info.endurance,
        pContext->item_info.extendInfo, (unsigned int)pContext->item_info.abilityType_,
        pContext->item_info.abilityValue_, (unsigned int)pContext->owner_type, guid_str,
        pContext->unit_price, pContext->black_point, randomOption,
        pContext->_reg_roi_category_key.field_0._qw,
        pContext->_reg_roi_category_key.field_1._low_category_key,
        pContext->owner_nexon_id, separate_upgrade);
    G_TraceLog()->sysLog(6, "%s", mSzBuffer);
    db->set_query("%s", mSzBuffer);
    ret = db->exec(true);
    G_TraceLog()->sysLog(6, "out onAUCTION_DB_REGIST_ITEM");
    return ret;
}

unsigned int HandlerFor_DB_::onAUCTION_DB_UPPER_BIDDING(nsl::CMsgCell* pCell)
{
    DBConnection* db;
    LPDBTR_AUCTION_DB_UPPER_BIDDING pContext;
    int ret;

    G_TraceLog()->sysLog(6, "In  onAUCTION_DB_UPPER_BIDDING");
    db = pApp->super_DBConnections.getDBConnection(1);
    pContext = (LPDBTR_AUCTION_DB_UPPER_BIDDING)pCell->GetDBTr();
    snprintf(mSzBuffer, 0x6000,
        "upDate auction_main set buyer_id=%d,buyer_name=\'%s\',price=%d where auction_id=%llu",
        pContext->buyer_id, pContext->buyer_name, pContext->price, pContext->auction_id);
    G_TraceLog()->sysLog(6, "%s", mSzBuffer);
    db->set_query("%s", mSzBuffer);
    ret = db->exec(true);
    G_TraceLog()->sysLog(6, "out onAUCTION_DB_UPPER_BIDDING");
    return ret;
}

unsigned int HandlerFor_DB_::onAUCTION_DB_INSERT_AVERAGE_PRICE(nsl::CMsgCell* pCell)
{
    DBConnection* db;
    LPDBTR_AUCTION_DB_INSERT_AVERAGE_PRICE pContext;
    int ret;

    G_TraceLog()->sysLog(6, "In  onAUCTION_DB_INSERT_AVERAGE_PRICE");
    db = pApp->super_DBConnections.getDBConnection(1);
    pContext = (LPDBTR_AUCTION_DB_INSERT_AVERAGE_PRICE)pCell->GetDBTr();
    if (pContext->_roi_average_key.option_category.isEmpty())
    {
        db->set_query(
            "inSert into auction_average_price(item_id,upgrade,average_price,seperate_upgrade) values(%u,%hhu,%d,%hhu)",
            pContext->item_id, (unsigned int)pContext->upgrade, pContext->average_price,
            (unsigned int)pContext->seperate_upgrade);
    }
    else
    {
        db->set_query(
            "inSert into auction_roi_average_price(item_id,upgrade,average_price,roi_high_key,roi_low_key,roi_index1,roi_index2,roi_index3,real_purchase_count,seperate_upgrade) values(%u,%hhu,%d,%llu,%d,%d,%d,%d,1,%hhd)",
            pContext->item_id, (unsigned int)pContext->upgrade, pContext->average_price,
            pContext->_roi_average_key.option_category.field_0._qw,
            pContext->_roi_average_key.option_category.field_1._low_category_key,
            (int)pContext->_roi_average_key._oiv.option_index_value[0],
            (int)pContext->_roi_average_key._oiv.option_index_value[1],
            (int)pContext->_roi_average_key._oiv.option_index_value[2],
            (signed char)pContext->seperate_upgrade);
    }
    ret = db->exec(true);
    G_TraceLog()->sysLog(6, "out onAUCTION_DB_INSERT_AVERAGE_PRICE");
    return ret;
}

unsigned int HandlerFor_DB_::onAUCTION_DB_GET_AVERAGE_PRICE(nsl::CMsgCell* pCell)
{
    DBConnection* db;
    LPDBTR_AUCTION_DB_GET_AVERAGE_PRICE pContext;
    int ret;
    char bRet;
    unsigned int row;

    G_TraceLog()->sysLog(6, "In  onAUCTION_DB_GET_AVERAGE_PRICE");
    db = pApp->super_DBConnections.getDBConnection(1);
    db->set_query(
        "upDate auction_average_price as A, auction_manual_average_price as B SET A.average_price = B.average_price where A.item_id = B.item_id and A.upgrade = B.upgrade and B.is_apply = 0");
    ret = db->exec(true);
    if (ret != 0)
    {
        G_TraceLog()->sysLog(7, "out onAUCTION_DB_GET_AVERAGE_PRICE(auction_manual_average_price), ERROR");
        exit(1);
    }
    unsigned long long updated = db->getAffectedRowCount();
    G_TraceLog()->sysLog(5, "auction average price is updated manually(%d updated)", updated);
    db->set_query("upDate auction_manual_average_price set is_apply = 1 where is_apply = 0");
    ret = db->exec(true);
    if (ret != 0)
    {
        G_TraceLog()->sysLog(7,
            "out onAUCTION_DB_GET_AVERAGE_PRICE(auction_manual_average_price is_apply), ERROR",
            updated);
        exit(1);
    }
    updated = db->getAffectedRowCount();
    G_TraceLog()->sysLog(5, "auction_manual_average_price disabled(%d rows)", updated);
    pContext = (LPDBTR_AUCTION_DB_GET_AVERAGE_PRICE)pCell->GetDBTr();
    db->set_query("seLect item_id, upgrade, average_price, seperate_upgrade from auction_average_price");
    ret = db->exec(true);
    bRet = 1;
    if (ret != 0)
    {
        G_TraceLog()->sysLog(7, "out onAUCTION_DB_GET_AVERAGE_PRICE, ERROR");
        exit(1);
    }
    row = 0;
    ret = 0;
    do
    {
        if (db->get_n_rows() <= row)
        {
loop_check:
            if (bRet == 1)
            {
                G_TraceLog()->sysLog(6, "out onAUCTION_DB_GET_AVERAGE_PRICE");
                return 0;
            }
            G_TraceLog()->sysLog(7, "Fail to fatch(). process exits.");
            puts("***************Fail to fatch(). process exits.************");
            exit(2);
        }
        bRet = db->fetch();
        if (!bRet)
        {
            G_TraceLog()->sysLog(7, "ERROR, db->fetch()");
            goto loop_check;
        }
        ret = (unsigned int)db->get_ulong(0, pContext->item_id);
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        ret = (unsigned int)db->get_ubyte(1, pContext->upgrade);
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        ret = (unsigned int)db->get_int(2, pContext->average_price);
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        unsigned char refine = 0;
        ret = (unsigned int)db->get_ubyte(3, refine);
        if (ret == 0)
        {
            goto loop_check;
        }
        ROI_AverageKey _temp_roi_average;
        G_Auction()->AddItemAveragePrice(pContext->item_id, pContext->upgrade,
                                         pContext->average_price, 0, _temp_roi_average,
                                         refine, true);
        row = row + 1;
    } while (true);
}

unsigned int HandlerFor_DB_::onAUCTION_DB_GET_ROI_AVERAGE_PRICE(nsl::CMsgCell* pCell)
{
    DBConnection* db;
    LPDBTR_HEADER pContext;
    int ret;
    char bRet;
    unsigned int row;

    G_TraceLog()->sysLog(6, "In  onAUCTION_DB_GET_ROI_AVERAGE_PRICE");
    db = pApp->super_DBConnections.getDBConnection(1);
    pContext = pCell->GetDBTr();
    db->set_query(
        "seLect db_inf_max_price,db_inf_min_price,db_inf_prob,db_inf_limit_count,db_inf_base_mul_min_a,db_inf_base_mul_max_b from auction_roi_constraint");
    ret = db->exec(true);
    bRet = 1;
    if (ret != 0)
    {
        G_TraceLog()->sysLog(7, "out onAUCTION_DB_GET_ROI_AVERAGE_PRICE roi_constraint, ERROR");
        exit(1);
    }
    ROI_Average_Constraint _roi_average_constraint;
    if (db->get_n_rows() == 1)
    {
        bRet = db->fetch();
        if (!bRet)
        {
            G_TraceLog()->sysLog(7,
                "out onAUCTION_DB_GET_ROI_AVERAGE_PRICE roi_constraint, ERROR db->fetch()");
            exit(1);
        }
        if (!db->get_int(0, _roi_average_constraint.inf_max_price))
        {
            return 1;
        }
        if (!db->get_int(1, _roi_average_constraint.inf_min_price))
        {
            return 1;
        }
        if (!db->get_int(2, _roi_average_constraint.inf_prob))
        {
            return 1;
        }
        if (!db->get_int(3, _roi_average_constraint.inf_limit_count))
        {
            return 1;
        }
        if (!db->get_int(4, _roi_average_constraint.inf_base_mul_min_a))
        {
            return 1;
        }
        if (!db->get_int(5, _roi_average_constraint.inf_base_mul_max_b))
        {
            return 1;
        }
    }
    G_Auction()->Set_ROI_Constraint(_roi_average_constraint);
    db->set_query(
        "seLect item_id,upgrade,roi_high_key,roi_low_key,roi_index1,roi_index2,roi_index3,average_price,real_purchase_count,seperate_upgrade from auction_roi_average_price");
    ret = db->exec(true);
    bRet = 1;
    if (ret != 0)
    {
        G_TraceLog()->sysLog(7, "out onAUCTION_DB_GET_ROI_AVERAGE_PRICE, ERROR");
        exit(1);
    }
    ROI_AverageKey _temp_roi_average_key;
    unsigned int _purchase_cnt = 0;
    row = 0;
    do
    {
        if (db->get_n_rows() <= row)
        {
loop_check:
            if (bRet == 1)
            {
                G_TraceLog()->sysLog(6, "out onAUCTION_DB_GET_ROI_AVERAGE_PRICE");
                return 0;
            }
            G_TraceLog()->sysLog(7, "Fail to fatch(). process exits.");
            puts("***************Fail to fatch(). process exits.************");
            exit(2);
        }
        bRet = db->fetch();
        if (!bRet)
        {
            G_TraceLog()->sysLog(7, "ERROR, db->fetch()");
            goto loop_check;
        }
        ret = (unsigned int)db->get_ulong(0, *(ulong*)&pContext[1].dbId);
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        ret = (unsigned int)db->get_ubyte(1, *(unsigned char*)&pContext[1].dbtrId);
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        ret = (unsigned int)db->get_longlong(2,
            *(long long*)&_temp_roi_average_key.option_category.field_0._qw);
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        ret = (unsigned int)db->get_int(3, _temp_roi_average_key.option_category.field_1._low_category_key);
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        ret = (unsigned int)db->get_short(4, _temp_roi_average_key._oiv.option_index_value[0]);
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        ret = (unsigned int)db->get_short(5, _temp_roi_average_key._oiv.option_index_value[1]);
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        ret = (unsigned int)db->get_short(6, _temp_roi_average_key._oiv.option_index_value[2]);
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        ret = (unsigned int)db->get_int(7, *(int*)((char*)&pContext[1].dbtrId + 1));
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        ret = (unsigned int)db->get_uint(8, _purchase_cnt);
        if (ret == 0)
        {
            ret = 0;
            goto loop_check;
        }
        unsigned char refine = 0;
        ret = (unsigned int)db->get_ubyte(9, refine);
        if (ret == 0)
        {
            goto loop_check;
        }
        _temp_roi_average_key.baseItem_index = pContext[1].dbId;
        G_Auction()->AddItemAveragePrice(pContext[1].dbId, (unsigned char)pContext[1].dbtrId,
                                         *(int*)((char*)&pContext[1].dbtrId + 1), _purchase_cnt,
                                         _temp_roi_average_key, refine, true);
        row = row + 1;
    } while (true);
}
