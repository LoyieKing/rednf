unsigned int DisPatcher_SetArea::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    // ORIG +0x07/+0x0d：param→-0x10（out 远槽）、msg→-0xc（req 近槽）——4.4 后声明
    // 占近槽（技21），out 先声明。三处 `out.m_8 = msg+0xd` 均 ORIG `movzbl mem,%edx`
    // 单步直落 %edx（技16/28 register asm 绑定破 4.4 累加器中转）。
    stSetAreaParam& out = static_cast<stSetAreaParam&>(param);
    const MSG_SET_USER_AREA& req =
        *reinterpret_cast<const MSG_SET_USER_AREA*>(&msg);
    if (user->get_state() != 3)
    {
        return 0x7fffffff;
    }
    if (user->IsBusyDisjointStore())
    {
        out.m_8 = req.m_0d;   // ORIG +0x44 `movzbl 0xd(%eax),%edx; ...; mov %dl,0x8`（同型 QI 直存，Main 已落 schar 字段）
        return 0xbf;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        out.m_8 = req.m_0d;   // ORIG +0x73
        return 0x3c;
    }
    if (user->IsCurCharacGhost())
    {
        out.m_8 = req.m_0d;   // ORIG +0x99
        return 0x13;
    }
    if (G_GameWorld()->IsForbiddenMove(user->getCurCharacName()))
    {
        if (G_GameWorld()->GetLeftTimeOfForbiddenMove(
                user->getCurCharacName()) != 0)
        {
            out.m_9 = req.m_0e;   // ORIG `movzbl 0xe,%edx; ...; mov %dl,0x9`
            return 0x44;
        }
        G_GameWorld()->EnableMove(user->getCurCharacName());
    }
    return 0;
}
