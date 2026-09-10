bool CNPCDynamicInfo::forceChangeMood(ENUM_NPC_MOOD mood)
{
    if (getMood() == mood)
        return false;
    while (changeMood(true) != mood)
        ;
    return true;
