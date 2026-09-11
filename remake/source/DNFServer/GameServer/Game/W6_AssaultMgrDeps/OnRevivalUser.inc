// ===========================================================================
// OnRevivalUser —— ORIG T 0x082ef464 (0x10b)
// local result=0；static RevivalUser(user,result)；若中途异常：
// PacketGuard(header(0,0x97)/byte(1)/byte(0)/byte(err)/finalize(true)) 发给该用户。
// ===========================================================================
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
}