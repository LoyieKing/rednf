int DATABASE::ITEM_QUERY::arad_sp_buy_item_check(ARAD_DATAS::BuyItemInfo& info)
{
    // ORIG T 0x0818cfab：prod_sale_entry_<quarter> count(*) 与 limitCount 比较。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)7,
                                                  (ENUM_SERVER_GROUP)0);
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x454, "%s", "Database null!!");
        return -1;
    }
    char quarter[5] = {0};  // ORIG `movl $0; movb $0` 两写消 5 字节零
    GetQuarterOfYear(quarter);
    if (info.checkType == 1)
    {
        db->set_query(
            "select count(*) from prod_sale_entry_%s where  ipg_no = %d and "
            " m_id = %d   and  charac_no = %d",
            quarter, info.ipgNo, info.mid, info.characNo);
    }
    else
    {
        db->set_query(
            "select count(*) from prod_sale_entry_%s where  ipg_no = %d and "
            " m_id = %d",
            quarter, info.ipgNo, info.mid);
    }
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x472,
            "[ERROR] DB Exec Error => MID[%d], CHARIDX[%d], ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        return -1;
    }
    if (!db->fetch())
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x47b,
            "[ERROR] DB Fetch Error => MID[%d], CHARIDX[%d], ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        return -1;
    }
    int count = 0;
    if (!db->get_int(0, count))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x485,
            "RESULT_COUNT MID[%d], CHARIDX[%d], ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        return -1;
    }
    info.buyable = 1;
    if (info.limitCount <= count)
        info.buyable = 0;
    return 0;
