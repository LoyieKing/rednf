// ORIG T 0x081747fe（C2 同址别名）
// N76 封顶：uid 写入块 4.4 恒 LHS(this/ecx) 先物化、ORIG 4.1 恒 RHS(槽回读)先，
// 3 行时序差不可翻；函数体赋值序（price,pay,uid）已复原，帧 0x28 一致。
GiftQuery::GiftQuery(const char* account, const char* sendCharac,
                     const char* recvCharac, unsigned int price,
                     unsigned int pay, long long uid)
{
    m_account = account;
    m_sendCharac = sendCharac;
    m_recvCharac = recvCharac;
    m_price = price;
    m_pay = pay;
    m_uid = uid;
    m_resultCode = 0;
    m_trans.init();
}