int CSkill::getMaxSkillLevel(int level, int firstGrowType,
                             int secondGrowType) const
{
    // b28d-37 [真语义 bug 修复，ORIG 0x08350700]：maxLevel 须取 min((level-m_levelLimit)+1, maxLevel)（旧实现恒无操作致上限偏大）；残 cmov（P230① -march=i486 可消，待路由）+ N82 帧。
    int maxLevel;
    if (secondGrowType < 1)
    {
        maxLevel = get_type_max_level(firstGrowType);
    }
    else
    {
        maxLevel = get_second_growtype_max_level(firstGrowType,
                                                 secondGrowType);
        if (maxLevel == 0)
        {
            maxLevel = get_type_max_level(firstGrowType);
        }
    }
    if (maxLevel == 0)
    {
        return -1;
    }
    // b28d-37 [真语义 bug 修正，ORIG 0x08350785]：ORIG 为
    // `eax=v+1; edx=maxLevel; cmp %edx,%eax; jle L; mov %edx,%eax; L: mov %eax,-0x10`
    // ⇒ maxLevel = min((level-m_levelLimit)+1, maxLevel)（v+1<=maxLevel 时上限缩小）。
    // 旧实现 `if (v+1 > keep) maxLevel = keep;` 恒为无操作（keep==maxLevel），
    // 导致返回上限偏大（v+1<=maxLevel 时未缩小）。改三元 min 形。
    // [N 封顶] 残差：4.4 三元产 `cmovle`（ORIG 为 jle 分支），P230① -march=i486
    // 可消 cmov 族但本波不改路由 ⇒ 待 Main 统一路由变更后复测；另帧 0x20 vs
    // ORIG 0x38 属 N82 槽分区。78/77。
    maxLevel = ((level - m_levelLimit) + 1 < maxLevel)
                   ? (level - m_levelLimit) + 1
                   : maxLevel;
    int q = (level - m_levelLimit) / m_fieldC8 + 1;
    if ((unsigned)(q < maxLevel ? q : maxLevel) >> 31)
    {
        return 0;
    }
    return q < maxLevel ? q : maxLevel;
