bool AvatarCoin::BuyProcess(CUser* user, Inven_Item& item)
{
    // [b28c-lane17] P0：清除历史 register asm("al") 绑定，改自然条件形。
    // ORIG 0x081801ee `test %al,%al; je 失败`。
    if (reinterpret_cast<AvatarRouletteServer*>(GlobalData::s_pAvatarRoulette)
            ->isAvatarPottery(static_cast<unsigned long>(item.m_addInfo)))
    {
        // ORIG 0x081801f6：item.m_addInfo2 先入 ebx 跨 getCurCharacInvenW 保活，
        // +0x658 直落 Add receiver（P226 内联实参形）。
        reinterpret_cast<AvatarCoin*>(
            reinterpret_cast<char*>(user->getCurCharacInvenW()) + 0x658)
            ->Add(static_cast<unsigned int>(item.m_addInfo2));
        if (!SaveToDB(user))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_AvatarRoulette.cpp",
                __PRETTY_FUNCTION__,
                0x19d, "SaveAvatarCoin failed");
        }
        HistoryLog::AddLog(user,
                           static_cast<unsigned int>(item.m_addInfo2));
        SendSyncPacket(user);
        return true;
    }
    return false;
