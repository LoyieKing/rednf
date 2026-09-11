// ORIG T 0x081752cc
bool CeraPointQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_code"))
            throw static_cast<int>(0x282);
        if (!db.exec(true))
            throw static_cast<int>(0x285);
        if (!db.fetch())
            throw static_cast<int>(0x288);
        if (!db.get_int(0, m_resultCode))
            throw static_cast<int>(0x28b);
        return 1;
    }
    catch (int e)   // ORIG: 按值 catch —— begin_catch 后先 deref 再存槽
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x291,
            "[Taiwan, Billing] ChargePointQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}