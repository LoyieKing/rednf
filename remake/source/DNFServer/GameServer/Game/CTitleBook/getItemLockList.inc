void CTitleBook::getItemLockList(
    std::vector<item_lock::stItemLockRef>& out)
{
    // b28d-183/198：ORIG 0x086411ba 实证——无 item 指针缓存（P183 地址重物化：
    // 每次全量展开 m_category[category][i]）；lock 用「声明预清 + 独立赋值」两句式
    // （P198①，ORIG 头部 movb $0x0,-0x11(%ebp)）；ref 赋值序 = lock → slot → space。
    unsigned char lock = 0;
    for (int category = 0; category < 4; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            lock = m_category[category][i].m_amp.GetLock();
            if (lock != 0)
            {
                item_lock::stItemLockRef ref;
                ref.m_lock = lock;
                ref.m_slot = (short)i;
                ref.m_space = (char)category + 0x13;
                out.push_back(ref);
            }
        }
    }
}