int CDisjointer::get_disjoint_result(CItem* item, Inven_Item* inventItem,
                                     DisJointResult& result, CUser* user)
{
    bool mine = isMine(user);
    // [b28d-solo-ExpertJobLink4] P23/P134：ORIG 实参右→左求值——最后实参
    // m_machine.get_user() 先算入 %ebx，mine 读槽入 %esi，grade 表达式随后入
    // %eax（无 grade 具名槽）；具名 grade 局部会把 get_user 推迟到实参点。
    return get_disjoint_result_(item, inventItem,
                                get_disjoint_machine_grade(), result, mine,
                                m_machine.get_user());
