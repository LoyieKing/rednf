int InterLoadServiceRestrictPolicy::dispatch_sig(CUser* user, char* buf, int len)
{
    SigLoadServiceRestrictPolicy* sig =
        reinterpret_cast<SigLoadServiceRestrictPolicy*>(buf);
    if (sig == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x94, 5);
        tr("[Taiwan, Restrict] SigLoadRestrictPolicy is null.");
        return 0x95;
    }
    ARAD::Singleton<ServiceRestrictManager>::Get()->beginPolicy(
        (RestrictCategory::Enum)sig->category, std::string(sig->policyString));
    return 0;
