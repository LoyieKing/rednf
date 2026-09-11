// ---- ORIG T 0x081313b6（decTicket.md）：先扣 m_ticket，不足扣 m_freeTicket；
// 任一成功置 update=1 并返 1（头口径 bool；调用点直证 0x0812e547 `xor $0x1; test; je`）。----
bool CharacAdvanceAltarManager::decTicket()
{
    // [b28d-D33 P183/P0] ORIG 0x081313b6 无 cur 具名槽：每次 `mov 0x8(%ebp),%eax;
    // mov (%eax),%eax` 重读成员并直接 test ⇒ 去缓存逐次重读。
    if (m_currentAdvanceAltarData == 0)
    {
        return 0;
    }
    bool dec = false;
    if (m_currentAdvanceAltarData->m_ticket >= 1)
    {
        --m_currentAdvanceAltarData->m_ticket;
        dec = true;
    }
    else if (m_currentAdvanceAltarData->m_freeTicket > 0)
    {
        --m_currentAdvanceAltarData->m_freeTicket;
        dec = true;
    }
    if (dec)
    {
        m_currentAdvanceAltarData->m_update = 1;
    }
    // [b28d-D33] ORIG 尾 `movzbl -0x1(%ebp),%eax; leave; ret` 直读 bool 槽 ⇒
    // 原码 `return dec;`（bool→char 隐式转换），非三目物化。
    return dec;
}