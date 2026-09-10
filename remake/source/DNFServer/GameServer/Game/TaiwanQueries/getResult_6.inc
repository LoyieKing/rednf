bool ChargeQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_tran, @out_code"))
            throw static_cast<int>(0x24c);
        if (!db.exec(true))
            throw static_cast<int>(0x24f);
        if (!db.fetch())
            throw static_cast<int>(0x252);
        if (!db.get_ulonglong(0, m_trans.m_tranNo))
            throw static_cast<int>(0x255);
        if (!db.get_int(1, m_resultCode))
            throw 600;   // ORIG movl $0x258 Ghidra 十进制化显示（同 0x258）
        return 1;
    }
    catch (int e)   // ORIG: 按值 catch —— begin_catch 后先 deref 再存槽
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x25e,
            "[Taiwan, Billing] ChargeQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
