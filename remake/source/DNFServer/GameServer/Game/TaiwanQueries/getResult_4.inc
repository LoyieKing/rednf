// ORIG T 0x08174b44
bool ConfirmQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_code"))
            throw static_cast<int>(0x1ee);
        if (!db.exec(true))
            throw static_cast<int>(0x1f1);
        if (!db.fetch())
            throw static_cast<int>(0x1f4);
        if (!db.get_int(0, m_resultCode))
            throw static_cast<int>(0x1f7);
        return 1;
    }
    catch (int e)   // ORIG: 按值 catch —— begin_catch 后先 deref 再存槽
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x1fd,
            "[Taiwan, Billing] ConfirmQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}