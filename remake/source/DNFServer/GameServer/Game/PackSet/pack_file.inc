FILE* PackSet::pack_file()     // 0x08ac7608
{
    if (!m_bUseEncryptedFile)
    {
        return m_packFile;
    }
    fprintf(stdout, "%s\n", "_C_DNF_SCRIPT_CRYPTER_ is turned on!!");
    fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x677, __FUNCTION__);
    return m_packFile;
}