void Secu_GoldControl::SubAuctionMoney(unsigned int value)
{
    // P63 同 AddAuctionMoney。
    if (value <= 4999)
        return;
    {
        m_cur.m_auctionSub += value;
        m_total.m_auctionSub += value;
        m_total.m_count[1]++;

        // ORIG 无中间槽：getHades() 返回值直接作 this（P0 去缓存，具名局部
        // 会诱发 -0xc 槽往返 + 栈帧 0x24、GoldMinus 参数走 eax 而非 edx）。
        ((XNuclear::CHades*)m_pUser->getHades())->
            GoldMinus(value, (XNuclear::ENUM_GOLD_CONTROL_REASON)2);
    }
