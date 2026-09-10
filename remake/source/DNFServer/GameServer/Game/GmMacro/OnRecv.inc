void CMacro::OnRecv(int type, int sub, char* data, int len)     // 0x084a8870
{
    if (m_state == 1)
    {
        Recording(type, sub, data, len);
    }
