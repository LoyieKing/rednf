void CUserCharacInfo::getExpBonusPoint(ExpBonusPointResult& out)
{
    if (m_selected)
        out = CUR->m_expBonus;
