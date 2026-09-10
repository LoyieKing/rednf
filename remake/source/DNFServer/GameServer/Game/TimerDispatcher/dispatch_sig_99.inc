int TimerAssaultSuperPowerState::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 3);
    if (user == NULL) {
        return 0;
    }
    // ORIG 0x08634180：短路 && 物化 %al 双岛 merge（mov$1/mov$0 + test%al）。
    // N96/P0：绑定形已废；短路直判为 4.4 常态（4.1 merge 岛形属跨版本 bool
    // ABI 负例在册）。
    if (p3 != 0 &&
        static_cast<unsigned int>(user->getCurCharacNo()) != p3)
        return 0;
    user->SetCurCharacSuperState(false);
    pvp_assault::CAssaultPlace::SendUserState(user);
    return 1;
