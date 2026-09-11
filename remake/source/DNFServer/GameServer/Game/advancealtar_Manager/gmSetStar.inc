// ---- ORIG T 0x08133720（gmSetStar.md）：当前星=v、星池点=基数+v、update=1。----
void CharacAdvanceAltarManager::gmSetStar(int star)
{
    // ORIG 无槽无调用：this+0 成员逐次重读（P0/P183）。
    if (m_currentAdvanceAltarData == 0)
    {
        return;
    }
    m_currentAdvanceAltarData->m_curStar = star;
    m_currentAdvanceAltarData->m_starPoint =
        m_currentAdvanceAltarData->m_starPoolBase + star;
    m_currentAdvanceAltarData->m_update = 1;
}