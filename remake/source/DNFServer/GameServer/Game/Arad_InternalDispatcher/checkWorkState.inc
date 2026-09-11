// ORIG Dispatcher_AradAvatarConvert::checkWorkState 0x081a003e W：
//   Secu Check(user,0x2a) 非 0 → SendCmdErrorPacket(0x24f,err) 返回 false；
//   否则 Singleton<AvatarConvertServer>::Get()->CheckCondition(user,&sig)；
//   尾部 ORIG 0x081a00a8 形 `call; xor $0x1,%eax; test; je →mov$1 岛` = !boolcall()（技巧1）。
bool Dispatcher_AradAvatarConvert::checkWorkState(CUser* user,
                                                  ::arad::SigAradAvatarConvert& sig)
{
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, (unsigned char)secuErr);
        return false;
    }
    if (!Singleton<AvatarConvertServer>::Get()->CheckCondition(user, &sig))
        return false;
    return true;
}