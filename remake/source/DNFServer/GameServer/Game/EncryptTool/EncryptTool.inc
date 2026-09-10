EncryptTool::EncryptTool()
{
    memset((char*)this, 0, 0x20);
    m_rijndael = new CRijndael;
    for (int i = 0; i <= 0xf; ++i)
    {
        memset(m_key[i], 0, 0x20);
        strncpy((char*)m_key[i], s_keyTable[i], strlen(s_keyTable[i]));
    }
