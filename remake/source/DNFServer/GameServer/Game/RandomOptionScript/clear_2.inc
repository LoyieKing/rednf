void RandomOptionScript::clear()
{
    m_map0.clear();
    m_map18.clear();
    m_map30.clear();
    m_map48.clear();
    m_baseItems60.clear();
    m_map78.clear();
    m_map90.clear();
    m_veca8.clear();
    m_mapb4.clear();
    m_mapcc.clear();
    m_ratioE4.clear();
    m_mapf4.clear();
    m_costVec10c.clear();
    m_map118.clear();
    m_map130.clear();
    // ORIG 0x08372648/0x083726a2：+0x130 clear 后、+0x160 clear 前，
    // 依序 set(&+0x90,&+0xcc) / set(&+0x118,&+0x130)。
    m_providerNormal148.set(&m_map90, &m_mapcc);
    m_providerModified154.set(&m_map118, &m_map130);
    m_map160.clear();
    m_map178.clear();
    m_map190.clear();
    m_prefixNormal.clear();
    m_prefixPremium.clear();
    // ORIG 0x083838c4 起：+0x1d8 vector clear 后直接 +0x1e8 map clear，
    // 中间的 m_str1e4（ORIG C1/D1 实证 +0x1e4 string）不参与 clear()。
    m_choosePrefix1d8.clear();
    m_map1e8.clear();
    m_set200.clear();
    m_set218.clear();
}