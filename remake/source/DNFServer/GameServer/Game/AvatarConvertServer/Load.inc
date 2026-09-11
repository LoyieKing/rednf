// Load —— ORIG T 0x081930ba：cMyTrace(pretty,0x37f,9,true,false) +
// operator()("\t- Loading Avatar Convert script - ")；loadScript() 非0 → 0。
bool AvatarConvertServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x37f, 9, true, false);
    trace("\t- Loading Avatar Convert script - ");
    if (loadScript())
        return false;
    return true;
}