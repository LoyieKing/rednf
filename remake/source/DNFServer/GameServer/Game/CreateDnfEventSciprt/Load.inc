bool CreateDnfEventSciprtServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0xd0, 9, true, false);
    trace("\t- Loading EventCreateDnf script - ");
    if (loadScript())
        return false;
    return true;
