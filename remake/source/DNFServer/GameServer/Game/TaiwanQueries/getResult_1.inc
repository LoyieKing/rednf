bool BalanceQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_balance, @out_result"))
            throw static_cast<int>(0x116);
        if (!db.exec(true))
            throw static_cast<int>(0x119);
        if (!db.fetch())
            throw static_cast<int>(0x11c);
        // [b28BM13] ORIG 0x08174092 -0x10(%ebp) 为共享 idx 槽，init 落 fetch
        // 检查之后：addl $1 后置自增、值经 eax 直传首参（P0：有槽⇒具名局部）。
        unsigned int idx = 0;
        if (!db.get_int(idx++, m_balance))
            throw static_cast<int>(0x120);
        if (!db.get_int(idx++, m_resultCode))
            throw static_cast<int>(0x123);
        return 1;
    }
    catch (int e)
    {
        // [b28BM13] ORIG 0x0817422d = `begin_catch; mov (%eax),%eax; mov %eax,-0xc`
        // （值先物化入槽，P35/P0）⇒ 原码为按值 catch；引用形反产指针落槽+回读
        // 解引用两段（b28BM13 实测）。throw(int) 语义等价（POD）。
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x129,
            "[Taiwan, Billing] BalanceQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
