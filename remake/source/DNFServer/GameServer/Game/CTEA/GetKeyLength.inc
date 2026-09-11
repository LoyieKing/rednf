// ---- ORIG _ZN7IMethod12GetKeyLengthEPj T @0x080b43e6（0x2f）：与 GetBlockSize 同构，
//   未初始化时写 *outError=0x70000005 并返 0；已初始化直返 m_keyLength(+0xc)。
int IMethod::GetKeyLength(unsigned int* outError)
{
    if (!m_bInit)
    {
        if (outError != NULL)
        {
            *outError = 0x70000005;
        }
        return 0;
    }
    return m_keyLength;
}