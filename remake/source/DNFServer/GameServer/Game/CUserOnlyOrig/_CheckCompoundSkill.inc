// NOTE: CUser::_check_level_up 的权威定义已由 CUserCheckLevel.cpp 落地（ORIG 0x08662aea），
// 本 TU 不重复提供。
// ---------------------------------------------------------------------------
// 3) CUser::_CheckCompoundSkill(CStackableItem const*, bool)
//    报告：docs/class_func_reports/CUser/_CheckCompoundSkill.md。ORIG 0x08670f1e。
//    CStackableItem 侧：m_recipe174(+0x174 RecipeInfo) 内
//      m_recipe174.m_vec18(+0x18)=item+0x18c 需求技能列表；
//      m_recipe174.m_field24(+0x24)=item+0x198(合成技能标志 char)、
//      m_recipe174.m_field25(+0x25)=item+0x199(expert recipe 标志 char)。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08670f1e：p167=M0/E6:_ZNSt6vectorISt4pairIiiESaIS1_EED1Ev;_ZNSt6vectorISt4pairIiiESaIS1_EEC1ERKS3_，gap=43；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08670f1e：修法（本轮）——check_skill_list 按值参数
// 的实参改匿名右值临时（直构进参数槽），消 EXTRA `vector<pair<int,int>>::D1Ev`×4 +
// `C1ERKS_`×2 ⇒ p167 CLEAR（19=19），178→138 条。残余帧差 0x10（sub $0x50 vs $0x60，
// N115 outgoing 预留）+ setcc 物化 ⇒ N82/N115 封顶。
bool CUser::_CheckCompoundSkill(const CStackableItem* item, bool checkRecipe)
{
    // [H6 2026-09-11 负结论·部分推翻 · Q2P342ScanB 2026-09-11 复测]
    //   H6 原测：旋钮 A（`__PRETTY_FUNCTION__` 替静态 PRETTY 字面量）使 133→132（判为反劣）；
    //   旋钮 B（两处具名 `bool ok` 内联进 if）使 133→131 ⇒ B 仍为负结论，**勿试 B**。
    //   复测（当前码态）：删 `static const char* PRETTY`、直接用 `__PRETTY_FUNCTION__`
    //   ⇒ our 指令数 **132 → 132（不变）**、TU 四数 497/977/1/42/284 **逐项不变**，
    //   而符号集**净改善**：新增 ORIG 有的 `_ZZN5CUser19_CheckCompoundSkillEPK14CStackableItembE19__PRETTY_FUNCTION__`
    //   （**size 0x3d = ORIG 相等**），并消除 ORIG 没有的局部符号 `…E6PRETTY`（我方多出的 4B 指针）。
    //   ⇒ 本旋钮按「符号集」目标为正、按 gap/四数目标为中性 ⇒ 已落地。
    //   （剩余 gap +3 仍指向 `mov %eax,%ebx; xor $1` 寄存器保活与 0x0867104e 段。）

    // [G6Sub1 单旋钮·P225 推广/P133] ORIG 0x08670f2c..0x08670f59 为**两跳同落**形：
    // `test %al,%al; je <主体>` + `call isAffectedPremium; xor $0x1,%eax; test %al,%al;
    // je <主体>` + fall-through `mov $0x0,%eax; jmp 尾`（值无栈槽往返，P0）。
    // 旧 `if (A && B)` 合并短路被 4.4 物化成 bool 岛：多出 `mov $1,%eax; jmp;
    // mov $0,%eax; test %al,%al; je; mov $0,%eax; jmp`（4 条，即本件 gap 主项）。
    // 拆成嵌套 if（各条件独立早退、共享同一失败出口）即逐条复现 ORIG 双跳。
    if (item->m_recipe174.m_field24 != 0)
    {
        if (isAffectedPremium((ENUM_PREMIUM_TYPE)7) == false)
        {
            return false;
        }
    }

    if (item->m_recipe174.m_field25 != 0)
    {
        // [b28d-solo-CUserOnlyOrig P167] ORIG 无 C1ERKS_/D1Ev 差集 ⇒ 实参是匿名右值
        // 临时（直构进按值参数槽），非具名 list 局部（具名左值恒调拷贝构造 + 析构）。
        // [H6B 2026-09-11 保留·B 旋钮复测] 两处具名 `bool ok` 内联进 if（P0：ORIG
        // 0x08670fba `mov %eax,%ebx; xor $0x1,%ebx; test %bl,%bl` 全程无栈槽）。
        // 复测读数：our 133→132、ndiff **78→61**（严格改善）、gap +2→+3（条数口径略远）
        // ⇒ 按「四数不劣 + ndiff 严格改善」保留（旧 H6 记录只比较条数故曾回滚）。
        // 差集随之由 {ORIG test/movl/xor/jmp/jne vs OURS mov/movzbl×2} 收敛为
        // {ORIG cmpb/movl/xor/jmp/jne vs OURS mov/movzbl}。
        if (reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())->check_skill_list(
                get_charac_job(),
                std::vector<std::pair<int, int> >(item->m_recipe174.m_vec18), 2))
        {
            return expert_job::CExpertJob::IsLearnRecipe(this, item);  // ==1 → true
        }
        // [H6 2026-09-11 第三试负结论·已回滚] 加裸 `register`（欲复现 ORIG 独有段
        // `call getCurCharacNo; mov %eax,%ebx` + `mov %ebx,0x8(%esp)` 的跨 cMyTrace ctor
        // %ebx 保活）：our 133→**132**（gap +2→+3，更远）⇒ 回滚。**本件三旋钮全部否**
        // （__PRETTY_FUNCTION__ ✗ / bool ok 内联 ✗ / register 保活 ✗），残余 ORIG 独有段
        // 集中在 0x08670fba `mov %eax,%ebx; xor $1,%ebx`、0x0867103c-0x08671064 双出口、
        // trace 块 `mov %eax,%ebx` 保活 ⇒ 4.1 独有寄存器保活族（N 类），4.4 不可复现；
        // 语义已完整（p167 CLEAR）⇒ 本件维持封顶，勿再按"消槽/内联/保活"方向重试。
        unsigned int characNo = getCurCharacNo();
        // [FL5 2026-09-10 负结论·已回滚] P37/P227 链在本件 ndiff 152→152 中性 ⇒ 维持具名形。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x5c68, 0);
        trace("EXPERT_JOB_ERROR : check_skill_list() charac_no(%d)", characNo);
        return false;
    }

    if (!checkRecipe)
    {
        if (!reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())->check_skill_list(
                get_charac_job(),
                std::vector<std::pair<int, int> >(item->m_recipe174.m_vec18), 2))
            return false;
    }
    return true;
}