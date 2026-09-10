unsigned int CharacData::upgreadGage()   // b28c-L5 SIGFIX：ORIG 三出口写 %eax 错误码；
                                         // 消费点 store 直存形定案 unsigned（见 .h 判词）
{
    ConfigParameter* cfg = &G_CDataManager()->m_altarConfig;  // CDataManager+0x6364（upgreadGage.md `add $0x6364`）
    if (m_gageLevel == 3)
    {
        return 2;   // ORIG 0x0812d194 `mov $0x2,%eax; jmp 尾`：满级错误码 2
    }
    // b28d-solo-ProcStage：ORIG 0x812d1b3 `fildl aux[level]` → `flds 0x4` → fmulp →
    //   `fistpl`（纯截断，无 +0.5f）→ `imul *m_strPtr` 覆盖同槽；base/ratio 均无具名槽。
    int cost = (int)((float)cfg->m_gageUpAux[static_cast<unsigned int>(m_gageLevel)] *
                     cfg->m_4);
    cost = *reinterpret_cast<int*>(cfg->m_strPtr) * cost;
    if (m_gage < cost)
    {
        return 3;   // ORIG 0x0812d1f0 `mov $0x3,%eax; jmp 尾`：gage 不足
    }
    m_gage -= cost;
    // ORIG 0x812d20b 决策树 `cmp $1; je L1; cmp $2; je L2; test; jne default` +
    //   叶块物理序 0,1,2 ⇒ 原码是 `switch (m_gageLevel)`（P251/P286；if-else 链
    //   在 4.4 恒反产 jne 内联块序）。
    switch (m_gageLevel)
    {
    case 0:
        m_gageLevel = 1;
        break;
    case 1:
        m_gageLevel = 2;
        break;
    case 2:
        m_gageLevel = 3;
        break;
    }
    return 0;   // ORIG 0x0812d23e `mov $0x0,%eax`（成功统一出口，非各臂就地写）
