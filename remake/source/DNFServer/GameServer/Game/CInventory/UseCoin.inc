int CInventory::UseCoin(eCoinSubReason reason)
{
    // ORIG 0x84faa98：!=0 递降取向（coin→event→pay），history 调用内联、
    // trace 块跳转目标。b28c-L2：ORIG 三 trace 块槽为 首块 -0x38 / 后两块 -0x18
    // （互斥块同型临时槽方向差）⇒ 具名对象改全表达式临时链（P221，4.4 复现 ORIG 槽深）。
    if (m_coin != 0)
    {
        m_coin--;
        if (m_pParent != 0)
        {
            m_pParent->m_historyLog.CoinSub(m_coin, 1, reason);
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x28e, 5)("History ERROR, m_pParent NULL, CoinSub");
        }
    }
    else
    {
        if (m_eventCoin != 0)
        {
            m_eventCoin--;
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.EventCoinSub(m_eventCoin, 1, reason);
            }
            else
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x29d, 5)("History ERROR, m_pParent NULL, EventCoinSub");
            }
        }
        else
        {
            if (m_payCoin != 0)
            {
                m_payCoin--;
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.PayCoinSub(m_payCoin, 1, reason);
                }
                else
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x2ad, 5)("History ERROR, m_pParent NULL, PayCoinSub");
                }
            }
            else
            {
                m_coin = 0;
                m_payCoin = 0;
                m_eventCoin = 0;
            }
        }
    }
    return m_coin;
