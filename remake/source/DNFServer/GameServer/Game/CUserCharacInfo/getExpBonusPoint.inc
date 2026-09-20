void CUserCharacInfo::getExpBonusPoint(ExpBonusPointResult& out)
{
    if (m_selected)
        out = m_selected->m_expBonusPointResult;
}