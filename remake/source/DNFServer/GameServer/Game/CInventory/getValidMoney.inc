int CInventory::getValidMoney(int add)
{
    int limit = 0x7fffffff;
    register const char* name = m_pParent->get_acc_name();
    register int level = ((CUserCharacInfo*)m_pParent)->get_charac_level() - 1;
    limit = G_CDataManager()->GetMoneyLimitPerLevel(level, name);
    if (limit < m_money + add)
    {
        return 1;
    }
    return 0;
