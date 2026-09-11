bool Timer_Schedule_MidNight::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x863a982: g_GoldControlDateFlag++; RegistNextTimer(); return 1
    ++GlobalData::g_GoldControlDateFlag;
    RegistNextTimer();
    return 1;
}