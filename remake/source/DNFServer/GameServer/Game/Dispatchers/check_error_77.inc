// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825f594 orig/ours=41/40 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: je     0825f602 <+0x6e> | jmp    0825f607 <+0x73> | mov    $0x0,%eax
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
}