void DB_UpdateRewardMedal::makeRequest(int, const _SIG_LOAD_REWARD_MEDAL&)
{
    // [b28c-4] ORIG 0x08124c70：空体保留 sub $0x10 帧（未使用局部）。
    char framePad[0x10];
    (void)framePad;
