// ---- getResult —— ORIG 0x0817460e：任一步失败抛 int 错误码
//（0x187=set_query / 0x18a=exec / 0x18d=fetch / 400=get_ulonglong / 0x193=get_int）。
// ORIG 含 catch：LogManager 记录异常后返回 0（行号 0x199）。----
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
}