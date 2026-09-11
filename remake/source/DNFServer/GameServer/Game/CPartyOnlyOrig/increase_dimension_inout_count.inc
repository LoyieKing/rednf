// ORIG T 0x0859bfe2（increase_dimension_inout_count.md）：副本允许维度进出时，
// 各维度 inout 槽 (dm 上限, 成员现值) 同加 delta 并截断到 dm 上限。
void CParty::increase_dimension_inout_count(int param)
{
    if (m_battleField.m_pDungeon != 0)
    {
        if (m_battleField.m_pDungeon->get_dimension_possible() == 0)
        {
            return;
        }
        {
            for (int i = 0; i <= 3; i++)
            {
                if (!checkValidUser(i))
                {
                    continue;
                }
                {
                    for (int j = 0; j <= 5; j++)
                    {
                        int dmNext = static_cast<char>(
                            G_CDataManager()->get_dimensionInout(j)) +
                            param;                      // -0x10 单份
                        int curNext = static_cast<char>(
                            reinterpret_cast<CUserCharacInfo*>(
                                m_member[i].m_pUser)
                                ->getDemensionInoutValue(
                                    static_cast<char>(j))) +
                            param;                      // -0xc 单份
                        if (curNext > dmNext)
                            curNext = dmNext;
                        reinterpret_cast<CUserCharacInfo*>(
                            m_member[i].m_pUser)
                            ->setDemensionInoutValue(
                                static_cast<char>(j),
                                static_cast<char>(curNext));
                    }
                }
            }
        }
    }
    return;
}