bool CCargo::IsOperatorable(int idx) const
{
    if (idx < 0 || m_capacity <= idx || idx > 0x97)
        return false;
    return true;
}