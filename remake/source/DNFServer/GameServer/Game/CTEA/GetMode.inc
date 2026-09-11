// ---- ORIG _ZN7IMethod7GetModeEPj T @0x080b4446（0x2f）：同形，直返 m_mode(+0x10)。
int IMethod::GetMode(unsigned int* outError)
{
    if (!m_bInit)
    {
        if (outError != NULL)
        {
            *outError = 0x70000005;
        }
        return 0;
    }
    return m_mode;
}