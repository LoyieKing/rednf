int Dispatcher_ComboSkillExtensionQuickSlotReset::check_error(CUser* user, MSG_BASE& msg)
{
    const MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET& req =
        reinterpret_cast<const MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET&>(msg);
    // b28d-D0: 同 ComboSkill::check_error，ORIG 0x0825f5ad 段三条件独立物化（P133）。
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() < 3)
    {
        return -1;
    }
    if (({ user->get_charac_job() != 9; }))
    {
        return -1;
    }
    // b28d-solo-Dispatchers: 同 ComboSkill::check_error —— ORIG 0x0825f5e5 两条件
    //   各自 `je <mov $0,%eax>` 共享岛（负极性 && 链，P214①/P45）。
    if (req.m_0d != 0 && req.m_0d != 1)
    {
        return 0x13;
    }
    return 0;
