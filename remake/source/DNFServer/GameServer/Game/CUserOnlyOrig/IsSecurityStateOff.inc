// ORIG T 0x0827b402（外联定义发射 T 符号）。
bool cSecuUserAction::IsSecurityStateOff()
{
    if (m_state == 1)
        return true;
    return false;
}