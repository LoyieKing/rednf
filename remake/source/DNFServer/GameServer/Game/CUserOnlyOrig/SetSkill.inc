// ---------------------------------------------------------------------------
// 10. CUser::SetSkill @0x0864db20
//     SIG_LOAD_SKILL 包内布局视图(+0x8 sp1 / +0xc bufA / +0x198 尺寸常量 /
//     +0x1a4 sp2 / +0x1a8 bufB / +0x340 tail1 / +0x344 tail2 / +0x348 cmd 200B)。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0864db20：p167=CLEAR，gap=-6；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864db20：N82 封顶——push edi/esi callee-saved 差
// + 帧（p167 CLEAR）。
bool CUser::SetSkill(SIG_LOAD_SKILL* skill, SIG_COMBO_SKILL* combo)
{
    // [FL5 2026-09-10 P290 试行] ORIG 0x0864dc19-0x0864dc1f 把 sp1/sp2/tail1 升格
    // 到 %ebx/%esi/%edi 跨 receiver call 保活；4.4 对 `int` 局部改走槽重读。
    // 改 unsigned（值域非负，语义等价；实参仍按隐式转换传 int 形参）。
    unsigned int sp1 = skill->m_remainSpTree1;              // +0x8
    // [CoBatchB 2026-09-11 负结论·已回滚] `char* const bufA/bufB`（欲复现 ORIG
    // 0x0864dc19-0x0864dc1f 的 %esi/%ebx 跨 call 保活）实测 **123→123 中性** ⇒ 回滚。
    char* bufA  = reinterpret_cast<char*>(&skill->m_skillSlotTree1[0]);   // +0xc
    int   size  = 0x198;   // ORIG 0x0864db3b movl $0x198,-0x2c（4.1 sizeof*SIZE 常量物化槽）
    unsigned int sp2 = skill->m_remainSpTree2;              // +0x1a4
    char* bufB  = reinterpret_cast<char*>(&skill->m_skillSlotTree2[0]);   // +0x1a8
    unsigned int tail1 = skill->m_remainSfpTree1;           // +0x340
    unsigned int tail2 = skill->m_remainSfpTree2;           // +0x344（ORIG 实参第 4 = sfp2）

    // ORIG 0x0864db71：恒假防御检查（4.1 保留代码）——`cmpl $0x198,-0x2c; je`，
    // 不匹配则 getCurCharacName 结果直落 0x14(%esp) 走 LogManager::logFormat 后返 0
    if (size != 0x198)
    {
        LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__, 0x13a6,
            "User %s - size != sizeof( _Mastered_skill ) * _Mastered_skill::SIZE_OF_SKILLSLOT",
            getCurCharacName());
        return false;
    }
    if (getCurCharacR() == 0)
    {
        // [FL5 2026-09-10 P37/P227] 全表达式临时链
        cMyTrace(__PRETTY_FUNCTION__, 0x13ad, 5)("CUser::SetSkill User  m_selected is null");
        return false;
    }

    // asm 实参序:(sp1, sp2, tail1, tail2=[0x344])——ORIG 第 4 参为 0x344 字段
    // [FL5 2026-09-10 P52/P200 重试] ORIG 0x0864dc19-0x0864dc1f 先把 sp1/sp2/tail1
    // 装入 %ebx/%esi/%edi（callee-saved，跨 getCurCharacSkillW() receiver call 保活），
    // 0x0864dcec 才 pop；DualCUOO 期「两语句 + 具名 slot」形把索引值压回栈槽。
    // 按 P52 用**单条链式表达式**（receiver 调用内联进语句）复现 callee-saved 独占形；
    // 若读数反劣（旧批测得 gap 6→10）则回退为具名形。
    ((SkillSlot*)getCurCharacSkillW())->set_remain_sp(sp1, sp2, tail1, tail2);

    // [A06 2026-09-10 轴A] ORIG 0x0864dcd8 `mov -0x2c(%ebp),%edx; mov %edx,0xc(%esp)`
    // ⇒ 第 3 实参传**具名局部 size**（4.1 不折立即数）；写死 0x198 少 1 条。
    // [A06 2026-09-10 负结论] `register char* bufA/bufB`（P231 提示形）实测反劣
    // （122→124 条、diff 62→92）⇒ 保持普通局部。残余 = ORIG 把 -0x24/-0x30 升格
    // %esi/%ebx 跨 receiver call（P290/P52 族），4.4 恒逐实参重读，封 N7。
    ((SkillSlot*)getCurCharacSkillW())->set_skill_slot(bufA, bufB, size);

    setSkillCommandData(reinterpret_cast<char*>(&skill->m_commands[0]));  // +0x348

    // 技巧37：call 结果免槽直落 this 实参（ORIG 0x0864dc87-0x0864dc9d 常参
    // `movl $0x0,0x4(%esp)` 不破 %eax，无 -0xc spill）
    ((SkillSlot*)getCurCharacSkillW())->SetLoadStrikerSkill(false);

    if (get_charac_job() == 9)
    {
        ((SkillSlot*)getCurCharacSkillW())->loadComboSkill(combo);
        ((SkillSlot*)getCurCharacSkillW())->verifyComboSkill();   // ORIG 0x0864dcd1 免槽
    }
    return true;
}