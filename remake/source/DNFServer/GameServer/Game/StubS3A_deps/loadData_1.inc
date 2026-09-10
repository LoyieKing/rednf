bool CItemMakingSkill::loadData(CUser* pUser, char* pData)
{
    (void)pUser;
    _reset();   // ORIG 0x08545218：虚调 vtable slot7
    // ORIG 0x0854521a 槽 -0x10 存 pData 指针；具名为视图指针以复现 `i` 先于基址装载。
    const ItemMakingSkillReqView* buf =
        reinterpret_cast<const ItemMakingSkillReqView*>(pData);
    for (int i = 0; i <= 0xa; ++i)
    {
        // ORIG 0x08545229：两次独立直读（P48）。
        // (short) cast 复现 ORIG 有符号 16 位比较（0x08545234 `cmp $0x3e7,%ax; jle`）。
        if ((short)buf->m_skills[i] > 0x3e7)
            m_skillLevels[i] = 0x3e8;
        else
            m_skillLevels[i] = buf->m_skills[i];
    }
    return true;
