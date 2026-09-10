void AvatarFixedHiddenOptionServer::UnitTest(AvatarFixedHiddenOptionServer* server)
{
    if (server == 0)
        return;
    Test1(server, 0, 1);
    Test1(server, 0x95, 1);
    Test1(server, 0x96, 2);
    Test1(server, 0x26dd, 0x3e);
    Test1(server, 0x26de, 0x3f);
    Test1(server, 9999, 0x3f);
