void* CUserCharacInfo::getCurCharacAddInfoRefW()
{
    enableSaveCharacInfo();
    return (void*)&m_selected->m_additioalInfo;
}