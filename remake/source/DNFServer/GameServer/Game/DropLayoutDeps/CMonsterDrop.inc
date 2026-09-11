CMonsterDrop::CMonsterDrop(int count)
    : CItemGenerator(count), m_hell(2), m_special(3)   // ORIG HellC1(2)/SpecialC1(3)
{
    // [B24D判词 N] 语义正确：基类 CItemGenerator(count) + m_hell(2)/m_special(3)
    //   成员 init + m_firstMobWeight=1/m_firstMobWeightRate=1.0f。P167 差集 ORIG
    //   CItemGeneratorC1/D1（完整构造/析构）vs 我方 C2/D2+CSpecialMonsterDropD1
    //   （基类/成员子对象构造析构 mangling），C1/C2、D1/D2 配对同语义，非缺口。
    // ORIG 0x0853a6c8：基类 → vptr 0x08c9a748 → 向量×2/表/Hell/Special/FirstMob
    // （成员自动）→ +0x259380=1 → +0x259384=1.0f。
    m_firstMobWeight = 1;
    m_firstMobWeightRate = 1.0f;
}