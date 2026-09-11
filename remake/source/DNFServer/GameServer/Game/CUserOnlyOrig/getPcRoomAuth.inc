bool CUser::getPcRoomAuth() const   // 0x0863be54 W；调用点 xor $0x1 ⇒ bool（Main 代落 CUser.h 同步）
{
    return m_pcRoomAuth;
}