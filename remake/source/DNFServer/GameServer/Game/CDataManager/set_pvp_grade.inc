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
