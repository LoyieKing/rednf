void APSystem::CUserProc::GetTodayRewardAndSendPostal(
    CUser* user, _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward)
{
    // [b28c-4] ORIG 0x0812255c：空体保留 sub $0x10 帧（未使用局部）。
    char framePad[0x10];
    (void)framePad;
}