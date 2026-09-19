bool Timer_Security_NonClient_CheckResponse::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x86394f0：call _NS_NonClient_CheckResponse(p1,p2,p3) 后 leave/ret（eax 未定）。
    _NS_NonClient_CheckResponse(p1, p2, static_cast<int>(p3));
#if !remake_verify
    return 1;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}