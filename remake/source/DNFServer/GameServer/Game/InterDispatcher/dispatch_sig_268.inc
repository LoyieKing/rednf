int Inter_GoblinPadChagePwdReward::dispatch_sig(
    CUser* user, char* data, int /*len*/)
{
    char* packet = data;
    if (user != 0)
    {
        user->getPad()->SendChagePwdReward(user);
    }
    return 0;
