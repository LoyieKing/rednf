void NPC_Teleport::set_teleport_data(STTeleportData const& data)
{
    // ORIG 0x08284124：set_party_teleport(data.m_tp)。
    set_party_teleport(data.m_tp);   // STTeleportData::m_tp@+0x04（Teleport.h 权威成员）
