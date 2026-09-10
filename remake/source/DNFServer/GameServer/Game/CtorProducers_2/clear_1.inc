void DisjointScript::clear()
{
    // [SweepCtorProd2] 88→119 条 vs ORIG 120：唯一残余 = ORIG 尾部对齐 nop（§6.1
    // 豁免）⇒ AE。p167 CLEAR（3=3）。
    // 已补：① ORIG 0x08375cf3..0x8375d42 parts 段（grade 0..3 × parts 0..9，
    // `movl $0,0x4(%ecx,%eax,4)`、eax=0x110+g*10+p ⇒ 基准 +0x444）；
    // ② 0xcc/0xd0 的 2 元素循环（ORIG 0x8375c5d..0x8375c7c `lea 0x30(%edx),%ecx;
    // mov %edx,0xc(%eax,%ecx,4)`，经 DisJoint.h 匿名 union 数组视角 m_skillProb[2] 复现）。
    // ORIG 0x08375bca：5 容器 clear → 4 LotteryInfo::clear → +0xbc=-1 →
    // skills clear → (+0xcc,+0xd0) 循环清零 → 三重循环 m_avatarDisjoint →
    // 二重循环 m_avatarPartsDisjoint。
    // [b28d-48] P191 配套：ORIG 0x08375bca 双 `movl $0,-0x20`（`int i=0;`
    // 与 `for(i=0;…)` 两条初始化都发射），for-init 声明形只发一条。
    int i = 0;
    for (i = 0; i < 5; ++i)
    {
        m_cubeIndexVecs[i].clear();
    }
    for (i = 0; i < 4; ++i)
    {
        m_legacyInfo[i].clear();
    }
    m_skillSelect = -1;
    m_skills.clear();
    // ORIG 0x08375bca 续：i=0..1 清零 (+0xcc,+0xd0) 两个 float——ORIG 是 2 元素
    // 循环 `mov i,%edx; mov this,%eax; lea 0x30(%edx),%ecx; mov $0,%edx;
    // mov %edx,0xc(%eax,%ecx,4)`（this+0xcc+4i，成员数组寻址）⇒ 用头内匿名 union
    // 的数组视角 m_skillProb[2] 循环访问（原两条独立赋值少 6 条且无槽复用）。
    for (i = 0; i <= 1; ++i)
    {
        m_skillProb[i] = 0;
    }
    // 随后三重循环 a(job)=0..10、b(grade)=0..3、c(slot)=0..4 清零
    // m_avatarDisjoint[a][b][c]（索引式 (c+b*5+a*20+0x34)，字节区 +0xd4..+0x443）。
    for (int a = 0; a < 11; ++a)
    {
        for (int b = 0; b < 4; ++b)
        {
            for (int c = 0; c < 5; ++c)
            {
                m_avatarDisjoint[a][b][c] = 0;
            }
        }
    }
    // ORIG 0x08375cf3..0x08375d42（SweepScriptDepsA 证据）：grade 0..3 × parts 0..9
    // 清零 m_avatarPartsDisjoint。寻址 `movl $0x0,0x4(%ecx,%eax,4)`、eax=0x110+g*10+p
    // ⇒ 基准 4+0x110*4 = 0x444，与头 [4][10]@+0x444 一致。
    for (int grade = 0; grade < 4; ++grade)
    {
        for (int parts = 0; parts < 10; ++parts)
        {
            m_avatarPartsDisjoint[grade][parts] = 0;
        }
    }
