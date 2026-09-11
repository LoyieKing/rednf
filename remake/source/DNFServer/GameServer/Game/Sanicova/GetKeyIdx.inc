// ORIG T 0x08599872：按 +0x2e 索引读全局密钥管理器表（0x940be80 基址，
// 步长 16 字节；idx==0 取 +4 槽、否则取 +8 槽 → manager[key*4+1/2]）。
int CPad::GetKeyIdx(int idx)
{
    if (idx == 0)
        return GlobalData::s_secu_cipher_keystring_manager_[m_keyIdx2e].keyId;
    return GlobalData::s_secu_cipher_keystring_manager_[m_keyIdx2e].tblId;
}