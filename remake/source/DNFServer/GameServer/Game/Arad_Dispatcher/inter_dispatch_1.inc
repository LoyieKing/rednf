bool Arad_Dispatcher_Manager::inter_dispatch(INTERNAL_PACKET_CODE code, CUser* user,
                                             char* body, int len)
{
    // ORIG 0x0818ff6c：查 inter 组，虚槽3 调用；err!=0 时记日志并对 user 断连。
    Arad_Dispatcher* d = m_interDispatchers[code];
    if (d == 0)
    {
        // [AradPrett 2026-09-11 · A 类落地] 本 TU 4 处手写 prett 串已全换
        // `__PRETTY_FUNCTION__`，四符号齐（vsz 对 ORIG 逐字节 SAME）：
        //   ①本处 `bool ARAD::Arad_Dispatcher_Manager::inter_dispatch(
        //   ARAD::Arad_Dispatcher_Manager::INTERNAL_PACKET_CODE, CUser*, char*, int)`
        //   （ORIG 0x08b99d80 size 0x7c）：定义侧第 1 形参拼写 `int code` →
        //   `INTERNAL_PACKET_CODE code`（类内 typedef 见 Arad_Dispatcher.h；同类型 ⇒
        //   mangled 不变 `…14inter_dispatchEiP5CUserPci…`）；
        //   ②`virtual int ARAD::Arad_Dispatcher_BuyItemCheck_Impl::inter_dispatch(
        //   CUser*, char*, int)`（0x08b980c0 size 0x58）③同 `…::db_dispatch(int, int,
        //   Stream*)`（0x08b98060 size 0x54）④`int ARAD::DATABASE::ITEM_QUERY::
        //   arad_sp_buy_item_check(ARAD_DATAS::BuyItemInfo&)`（0x08b93ba0 size 0x51）。
        //   ②③ `virtual ` 前缀由类内声明（.h:84-85）决定，类外定义不带关键字亦打印。
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatch_Manager.cpp",
            __PRETTY_FUNCTION__,
            0x7d, "Not found Inter_Dispatch_Code[%d]", code);
        return false;
    }
    // ORIG 0x0818ffeb：`cmpl $0,-0x10; je end(mov$1)`＝ err!=0 才进错误块
    // （尾 mov$1 落函数尾，反向 choose 岛不可用 P63/P191）。
    const int err = d->inter_dispatch(user, body, len);
    if (err != 0)
    {
        unsigned int accId = 0;
        if (user != 0)
        {
            accId = user->get_acc_id();
            user->DisConnSig((DISCONN_SIG)2, 1, err + 1000000);
        }
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatch_Manager.cpp",
            __PRETTY_FUNCTION__,
            0x8d, "Inter Dispatch Result Error Inter_Dispatch_Code[%d] user_acc_id[%d]",
            code, accId);
        return false;
    }
    return true;
}