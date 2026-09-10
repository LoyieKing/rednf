bool user_creature::CDBMsgSender::Init(ENUM_INTERNALPACKET type, CUser* user)
{
    if ((int)type == ENUM_INTERNALPACKET_0x89)
        m_stream = GlobalData::s_big_stream_pool->Acquire("Creature.cpp", 0xa7);
    else
        m_stream = GlobalData::s_stream_pool->Acquire("Creature.cpp", 0xa9);
    if (m_stream == 0)
        return false;
    m_user = user;
    CStreamGuard guard(m_stream, false);
    (*guard)->operator<<((int)type);
    (*guard)->operator<<((int)m_user->GetUID());
    return true;
