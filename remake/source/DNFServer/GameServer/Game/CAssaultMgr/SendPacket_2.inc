bool CAssaultPlace::SendPacket(PacketGuard& packet, ENUM_ASSAULT_TEAM team)
{
    // ORIG T 0x82e6c56：仅指定队伍转发。
    bool ok = true;
    for (int i = 0; i < 8; ++i) {
        CUser* user = m_assaulters[i].GetUser();
        if (user != 0 && m_assaulters[i].GetTeam() == team)
        {
            // [b28c-L7] 同上（ORIG 0x082e6cc6 同形）。
            if (!user->Send(packet))
                ok = false;
        }
    }
    return ok;
}