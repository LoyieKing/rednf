// ---- AvatarCoin::GetPackage ORIG @ 0x0818028a ----
bool AvatarCoin::GetPackage(CUser* user, int avatarUid, bool bReset)
{
    // [b28c-lane17] ORIG 0x0818029b：`cmpb $0x0,-0xc; je body` ⇒ 源码是
    // `if (bReset) return 1;`（真值提前返回，body 直落），非 `if(!bReset){body}`。
    if (bReset)
        return 1;
    {
        // ORIG 0x081802a4：avatarUid 先入 ebx，再 call getCurCharacInvenW
        // + add $0x658 直落 Add receiver（无 coinOf call）。
        user->getCurCharacInvenW()->m_avatarCoin.Add(static_cast<unsigned int>(avatarUid));
        if (!SaveToDB(user))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_AvatarRoulette.cpp",
                __PRETTY_FUNCTION__, 0x1b0,
                "SaveAvatarCoin failed");
        }
        HistoryLog::AddLog(user, static_cast<unsigned int>(avatarUid));
        SendSyncPacket(user);
    }
    return 1;
}