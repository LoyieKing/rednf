bool CDataManager::set_reward_sp()  // [P330 int→bool] 与 CDataManager.h 声明同步（init 调用点 ORIG xor $1 形，idx 3876）
{
    stSpPerLevelTable table;
    if (importSpPerLevelReferenceTable("Etc/spTable.etc", table))
    {
        return 0;
    }
    // 残差 N（N18a/N7 族）：两循环 4.4 寄存器指派与 store 序与 4.1 异形
    // （首循环 mov this 先于 mov i、常量拆 0x36a4+disp4；第二循环 ebx=this/
    // esi=i 且表值滞留 eax），int 循环变量探针反致 setle 破坏 setbe，不可翻。
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
