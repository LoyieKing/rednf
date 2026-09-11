// ORIG T 0x080e4994：vector 索引清单 + 0x1b8 包（put_header/put_byte/finalize + CUser::Send）。
void CCharacInformNotice::sendInformNoticeFlag(CUser* user)
{
    std::vector<unsigned char> indexes;
    getInformNoticeFlagIndex(indexes);

    PacketGuard packet;
    packet.put_header(0, 0x1b8);
    packet.put_byte(static_cast<int>(indexes.size()));
    for (size_t i = 0; i < indexes.size(); ++i)
    {
        packet.put_byte(static_cast<int>(indexes[i]));
    }
    packet.finalize(true);
    user->Send(packet);
}