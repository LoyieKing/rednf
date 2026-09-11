// ===== InterLoadServiceRestrictPolicy::dispatch_sig —— ORIG 0x08160796 =====
// buf==0 → cMyTrace(0x94) 日志 "[Taiwan, Restrict] SigLoadRestrictPolicy is null." 返回 0x95；
// string 取 buf+0x119d，beginPolicy(Get(), *(int*)buf, string)；返回 0。
// [b28d-solo-TaiwanInternalDispatcher] AE（sig 提前 + std::string 临时全表达式）
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
}