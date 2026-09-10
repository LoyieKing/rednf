int CPad::GetKeyIdx(int idx)
{
    if (idx == 0)
        return GlobalData::s_secu_cipher_keystring_manager_[m_keyIdx2e].keyId;
    return GlobalData::s_secu_cipher_keystring_manager_[m_keyIdx2e].tblId;
