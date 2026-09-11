// [b28d-51] 判词 N（ORIG 0x082fefce，N82 族）：switch 决策树 + 帧槽方向差；
// 语义/调用集齐，byte 层封顶。
// [F2d-C05② 甄别] 无「两具名局部」比较（比较对象为 enum/int 形参 + 指针解引用）⇒ F2 不适用；
//   A/B 实测删 `(int)type` 零效（show_func_diff 两态同形 orig=110 our=112 gap=-2 ndiff=18；
//   ORIG-only={jg×2} vs OURS-only={mov,jle×2,jmp} = 块排布镜像 N82/N93附）⇒ 回滚维持原形。
bool CClearCondition::ClearCondition(ENUM_DUNGEON_CLEAR_CONDITION_TYPE type, int idx)
{
    if ((int)type < 0 || (int)type > 4)
    {
        return false;
    }
    // ORIG 0x82fefce：+0x14 指向 vector<DungeonClearCondition>（外部条件表），
    // 与 m_vec 同步遍历；条目 m_0/m_4 匹配 type/idx 时，若 m_8 > 当前计数
    // 则计数自增并 m_count++，否则返回 false；结尾 (m_flag <= m_count)。
    std::vector<DungeonClearCondition>::iterator itd = m_pCondVec->begin();
    std::vector<DungeonClearCondition>::iterator endd = m_pCondVec->end();
    std::vector<int>::iterator itv = m_vec.begin();
    std::vector<int>::iterator endv = m_vec.end();
    while (itd != endd && itv != endv)
    {
        DungeonClearCondition* d = &*itd;
        int* v = &*itv;
        // [M4S2 · P211 类型档旋钮] ORIG 0x082ff06b 三条形
        //   `mov -0x10(%ebp),%eax; mov (%eax),%eax; cmp 0xc(%ebp),%eax`
        //   ⇒ 字段侧是**同型 enum 左值**（op1 形参留内存直比）。我方 m_0 声明为
        //   int ⇒ `int == enum` 需转换 ⇒ 反产 `mov (%eax),%edx; mov 0xc(%ebp),%eax;
        //   cmp %eax,%edx`（+1 条）。实测：删 (int)/换序均零效；probe
        //   /tmp/m4s2_probe/pr_cmp{,2}.cpp 中 f2（字段 enum）与 f7/f8（同址 enum
        //   视图）产 ORIG 逐条形，f7/f8 不引入额外槽。
        //   ⇒ 本处按 P211 以同址 enum 视图取值；**建议 .h 侧把
        //     DungeonClearCondition::m_0 声明为 ENUM_DUNGEON_CLEAR_CONDITION_TYPE**
        //     （属他泳道写域，仅上报）。
        if (*(ENUM_DUNGEON_CLEAR_CONDITION_TYPE*)d == type && d->m_4 == idx)
        {
            // [L6S2b · P222/P194 块序翻正] ORIG 为 `cmp %eax,%edx; jg <自增块>`
            // + 失败块 fall-through（0x082ff08d/08f/094：`jg +0xcb` → `mov $0x0,
            // %eax; jmp 出口`），即否定早返形；旧译 if/else 被 4.4 反产
            // `jle <失败>` + 自增 fall-through + 多一条 `jmp`（gap −2 的来源）。
            // 探针 /tmp/L6S2_probe/pr_pol.cpp：f2(`if (m8 <= *v) return 0; ++…`) 产
            // jg+失败块 fall-through（= ORIG），f1/f3 均产 jle 反形。
            if (d->m_8 <= *v)
            {
                return false;
            }
            ++(*v);
            ++m_count;
        }
        ++itd;
        ++itv;
    }
    // [M4S2 · P222 镜像] ORIG 0x082ff116..0x082ff121 形为
    //   `cmp %eax,%edx; jg +0x153; mov $0x1,%eax; jmp +0x158; +0x153: mov $0x0,%eax`
    //   ⇒ then 块 = return 1（fall-through）、else 块 = return 0（越过 jmp 落点）。
    //   旧译 `if (m_flag > m_count) return false; return true;` 反产
    //   `jle +0x155; mov $0x0; jmp; +0x155: mov $0x1`（镜像 + 多 1 条 jmp），
    //   改为 if/else 正条件形后与 ORIG 逐条一致。
    if (m_flag <= m_count)
    {
        return true;
    }
    else
    {
        return false;
    }
}