int CAssaultMgr::OnRevivalUser(CUser* user)
{
    int result = 0;
    try
    {
        pvp_assault::CAssaultPlace::RevivalUser(user, result);   // ORIG：static 两参符号 _ZN…RevivalUserEP5CUserRi
    }
    catch (ENUM_ERROR e)
    {
        // ORIG 0x082ef49f：header(1,0x97)/byte(0)/byte(e)/int(result=0)
        PacketGuard packet;
        packet.put_header(1, 0x97);
        packet.put_byte(0);
        packet.put_byte(static_cast<int>(e));
        packet.put_int(result);
        packet.finalize(true);
        user->Send(packet);
    }
    return 1;
