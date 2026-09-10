int Dispatcher_EnchantByBead::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d24b8：param+4 = check_error(...)；>0 返 0；<0 LineFunc(0x2f61,err,0)；
    // ==0 调 GlobalData::s_ExpertJobMgr->OnEnchantByBead(user, msg+0xd(char),
    // msg+0xe(short), msg+0x10(char), msg+0x12(short))，非 0 结果写回 param+4，恒返 0。
    stEnchantByBeadParam& p = static_cast<stEnchantByBeadParam&>(param);
    p.m_4 = check_error(user, msg, param);
    if (p.m_4 > 0)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        return LineFunc(0x2f61,
                        __PRETTY_FUNCTION__,
                        p.m_4, 0);
    }
    const MSG_ENCHANT_BY_BEAD* m =
        reinterpret_cast<const MSG_ENCHANT_BY_BEAD*>(&msg);
    int result = 0;
    // asm：msg+0xd movsbl、msg+0xe movswl、msg+0x10 movsbl、msg+0x12 movswl
    result = GlobalData::s_ExpertJobMgr->OnEnchantByBead(
        user,
        static_cast<int>(m->m_field0d),
        static_cast<int>(m->m_field0e),
        static_cast<int>(m->m_field10),
        static_cast<int>(m->m_field12));
    if (result != 0)
    {
        p.m_4 = result;
    }
    return 0;
