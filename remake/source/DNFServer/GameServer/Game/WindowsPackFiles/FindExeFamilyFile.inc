bool WindowsCodeProtectorServer::FindExeFamilyFile(const char* name) // 0x085755f6
{
    if (m_exeFamily != 0 && strcmp(m_exeFamily->m_name, name) == 0)
    {
        return true;
    }
    return false;
}