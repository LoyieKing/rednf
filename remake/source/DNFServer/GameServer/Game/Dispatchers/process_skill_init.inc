// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e5bdc orig/ours=49/56 ndiff-blocks=8
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%eax | mov    %eax,-0xc(%ebp)
void Dispatcher_SkillInit::process_skill_init(CUser* user, ParamBase& param)
{
    // ORIG 0x081e5bdc
    if (user == 0)
    {
        return;
    }
    stSkillInit& p = static_cast<stSkillInit&>(param);
    int kind = (int)(char)p.m_8;                    // ORIG -0xc（有槽往返）
    // ORIG 0x081e5bdc：四次 `call getCurCharacSkillW` 的结果**从不落槽**——
    // `mov %eax,(%esp)` 直传 receiver（P0：原码无 slot 局部，每步现取现用）
    // ⇒ 删 slot 变量，调用点写成 receiver 内联表达式。
    ((SkillSlot*)user->getCurCharacSkillW())
        ->clear_all_skills((ENUM_SKILL_TREE_KIND)kind);
    ((SkillSlot*)user->getCurCharacSkillW())->set_parent(user);
    ((SkillSlot*)user->getCurCharacSkillW())
        ->InitSkill(false, (ENUM_SKILL_TREE_KIND)kind);
    ((SkillSlot*)user->getCurCharacSkillW())
        ->resetComboSkill((ENUM_SKILL_TREE_KIND)kind);
    user->setComboSkillSet(true);
}