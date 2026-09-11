// ORIG 0x081e3c08 T _ZN33Dispatcher_RequestCharacSkillInfo11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/Dispatcher_RequestCharacSkillInfo/check_error.md 汇编+反编译 C 全文转写）
// user+0x796e8 = m_characList（vector<_Charac_info>）；槽 char 截断按 movsbl；
// _Charac_info+0x27 = m_level（short）；G_CDataManager()+0x629c = m_characLink.m_field0。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e3c08 orig/ours=75/68 ndiff-blocks=9
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,-0xc(%ebp)
int Dispatcher_RequestCharacSkillInfo::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    // ORIG 0x081e3c1f `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` —— msg 引用绑定在
    //   user==0 检查**之后**、get_state 检查之前（P28 声明序即槽序）= -0x10。
    MSG_REQUEST_CHARAC_SKILL_INFO& mref = reinterpret_cast<MSG_REQUEST_CHARAC_SKILL_INFO&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    if (user->get_state() <= 2)
    {
        return -1;
    }
    // [G1Frest 2026-09-10] ORIG 0x081e5xx：m_0d 在三个使用点各自重读
    //   （`mov -0x10(%ebp),%eax; movzbl 0xd(%eax),%eax; movsbl %al,%eax` ×3），
    //   level 亦无独立槽（`movswl %ax,%ebx` 直落 callee-saved）
    //   ⇒ 旧码 slot/level 两个具名局部（-0x10/-0xa）为发明，且把 mref 槽挤到 -0x14。
    if ((int)static_cast<signed char>(mref.m_0d) < (int)user->m_characList.size())
    {
        if (user->m_characList[(int)static_cast<signed char>(mref.m_0d)].m_characNo == 0)
        {
            return 0x15;
        }
        if (user->m_characList[(int)static_cast<signed char>(mref.m_0d)].m_level <
            G_CDataManager()->m_characLink.m_field0)
        {
            return 0xe;
        }
        return 0;
    }
    return 0x39;
}