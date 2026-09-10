bool DB_BillingRecord::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_BILLING_RECORD* sig;
    sig = stream->GetOutBuffer<SIG_BILLING_RECORD>();
    if (sig != 0)
    {
        r = false;
        InsertBuyUser(sig, r);
        InsertSaleEntry(sig, r);
        return 1;
    }
    else
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x606e, "SIG_BILLING_RECORD is NULL");
        return 0;
    }
