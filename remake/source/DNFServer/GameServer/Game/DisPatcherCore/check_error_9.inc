// ORIG 0x081c7f32 T _ZN24DisPatcher_SelectDungeon11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_SelectDungeon/check_error.md 汇编+反编译 C 全文转写）：
// state∉{5,0xd} 或 IsBusyDisjointStore → 0x7fffffff；m_field10∉[0,2] 或
// m_dungeonType<0 → -1；m_field11∉[0,1] → -2；m10>0 且 m11>0 → 0x7fffffff；
// m10!=2 时要求队长（getManager()==user）、party state==1、!(m11==1&&is_quick_party)；
// 两段重复 party 检查（ORIG 0x0d9 与 0x0a9 两块），全部通过 → 0。
unsigned int DisPatcher_SelectDungeon::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    if (user->get_state() != 5 && user->get_state() != 0xd)
    {
        return 0x7fffffff;
    }
    if (user->IsBusyDisjointStore())
    {
        return 0x7fffffff;
    }
    // ORIG 0x081...：msg→-0x14 spill 在 IsBusyDisjointStore 块 return 岛之后
    // （技巧13：具名局部落栈时机 = 声明位置）
    const MSG_SELECT_DUNGEON& req =
        *reinterpret_cast<const MSG_SELECT_DUNGEON*>(&msg);
    if ((char)(unsigned char)req.m_field10 < 0 ||
        (char)(unsigned char)req.m_field10 > 2 ||
        (char)(unsigned char)req.m_dungeonType < 0)
    {
        return -1;
    }
    if ((char)(unsigned char)req.m_field11 < 0 ||
        (char)(unsigned char)req.m_field11 > 1)
    {
        return -2;
    }
    if ((char)(unsigned char)req.m_field10 > 0 &&
        (char)(unsigned char)req.m_field11 > 0)
    {
        return 0x7fffffff;
    }
    if ((unsigned char)req.m_field10 == 2)
    {
        // 第一段（ORIG +0x0d3/+0x165 两处 m10==2 检查形状保留）
    }
    else
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party == 0)
        {
            return 0x7fffffff;
        }
        if (party->getManager() != user)
        {
            return 0x7fffffff;
        }
        if (party->get_state() != 1)
        {
            return 0x7fffffff;
        }
        if (req.m_field11 == 1 && party->is_quick_party())
        {
            return 0x7fffffff;
        }
    }
    if ((unsigned char)req.m_field10 == 2)
    {
        return 0;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return 0x7fffffff;
    }
    if (party->getManager() != user)
    {
        return 0x7fffffff;
    }
    if (party->get_state() != 1)
    {
        return 0x7fffffff;
    }
    return 0;
}
