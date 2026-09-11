// ---- IsPossibleDeal（ORIG T 0x85c7c0a，报告 IsPossibleDeal.md 全文）----
// 卖家 CheckMoney 失败 → Do(0x16)+定向错误包；买家余额不足 → Do(10)。a3/a4 未用。
bool CPrivateStore::IsPossibleDeal(int gold, unsigned int /*a3*/, unsigned int /*a4*/)
{
    if (!m_pSeller->CheckMoney(gold))
    {
        m_errorHandler.Do(0x16);
        CPacketBufPrivate packet;
        packet.MakeErrorReturnPacket(0, 0x52, 0x16);
        m_pSeller->Send(packet);
        return false;
    }
    if ((int)m_pBuyer->getCurCharacInvenR()->get_money() < gold)
    {
        m_errorHandler.Do(10);
        return false;
    }
    return true;
}