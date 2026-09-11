// ============================================================================
// CDataManager::set_pvp_grade —— ORIG 0x08360aa2
// 加载 PvP 等级参考表，填充 GlobalData::s_ref_pvp_grade（RefPvpGrade）。
// 调用 importPvPReferenceTable（本 TU 实现，ORIG 0x890857b）。
// ============================================================================
// 残余 DIFF 归因（o=93 n=90）：ORIG `mov &s_ref_pvp_grade,%eax` 为链接后
// 绝对地址（.o 显示 $0x0 reloc）；循环体 SetPvpGrade 的 grade 指针 ORIG 经
// 单次装载复用（0x08360b0x 区），4.4.7 每次重装载全局（P48 反向），序列
// 等价但行序差；table ctor 槽 -0x50 vs -0x54（4.4 对 4B 对齐选择差异，P112）。
// [b28d-solo-CDataManager AE] 修复：prevExp/lastGrade 声明序按 ORIG 入口清零序
// [b28d-orig-bool] 返回类型 int→bool：init 调用点 ORIG `call set_pvp_grade;
// xor $0x1,%eax; test %al`（bool 值域 0/1 的取反形），我方 int 声明发
// `test %eax,%eax; sete %al`。mangled 不含返回类型，符号不变。
// （-0x14 先），循环体序改为 ++lastGrade → data.curr=prevExp → data.next →
// prevExp → SetPvpGrade（三次独立 op[]，P48）。
// [SweepCDataManager-2] 本函数局部 `stPvpRefTable_t table;` 发射 W 符号
// _ZNSt12_Vector_baseIN15stPvpRefTable_t15stPvPGradeExp_tESaIS1_EED1Ev
// （ORIG 0x083b1e6a，40 条 vs 我方 25 条，gap 15）：ORIG 4.1 布 EH landing
// pad（`_Unwind_Resume` + `_Vector_implD1Ev` 双份，0x083b1ea4..0x083b1ebf），
// 我方 4.4.7 无——根因是 4.4 对类内可见定义的 `_M_deallocate`
// （→`new_allocator::deallocate`→`::operator delete` throw()）推断 nothrow
// ⇒ 不布 cleanup，4.1 不推断。`bits/stl_vector.h` 禁改、allocator 换型会改
// mangled（`SaIS1_E` 编码）⇒ 无源码解，N 封顶；正文 19 条逐条一致。
bool CDataManager::set_pvp_grade()
{
    // ORIG 0x08360aa2：Clear 走 `mov &s_ref_pvp_grade,%eax; call` 两段直发
    // （无指针命名槽）；lastGrade(-0x14)/prevExp(-0x10) 在 table ctor 前
    // 初始化；table 由 C1 0x0837f45e 自带清零（ORIG 无 m_count 手写 store）。
    // [b28d-lane16 真语义缺口] ORIG 0x08360aaa-0x08360ab2 先调 RefPvpGrade::Clear()
    // （p167 差集 MISSING -1 _ZN11RefPvpGrade5ClearEv）；原重建缺失该清表调用。
    GlobalData::s_ref_pvp_grade->Clear();
    // [b28d-solo-CDataManager] ORIG 入口清零序 -0x14(prevExp) → -0x10(lastGrade)
    // ⇒ 声明序 prevExp 先（4.1/4.4 同向：先声明占深槽）。循环体序按 ORIG
    // 0x08360aff：++lastGrade → data.curr=prevExp → data.next → prevExp →
    // SetPvpGrade（三次独立 op[]，P48）。
    int prevExp = 0;
    int lastGrade = 0;
    stPvpRefTable_t table;
    if (importPvPReferenceTable("Etc/pvp_ref.etc", table) != 0)
        return false;
    for (unsigned int i = 1; i < (unsigned int)table.m_count; ++i)
    {
        RefPvpGrade::RefPvpGradeData data;
        ++lastGrade;
        data.curr = prevExp;
        data.next = table.m_grades[i].b;
        prevExp = table.m_grades[i].b;
        GlobalData::s_ref_pvp_grade->SetPvpGrade(table.m_grades[i].a, data);
    }
    GlobalData::s_ref_pvp_grade->set(table);
    GlobalData::s_ref_pvp_grade->SetLastGrade(lastGrade);
    return true;
}