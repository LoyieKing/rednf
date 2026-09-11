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
    // [P6S3 round6 · 路由杠杆实测（证据留给 Main）] ① 同源码整 TU 叠
    //  `-march=i486 -mtune=generic` 后：本件 78/77 DIFF → **NEAR 78/79**（cmov 归零，
    //  出 ORIG 形 `add $0x1,%eax; jle`），TU 四数 near 0→2 / diff 2→0（零回退）；
    //  残差仅剩帧 0x38 vs 0x20 + 槽 -0x10/-0xc/-0x1c vs -0x8/-0x4/-0x14（N82 槽分区）。
    //  ② **per-function 不用改路由也能落地**：探针 /tmp/p6/sc/p2.cpp 实证
    //  `__attribute__((target("arch=i486")))` 挂在**声明**上（.h 侧）即可让本函数出
    //  ORIG 形（`addl $1,%eax; jle`，无 cmov）；挂在定义上被 4.4.7 C++ 拒
    //  （「attributes are not allowed on a function-definition」）；`#pragma GCC
    //  push_options/target("arch=i486")` 在 4.4.7 对本函数**无效**（探针 p4.cpp 仍 cmov）。
    //  ⇒ 若 .h 侧愿加该声明属性（P7HeaderAxis/P6AuditOldN 域），本件与 can_learn 可
    //  直接取 i486 形；否则需整 TU 路由。两者都属头/路由轴，本泳道不再动。
    maxLevel = ((level - m_levelLimit) + 1 < maxLevel)
                   ? (level - m_levelLimit) + 1
                   : maxLevel;
    int q = (level - m_levelLimit) / m_fieldC8 + 1;
    if ((unsigned)(q < maxLevel ? q : maxLevel) >> 31)
    {
        return 0;
    }
    return q < maxLevel ? q : maxLevel;
}