int CRandomOptionItemHandleTest::_regist_auction(Inven_Item& item, long long auctionId,
                                                 int occTime, int expireTime,
                                                 char const* ownerName)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_C, E_SERVER_GROUP_NONE);
    char* randomOptionStr = db->blob_to_str(1, (char*)&item + 0x25, 0xe);
    char* guidStr = db->blob_to_str(0, (char*)&item + 0x15, 10);
    // unsigned char：ORIG getAbilityType 后 movzbl 零扩展（P11；char 会 movsbl）。
    unsigned char ampType = item.m_amp.getAbilityType();
    unsigned short ampValue = item.m_amp.getAbilityValue();

    db->set_query(
        "inSert into auction_main(auction_id,occ_time,expire_time,owner_id,owner_name,"
        "buyer_id,buyer_name,price,instant_price,seal_flag,item_id,add_info,upgrade,"
        "seal_cnt,endurance,extend_info,amplify_option,amplify_value,owner_type,"
        "item_guid,unit_price,black_point,random_option) values(%llu,from_unixtime(%d),"
        "%d,%d,'%s',%d,'%s',%d,%d,%d,%u,%d,%hhu,%hhu,%hu,%hu,%hhu,%hu,%hhu,'%s',%d,%d,'%s')",
        auctionId,
        occTime,
        occTime + 0x3f480,
        expireTime,
        ownerName,
        -1,
        "",
        1,
        2,
        (unsigned char)item.m_field0,
        item.m_addInfo,
        item.m_addInfo2,
        (int)(item.m_attr & 0x1f),
        (int)(item.m_attr >> 5),
        item.m_fieldb,
        item.m_fieldd,
        (int)ampType,
        (int)ampValue,
        0,
        guidStr,
        1,
        0,
        randomOptionStr);
    db->exec(true);
    return 1;
