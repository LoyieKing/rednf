void CQuestShop::sendCharacQuestPiece(CUser* user)
{
    // ORIG 0x85ef7cc T：put_header(0,0x1be) / put_int(getPiece) / finalize(true) / user->Send
    PacketGuard packet;
    packet.put_header(0, 0x1be);
    packet.put_int(getPiece());
    packet.finalize(true);
    user->Send(packet);
}