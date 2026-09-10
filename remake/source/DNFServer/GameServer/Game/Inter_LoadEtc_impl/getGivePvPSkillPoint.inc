int PvPSkillTreeParameterScript::getGivePvPSkillPoint(int job, int firstGrow,
                                                      int secondGrow,
                                                      int grade) const
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 75/75：clamp 写回形参（P77 扩展）+
    // 内层键 job*5+firstGrow（真语义 bug 修正）+ 具名 key2 复现槽序（P236 反向）。
    // [b28d-solo-Inter_LoadEtc_impl] P77 扩展：ORIG 0x08a5dc74 clamp 直接写回形参
    // （0x18(%ebp) 原地改写，无 key 局部）；内层键 = job*5 + firstGrow——三重证据：
    // ①汇编 `mov 0xc(%ebp);shl2;add;add 0x10(%ebp)`；②Ghidra `param_2*5+param_3`；
    // ③调用点符号 080fdf20=get_charac_job / 08110c94=getCurCharFirstGrowType /
    // 0822f23c=getCurCharSecondGrowType / 0819ee4a=get_pvp_grade。旧
    // firstGrow*5+secondGrow 为真语义 bug（secondGrow 在 ORIG 本函数内未被消费）。
    (void)secondGrow;
    if (m_field4c < grade)
    {
        grade = m_field4c;
    }
    std::map<int, std::map<int, int> >::const_iterator it = m_map18.find(grade);
    if (it != m_map18.end())
    {
        int key2 = job * 5 + firstGrow;
        std::map<int, int>::const_iterator it2 = it->second.find(key2);
        if (it2 != it->second.end())
        {
            return it2->second;
        }
    }
    return 0;
