int Dispatcher_UpdateContractOfCubeInfo::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x825fa8e：!isAffectedPremium(0x5c) → xor $1,test al 形态返回 8。
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x5c))
    {
        return 8;
    }
    return 0;
}