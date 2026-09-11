bool CAuctionAveragePrice::LoadAveragePrice()
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_C, E_SERVER_GROUP_NONE);
    mysql->set_query("seLect item_id,upgrade,average_price from auction_average_price");
    if (!mysql->exec(true))
    {
        return false;
    }
    if (mysql->get_n_rows() == 0)
    {
        return true;
    }

    int itemID = 0;
    int price = 0;
    unsigned char upgrade = 0;
    char local11 = 0;
    unsigned int count;
    for (count = 0; count < (unsigned int)mysql->get_n_rows(); ++count)
    {
        if (!mysql->fetch())
        {
            return false;
        }
        if (!mysql->get_int(0, itemID))
        {
            return false;
        }
        if (!mysql->get_ubyte(1, upgrade))
        {
            return false;
        }
        if (!mysql->get_int(2, price))
        {
            return false;
        }
        SetAveragePrice(itemID, upgrade, price);
    }
    return true;
}