void EncryptTool::setCryptKey(int index)
{
    m_rijndael->Initialize((const char*)m_key[index], 0x20,
                           CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
