bool CEnchanter::_DrawEnchant()
{
    int resultExp = 0;
    if (IsSuccess(m_user,
                  static_cast<CStackableItem const*>(
                      reinterpret_cast<CStackableItem*>(m_field40)),
                  resultExp))
    {
        IncreaseExpertJobExp(m_user, m_user->GetCurCharacExpertJobExp() +
                                         resultExp);
        return true;
    }
    return false;
