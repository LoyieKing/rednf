// ---- ORIG T 0x0813375c（advancealtar__CharacAdvanceAltarManager/gmSetTicket.md）----
void CharacAdvanceAltarManager::gmSetTicket(int ticket)
{
    // ORIG 无槽无调用：this+0 成员逐次重读（P0/P183）。
    if (m_currentAdvanceAltarData == 0)
    {
        return;
    }
    m_currentAdvanceAltarData->m_ticket = ticket;  // ORIG *(cur+4)=param
    m_currentAdvanceAltarData->m_update = 1;       // ORIG *(cur+0xcc)=1
}