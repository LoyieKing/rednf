// ORIG 0x85d3e1e：+0x14 置空、m_name.clear()、+0x1c 清零、+0x20 清零。
void CDisjointMachine::reset()
{
    m_user = 0;
    m_name.clear();
    m_field1c = 0;
    m_field20 = 0;
}