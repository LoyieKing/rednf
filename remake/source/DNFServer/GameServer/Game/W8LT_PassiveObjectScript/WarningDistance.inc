// ---- WarningDistance（ORIG C1/C1Eiii T 0x08a3aeca/0x08a3aeec）----
// C1：三字段 movl $0 直写，函数体逐句赋值（每字段重读 this 0x8(%ebp)）；
// C1Eiii：参数 edx 中转 store，同函数体赋值形态。
WarningDistance::WarningDistance()
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
}