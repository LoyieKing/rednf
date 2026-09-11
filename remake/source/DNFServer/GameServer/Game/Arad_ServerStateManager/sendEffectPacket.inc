// ---------------------------------------------------------------------------
// sendEffectPacket —— ORIG 0x081a9a1c：
// PacketGuard 组包：put_header(0,0x21a)/put_int(1)/put_int(code)/
// put_binary(msg,0xc)/finalize(true)；user==NULL → G_GameWorld()->send_all，
// 否则 user->Send。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::sendEffectPacket(ENUM_GLOBAL_EFFECT_ARAD code,
                                               const AradServerStateMessage& msg,
                                               CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0x21a);
    packet.put_int(1);
    packet.put_int((int)code);
    packet.put_binary((char*)&msg, 0xc);   // ORIG put_binary(char*,int)（非 const 重载）
    packet.finalize(true);

    if (user == 0)
        G_GameWorld()->send_all(packet);
    else
        user->Send(packet);
}