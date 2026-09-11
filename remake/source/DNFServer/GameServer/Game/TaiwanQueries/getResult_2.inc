// ORIG T 0x081742fc
bool BalancePointQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_balance, @out_point, @out_result"))
            throw static_cast<int>(0x14b);
        if (!db.exec(true))
            throw static_cast<int>(0x14e);
        if (!db.fetch())
            throw static_cast<int>(0x151);
        // [b28BM13] 同 BalanceQuery::getResult：ORIG 共享 idx 槽（-0x10）后置自增。
        unsigned int idx = 0;
        if (!db.get_int(idx++, m_balance))
            throw static_cast<int>(0x155);
        if (!db.get_int(idx++, m_point))
            throw static_cast<int>(0x158);
        if (!db.get_int(idx++, m_resultCode))
            throw static_cast<int>(0x15b);
        return 1;
    }
    catch (int e)   // [b28BM13] ORIG 值先物化入槽 ⇒ 按值 catch（同 BalanceQuery）
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x161,
            "[Taiwan, Billing] BalanceQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}