// 具名局部 comp 先构造（ORIG ctor 调用居函数首），比较用 eq + je 分支形态。
bool CNPCScript::isKeyItem(unsigned long key, unsigned int id) const
{
    isSameItem comp(key, id);
    std::vector<std::pair<unsigned long, unsigned int> >::const_iterator result =
        std::find_if(m_npcScript.m_favorableRelationShip.m_fav.m_keyItems.begin(),
                     m_npcScript.m_favorableRelationShip.m_fav.m_keyItems.end(), comp);
    if (result == m_npcScript.m_favorableRelationShip.m_fav.m_keyItems.end())
    {
        return false;
    }
    return true;
}