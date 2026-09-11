// ============================================================================
// stNPCCommonData_t::clear —— ORIG W 0x089fae9e（objdump 全文核实）：
// 仅清标量——循环 i=0..9 写 favorExp[i]（movw $0,0x2(%eax,%edx,2)，cmpl $0x9/setle），
// 再按 +0x00→+0x34→+0x30→+0x32 顺序清四个标量；无任何容器 clear。
// ============================================================================
void stNPCCommonData_t::clear()
{
    for (int i = 0; i <= 9; ++i)
    {
        m_favorExp[i] = 0;
    }
    m_maxFavorExp = 0;
    m_maxFavorLevelNpc = 0;
    m_favorSystemBeginQuest = 0;
    m_favorSystemBeginLevel = 0;
}