// ORIG 0x0819114c：非静态成员，find 于 this+0x38（= m_useEmblemGradeRateMap）。
int AvatarRechargeServer::GetRateFromEmblemGrade(int grade)
{
    // [b28c-L9] ORIG 0x0819114c：find→-0x10、end→-0xc 两匿名槽 + 外链
    // `_Rb_tree_iterator::operator!=`（非 const 迭代器，P96）+ 正极性
    // `it != end` → `it->second`（P220/P47）。旧「const 迭代器 + ==end」形
    // 多生成转换构造与 operator== 外链（44 vs 34 insn）。
    std::map<int, int>::iterator it = m_useEmblemGradeRateMap.find(grade);
    if (it != m_useEmblemGradeRateMap.end())
        return it->second;
    return 0;
}