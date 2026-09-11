// ORIG printClient 0x084b3398
// _ZN11game_master16CClearRewardTest11printClientEP5CUserPKcS4_：0x13e 包封装两段
// 字符串（文件名 + 内容）发本人。
void CClearRewardTest::printClient(CUser* user, char const* fileName, char const* content)
{
    PacketGuard packet;
    packet.put_header(0, 0x13e);
    packet.put_byte(1);
    packet.put_int(strlen(fileName));
    packet.put_str(fileName, strlen(fileName));
    packet.put_int(strlen(content));
    packet.put_str(content, strlen(content));
    packet.finalize(true);
    user->Send(packet);
}