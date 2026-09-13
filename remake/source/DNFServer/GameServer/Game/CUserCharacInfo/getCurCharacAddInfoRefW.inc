void* CUserCharacInfo::getCurCharacAddInfoRefW()
{
    enableSaveCharacInfo();
    return (void*)&CUR->m_addInfo;
}