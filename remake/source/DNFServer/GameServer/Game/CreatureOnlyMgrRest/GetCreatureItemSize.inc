// ORIG _ZNK13user_creature12CCreatureMgr19GetCreatureItemSizeEv W 0x8531fbe
//【objdump】：单 call m_creatureItems.size() 后直返。
int CCreatureMgr::GetCreatureItemSize() const
{
    return (int)m_creatureItems.size();
}