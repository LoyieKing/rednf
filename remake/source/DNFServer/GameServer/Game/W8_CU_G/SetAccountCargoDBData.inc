// ORIG 0x0867d86e：账务仓库 DB 同步转发给内嵌 CAccountCargo。
void CUser::SetAccountCargoDBData(Inven_Item* items, unsigned int money,
                                  unsigned int capacity)
{
    m_accountCargo.SetDBData(this, items, money, capacity);
}