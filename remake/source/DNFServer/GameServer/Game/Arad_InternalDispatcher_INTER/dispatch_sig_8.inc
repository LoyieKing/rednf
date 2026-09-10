int Arad_INTER_kAvatarConvert::dispatch_sig(CUser* user, char* body, int len)
{
    if (user == 0)
        return 0x6d8;
    if (user->getCurCharacR() == 0)
        return 0x6db;

    arad::SigAradAvatarConvert* sig = (arad::SigAradAvatarConvert*)body;   // ORIG 读 0x10(%ebp)=body
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x6e0, "AVATAR CONVERT INTER_DSP(kAvatarConvert) null.");
        return 0x6e1;
    }

    // P48 + ORIG 寄存器出口：! 语义产生 xor $0x1 翻转；Check/Convert 每次各自 Get()。
    if (!Singleton<AvatarConvertServer>::Get()->CheckCondition(user, sig))
        return 0;
    if (!Singleton<AvatarConvertServer>::Get()->Convert(user, sig))
        return 0;
    return 0;
