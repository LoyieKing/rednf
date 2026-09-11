// ORIG T 0x08a9eb3a：遍历 m_macroCommands(+0x94)，短路双条件
// （first==m_field00 且 command==m_field04，各一次 operator[]），
// 命中返回 m_type(+8)；未命中 0。循环变量 unsigned（Ghidra uint；
// ORIG 循环头 seta 无符号判续）。比较左侧 first 先求值（ORIG 循环体
// 首条 movswl first→ebx 跨 op[] 调用缓存）。
int StrikerScript::getAddCommand(short firstGrowType, int command)
{
    for (unsigned int i = 0; i < m_macroCommands.size(); ++i)
    {
        if (firstGrowType == m_macroCommands[i].m_field00 &&
            command == m_macroCommands[i].m_field04)
        {
            return m_macroCommands[i].m_type;
        }
    }
    return 0;
}