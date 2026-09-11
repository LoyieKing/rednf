void APSystem::CUserProc::CheckAndSendToMessageAllUser(unsigned int a,
                                                       unsigned int b,
                                                       const char* msg)
{
    // [b28c-4] ORIG 0x08122564：空体保留 sub $0x20 帧（未使用局部）。
    char framePad[0x20];
    (void)framePad;
}