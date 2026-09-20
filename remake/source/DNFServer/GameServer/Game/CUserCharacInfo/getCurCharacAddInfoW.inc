void* CUserCharacInfo::getCurCharacAddInfoW()
{
    enableSaveCharacInfo();
    return (void*)&m_selected->m_additioalInfo;
}