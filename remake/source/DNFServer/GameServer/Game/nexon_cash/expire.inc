// [T1-GI · P327] UserBalance::expire 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN5nexon4cash11UserBalance6expireEv`（ORIG 0x081b163a，ORIG CU
// UserBalance.cpp）。仅移函数定义，零语义/布局改动。本 TU 为 TCPSocket / Transaction /
// UserBalance 合并桶，一 TU 一 GI ⇒ 另两条（tx_id_ / MAX_SEND_RETRY）需拆 TU 才可消。
// ORIG 0x081b147c：遍历删除 expired 者（tx->cancel() 退款+出管理器表，
// erase(it) 复用迭代器）。
void UserBalance::expire()
{
    std::vector<Transaction*>::iterator it = m_transactions.begin();
    while (it != m_transactions.end())
    {
        if ((*it)->expired())
        {
            (*it)->cancel();
            it = m_transactions.erase(it);
        }
        else
        {
            ++it;
        }
    }
}