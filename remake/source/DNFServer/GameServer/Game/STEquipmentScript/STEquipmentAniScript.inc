STEquipmentAniScript::STEquipmentAniScript()
{
    // [SweepSTEquip 2026-09-09 旧判词 N4/P235 作废] C1 0x0898ca36 375 条：
    // ORIG 对 +0x10/+0x2c/+0x3c 三个连续 string 区发**倒计数构造循环**
    // （0x0898ca42 `lea 0x10(%eax),%esi; mov $0x1,%ebx; … sub $1,%ebx;
    // cmp $-1,%ebx; jne`，P151）。正解不是 union 视图而是**真数组成员**：
    // .h 已把 +0x10/+0x14→m_arr10[2]、+0x2c..0x38→m_arr2c[4]、
    // +0x3c..0x48→m_arr3c[4]（sizeof 0x7c 与全部偏移不变，隐式拷贝/赋值/
    // 析构仍由编译器合成）。修后 375=375 逐条同形；同族 C1ERKS_ 433=433、
    // D1Ev 332=332、aSERKS_ 180=180（原 192）、clear 118=118 全部归零。
    clear();
