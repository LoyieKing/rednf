// Load —— ORIG T 0x081915ba：cMyTrace(pretty,0x1c7,9,true,false) +
// operator()("\t- Loading Emblem Compound script - ") + loadScript()==0。
bool EmblemCompoundServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1c7, 9, true, false);
    trace("\t- Loading Emblem Compound script - ");
    if (loadScript())
        return false;
    return true;
}