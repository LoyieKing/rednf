bool Secu_AccountHacking::isSameAccountCharacter(char const* name)
{
    // ORIG T 0x08279182（isSameAccountCharacter.md）：绑定用户的角色列表中
    // strncmp(m_name, name, 0x1e)==0 即命中（列表元素 _Charac_info +0x04 起）。
    if (name == 0)
        return false;

    std::vector<_Charac_info>::const_iterator it =
        ((CUser*)m_userP)->m_characList.begin();
    std::vector<_Charac_info>::const_iterator e =
        ((CUser*)m_userP)->m_characList.end();
    while (it != e)
    {
        if (strncmp(it->m_name, name, 0x1e) == 0)
            return true;
        ++it;
    }
    return false;
