// ORIG W 0x84ed4a4：movb %?,+0x04（level 截断为 unsigned char）。
void CTimeGate::SetLevel(int level)
{
    m_openState = (unsigned char)level;
}