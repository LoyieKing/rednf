// ORIG 0x084b7e48：PacketGuard，makeGlobalEffectInfo，finalize(true)，G_GameWorld()->send_all。
void CGlobalEffectManager::notifyGlobalEffectInfo() const
{
    PacketGuard packet;
    makeGlobalEffectInfo(packet);
    packet.finalize( true);
    G_GameWorld()->send_all(packet);
}