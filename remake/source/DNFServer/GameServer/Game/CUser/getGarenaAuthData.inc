char* CUser::getGarenaAuthData()
{
    return reinterpret_cast<char*>(&m_garenaAuthData);  // +0x6ef92 Taiwan::GarenaAuthData 成员视图（签名保持 char*）
