void CEnchanter::Reset()
{
    for (int i = 0; i < 2; ++i)
        m_materials[i].Reset();
    m_user = 0;
    m_peer = 0;
    m_uid = 0;
    m_peerUid = 0;
    m_field38 = 0;
    SetType((ENUM_EXPERT_JOB_TYPE)1);
    m_field3c = 0;
    m_peerRequestType = ENUM_PEER_REQUEST_TYPE_0;   // ORIG 写序：+0x44 先于 +0x40
    m_field40 = 0;
