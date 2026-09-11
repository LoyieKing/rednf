void* CUserCharacInfo::getCurCharacAddInfoW()
{
    enableSaveCharacInfo();
    return (void*)CUR->m_addInfo;
}