bool PurchaseQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_tran, @out_code"))
            throw static_cast<int>(0x187);
        if (!db.exec(true))
            throw static_cast<int>(0x18a);
        if (!db.fetch())
            throw static_cast<int>(0x18d);
        if (!db.get_ulonglong(0, m_trans.m_tranNo))
            throw static_cast<int>(400);
        if (!db.get_int(1, m_resultCode))
            throw static_cast<int>(0x193);
        return 1;
    }
    catch (int e)   // ORIG: 按值 catch —— begin_catch 后先 deref 再存槽
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x199,
            "[Taiwan, Billing] PurchaseQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
