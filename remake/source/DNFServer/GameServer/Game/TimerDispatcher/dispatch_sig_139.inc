bool Timer_GenerateCaptchaData::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x863b29c: GlobalData::GenerateCaptchaData(); RegistNextTimer(3); return 1
    GlobalData::GenerateCaptchaData();
    RegistNextTimer(3);
    return 1;
}