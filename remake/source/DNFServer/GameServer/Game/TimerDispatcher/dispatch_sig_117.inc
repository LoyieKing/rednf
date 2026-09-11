bool Timer_Security_NonClient_CheckResponse::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x86394f0：call _NS_NonClient_CheckResponse(p1,p2,p3) 后 leave/ret（eax 未定）。
    _NS_NonClient_CheckResponse(p1, p2, static_cast<int>(p3));
}