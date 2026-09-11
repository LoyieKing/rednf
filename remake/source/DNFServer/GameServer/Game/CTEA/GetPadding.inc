// ---- ORIG _ZN7IMethod10GetPaddingEPj T @0x080b4476（0x2f）：同形，直返 m_padding(+0x14)。
int IMethod::GetPadding(unsigned int* outError)
{
    if (!m_bInit)
    {
        if (outError != NULL)
        {
            *outError = 0x70000005;
        }
        return 0;
    }
    return m_padding;
}