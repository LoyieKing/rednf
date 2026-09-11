void CInventory::set_parent(CUser* parent)
{
    m_pParent = parent;
    m_creatureMgr.SetUser(parent);
}