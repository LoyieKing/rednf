bool AvatarRechargeServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x22, 9, true, false);
    trace("\t- Loading Avatar Recharge script - ");
    if (loadScript())
        return false;
    return true;
