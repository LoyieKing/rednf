bool CharacAdvanceAltarManager::isAvailableTciket() const
{
    if (m_currentAdvanceAltarData == 0)
    {
        return false;
    }
    // ORIG 4 字节读 +0x04 / +0x08 两票池（DB 列对照：+0x04=ticket_free、
    // +0x08=ticket_cera，见 _RidableDbHead；_RidableBase 现有成员名与该列名
    // 相反，属命名口径问题，本批只去裸偏移不改共享成员名）。
    // ORIG 无任何槽：指针逐次重读、和与比较全寄存器直算（P0/P183）。
    return (m_currentAdvanceAltarData->m_ticket +
            m_currentAdvanceAltarData->m_freeTicket) > 0;
