bool CUser::isDuplicationMessage(const std::string& msg)
{
    // ORIG 0x08688826：比较实参 = `lea 0x79664/0x79668(%eax),%edx` + `mov %eax,
    // 0x4(%esp)` ⇒ **m_str[i] == msg**（str 在左占 this 槽，原 msg==str 反序）；
    // ORIG -0x9 槽 = dup 结果槽；索引无槽——`movzbl 0x7966c; movsbl %al,%eax`
    // 原地（(char) 有符号 cast 内联，具名 idx 局部多占槽致 dup 漂到 -0xa）。
    bool dup = (m_str79664[0] == msg) || (m_str79664[1] == msg);
    m_str79664[(char)m_dupMsgSlotIdx] = msg;   // 写址 this+0x79664+idx*4（Ghidra (idx+0x1e598)*4 换算）
    m_dupMsgSlotIdx = m_dupMsgSlotIdx ^ 1;   // ORIG 0x8688?? `mov 0x8; movzbl 0x7966c` 重读成员
    return dup;
}