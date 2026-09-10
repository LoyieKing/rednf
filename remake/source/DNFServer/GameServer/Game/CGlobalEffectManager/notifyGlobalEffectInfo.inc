void CGlobalEffectManager::notifyGlobalEffectInfo() const
{
    PacketGuard packet;
    makeGlobalEffectInfo(packet);
    packet.finalize( true);
    G_GameWorld()->send_all(packet);
