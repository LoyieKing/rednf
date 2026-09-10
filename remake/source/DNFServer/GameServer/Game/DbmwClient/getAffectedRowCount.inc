long long DbmwClient::getAffectedRowCount()
{
    // ORIG T @0x08120dac：mov 0x30(%eax),%eax; mov %eax,%edx; sar $0x1f,%edx; ret
    // → 读 +0x30（m_affectedRow）符号扩展返 64 位。头声明 long long（非 const）。
    return static_cast<long long>(static_cast<signed int>(m_affectedRow));
