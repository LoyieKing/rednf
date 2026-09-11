// ORIG T 0x0868f472 sz0x158：i=0..min(6,m_field796c8) 发 0x1e 字节 MoneyLogPacket；
// 最后 resetMoneyLog()。对象布局为全局 MoneyLogPacket（PacketHeader 10B +
// 5 int @+0xa..+0x1d；ORIG 字段偏移实证），ctor 写 0x2723/0x1e 头并清零。
void CUser::SendMoenyLog()
{
    if (getCurCharacR() != 0)
    {
        for (int i = 0; i < 7 && i <= m_moneyLogIdx; ++i)
        {
            if (m_moneyLogs[i].m_money[0] == 0 &&
                m_moneyLogs[i].m_money[1] == 0)
                continue;
            MoneyLogPacket pkt;
            pkt.m_characNo = getCurCharacNo();
            pkt.m_money0   = m_moneyLogs[i].m_money[0];
            pkt.m_money1   = m_moneyLogs[i].m_money[1];
            pkt.m_accId    = get_acc_id();
            pkt.m_money2   = m_moneyLogs[i].m_money[2];
            // ORIG 0x868f556：&pkt 先求值存 %ebx（跨 GetServerGroup/GetServerProxy
            // 存活），proxy 调用结果不经具名槽，直接 SendPacket。
            GlobalData::s_statistic_proxy_mgr->GetServerProxy(GetServerGroup())
                ->SendPacket((char*)&pkt, 0x1e);
        }
    }
    resetMoneyLog();
}