int Inter_CancelRestingUser::dispatch_sig(CUser* user, char* /*data*/, int /*len*/)
{
    if (user != 0)
    {
        user->setRestingUserRestrict(false);
    }
    return 0;
