bool CSkill::can_learn(int level, int firstGrowType, int secondGrowType,
                       int targetLevel) const
{
    // [M5C-SkillSlang 2026-09-11 · P339 + 三元形变，ORIG 0x0835039e] 旧判词只覆盖 ① 的
    // setcc 残余（P194/N-88），② ③ 为源码形差，本波改：
    // ① B 段 ORIG 0x0835040d `cmp %edx,%eax; jge 0x08350425`，then 再发 call
    //    （0x0835041e）、两臂各自落 eax 后 **汇合点** 0x0835043a `mov %eax,-0x14` 才写槽
    //    ⇒ ORIG 是三元赋值；我方 if/else 双写槽（+1）且条件被 4.4 物化 setl+test+je（+2）。
    // ② else 段 ORIG 0x083504a0 `mov -0xc(%ebp),%edx; cmp %edx,%eax; jle 0x083504a9;
    //    mov %edx,%eax`（**不重算** v+1，两值择一）⇒ 三元 min 形；我方 if/else 反极性
    //    `jg` + 重算整段（+5）。
    // ③ ORIG 三个 int 局部槽 -0x14(maxLevel)/-0x10(needLevel)/-0xc(secondGrowMax)
    //    （B 段写 -0x14、else 段临时写 -0xc、尾部写 -0x10）⇒ 头部全声明 + 该声明序。
    int maxLevel;
    int needLevel;
    int secondGrowMax;
    if (secondGrowType < 1)
    {
        if (targetLevel < 1 || get_type_max_level(firstGrowType) < targetLevel)
        {
            return false;
        }
        maxLevel = (get_type_max_level(firstGrowType) <
                    (level - m_levelLimit) + 1)
                       ? get_type_max_level(firstGrowType)
                       : (level - m_levelLimit) + 1;
    }
    else
    {
        secondGrowMax = get_second_growtype_max_level(firstGrowType,
                                                      secondGrowType);
        if (secondGrowMax == 0)
        {
            secondGrowMax = get_type_max_level(firstGrowType);
        }
        if (targetLevel < 1 || secondGrowMax < targetLevel)
        {
            return false;
        }
        maxLevel = ((level - m_levelLimit) + 1 <= secondGrowMax)
                       ? (level - m_levelLimit) + 1
                       : secondGrowMax;
    }
    if (maxLevel < targetLevel)
    {
        return false;
    }
    needLevel = m_levelLimit + m_fieldC8 * (targetLevel - 1);
    return needLevel <= level;
}