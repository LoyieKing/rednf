// ---------------------------------------------------------------------------
// _DrawEnchant（报告 _DrawEnchant.md，ORIG T 0x849cac6 sz 0x65）：
// IsSuccess 判定，成功则把结果经验并入当前职业经验。
// ---------------------------------------------------------------------------
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
}