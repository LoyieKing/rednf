// ORIG 0x081b0244：tx 存在→tx->cancel()（退款+自移除）、user 表移除、delete tx。
// 正向 if 包裹（0x081b0263 je 直达 leave +0x51，无尾 jmp/nop 岛）。
void TransactionManager::cancel(unsigned int id)
{
    Transaction* tx = getTransaction(id);
    if (tx != 0)
    {
        tx->cancel();
        tx->getUser()->removeTransaction(tx);
        delete tx;
    }
}