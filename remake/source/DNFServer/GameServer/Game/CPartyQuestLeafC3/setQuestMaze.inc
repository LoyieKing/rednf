// ============================================================================
// CBattle_Field::setQuestMaze —— ORIG T 0x0830e302（setQuestMaze.md）
// 汇编：sub $0x4；mov %al,-0x4(%ebp)；mov 0x8(%ebp),%eax；movzbl -0x4(%ebp),%edx；
//       mov %dl,0x110(%eax)；leave; ret。
// 即形参落栈后按布尔字节写入 this+0x110 == m_questMaze。
// 注：m_questMaze 已在 CBattle_Field.h 修正为 bool（1 字节，布局不变）——
// bool 成员直赋才能复现 ORIG 的「先取 this、后 movzbl 读回」指令次序
// （char 成员会先做 bool→char 扩展，与 ORIG 次序不符，经探针核实）。
// ============================================================================
void CBattle_Field::setQuestMaze(bool flag)
{
    m_questMaze = flag;
}
