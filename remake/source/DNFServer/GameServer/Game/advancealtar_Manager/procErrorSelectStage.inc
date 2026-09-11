// ----------------------------------------------------------------------------
// ORIG T 0x081309f0 _ZN12advancealtar7Manager20procErrorSelectStageEP5CUser10ENUM_ERROR
// user 判空或 err==0 直接返回；否则 PacketGuard: put_header(1,0x10) /
// put_byte(0) / put_int(err) / finalize(true) → CUser::Send。
// ----------------------------------------------------------------------------
void Manager::procErrorSelectStage(CUser* user, ENUM_ERROR err)
{
    if (user == 0 || err == 0)
    {
        return;
    }

    PacketGuard guard;
    guard.put_header(1, 0x10);
    guard.put_byte(0);
    guard.put_int(static_cast<int>(err));
    guard.finalize(true);
    user->Send(guard);
}