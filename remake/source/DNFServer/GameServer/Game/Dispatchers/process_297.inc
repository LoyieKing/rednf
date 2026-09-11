// ORIG 0x0825f4e8（报告 Dispatcher_ComboSkillExtensionQuickSlotReset/process.md）：
// err=check_error → param.m_4；>0 返 0；<0 LineFunc(0x384)；
// ==0 pullComboSkillExtensionQuickSlot(msg+0xd) → send_skill_info。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825f4e8 orig/ours=52/51 ndiff-blocks=10
//   帧差 sub $24 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    -0x10(%ebp),%edx
int Dispatcher_ComboSkillExtensionQuickSlotReset::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stComboSkillExtensionQuickSlotReset& out =
        static_cast<stComboSkillExtensionQuickSlotReset&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x384,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    // ORIG 0x0825f557: `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` —— req 引用槽 -0xc，
    //   声明在 err==0 分支内（原声明在函数顶部，多占 -0x14 槽并使帧 +4）。
    MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET& req =
        reinterpret_cast<MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET&>(msg);
    // [G5·P52 负结论 2026-09-11] getCurCharacSkillW() 返回 char（非 SkillSlot*）⇒ 链式
    //   形不可写（编译错误）；ORIG 0x0825f557 形保留为具名 skillSlot + cast。
    SkillSlot* skillSlot = (SkillSlot*)user->getCurCharacSkillW();
    skillSlot->pullComboSkillExtensionQuickSlot((ENUM_SKILL_TREE_KIND)(char)req.m_0d);
    user->send_skill_info();
    return 0;
}