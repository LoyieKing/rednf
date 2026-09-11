// ORIG 0x0819026c：cMyTrace(__PRETTY_FUNCTION__,0x22,9,true,false) +
// "\t- Loading Avatar Recharge script - "；loadScript()==0 即成功。
bool AvatarRechargeServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x22, 9, true, false);
    trace("\t- Loading Avatar Recharge script - ");
    if (loadScript())
        return false;
    return true;
}