// ORIG T 0x08174d60
bool CancelQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_code"))
            throw static_cast<int>(0x21a);
        if (!db.exec(true))
            throw static_cast<int>(0x21d);
        if (!db.fetch())
            throw static_cast<int>(0x220);
        if (!db.get_int(0, m_resultCode))
            throw static_cast<int>(0x223);
        return 1;
    }
    catch (int e)   // ORIG: 按值 catch —— begin_catch 后先 deref 再存槽
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x229,
            "[Taiwan, Billing] CancelQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}