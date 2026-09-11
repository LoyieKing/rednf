// ===================== set_reward_sp =====================
// [b28d-solo-CDataManagerMethods] DIFF 封 N76/N94：P167 CLEAR（6/6）。
// ORIG 两循环均为「先求下标/RHS、后装 this」（第二循环值经 eax→edx 中转）；
// 4.4 成员函数恒 LHS-first（探针 f_int/f_uint/h_int/h_zero：类型异型化不翻），
// 第一循环仅 mov 装载序差、第二循环求值序差 ⇒ 语义等价封顶。
// ORIG 0x08360bde：导入 Etc/spTable.etc 至局部 stSpPerLevelTable；
// 导入失败返回 0。成功时将 +0xDA90 表清零后自 index1 起回填表值
// （index0 恒为 0），返回 1。逐次重读 this（与 O0 成员访问一致）。
bool CDataManager::set_reward_sp()  // [P330 int→bool] 与 CDataManager.h 声明同步（init 调用点 ORIG xor $1 形，idx 3876）
{
    stSpPerLevelTable table;
    if (importSpPerLevelReferenceTable("Etc/spTable.etc", table))
    {
        return 0;
    }
    //   残余 N（N18a/N7 族）：两循环 4.4 寄存器指派与 store 序与 4.1 异形
    //   （首循环 mov this 先于 mov i、常量拆 0x36a4+disp4；第二循环 ebx=this/
    //   esi=i 且表值滞留 eax），int 循环变量探针反致 setle 破坏 setbe，不可翻。
    //   [l5f 2026-09-11 · 探针复核] 根因定位（/tmp/probe_sr2.cpp）：v5（this 直接
    //   成员、无 cast）逐条复现 ORIG 形（索引→%ebx 保活 + call 后重装 this + 值经
    //   %edx 中转）；而 v6/v8/v13/v19/v20/v21（CDataManager→View 指针/引用 cast 六
    //   形）**全部**产 this→%ebx 提升形（判词旧记录「取址/int cast 无效」的正因）。
    //   ⇒ 唯一解 = 让 +8 处数组成为 CDataManager 自身具名成员（.h 项，按纪律只报告：
    //   CDM+8 若可安全具名 unsigned int[0x3702]，本 TU 视图 CDataManagerRewardSpView
    //   即可删除）；本 TU 内源形已穷举 7 形 ⇒ 封 N7/N18a。
    for (int i = 0; i < 0x60; ++i)
    {
        ((CDataManagerRewardSpView*)this)->m_sp8[0x36a2 + i] = 0;
    }
    for (unsigned long i = 1; i < 0x60; ++i)
    {
        // [round5-A26 判词·N7/N18a 复核] ORIG 0x08360c4c 形 = i 值跨 operator[]
        // call 保活入 %ebx + RHS-first + `mov %eax,%edx` 中转 + this 在 store 前
        // 重载；4.4 恒 this→%ebx、索引 `lea 0x36a2(%eax),%esi` 预置、无中转。
        // 本轮实测两形：①取址 `(unsigned short&)`/`(int)` cast 调用点旋钮无效；
        // ②同址 int 视角 union（m_sp8i）+ 去 cast 使两端异型 —— 产物与 unsigned
        // 原形**逐条全同**（66 条，无中转）⇒ 4.4 视同宽整型转换为无操作，
        // 属寄存器指派族（N7/N18a）封顶，判词同 b28d bm4。
        ((CDataManagerRewardSpView*)this)->m_sp8[0x36a2 + i] = table[i];
    }
    return 1;
}