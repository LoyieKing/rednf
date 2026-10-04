// ---- AvatarCoin::BuyProcess ORIG @ 0x081801d0 ----
// mangled _ZN10AvatarCoin10BuyProcessEP5CUserR10Inven_Item
// 陶罐（avatar pottery）商品购买：isAvatarPottery(item.m_addInfo) 为假直接
// 返 0；否则取 m_addInfo2（+0x07）为币值，加到背包基址 +0x658 的 AvatarCoin，
// SaveToDB 失败打 ORIG 同文日志（__PRETTY_FUNCTION__ 文本），随后
// HistoryLog::AddLog + SendSyncPacket，返 1。
bool AvatarCoin::BuyProcess(CUser* user, Inven_Item& item)
{
    // [b28c-lane17] P0：清除历史 register asm("al") 绑定，改自然条件形。
    // ORIG 0x081801ee `test %al,%al; je 失败`。
    if (reinterpret_cast<AvatarRouletteServer*>(GlobalData::s_pAvatarRoulette)
            ->isAvatarPottery(static_cast<unsigned long>(item.m_addInfo)))
    {
        // ORIG 0x081801f6：item.m_addInfo2 先入 ebx 跨 getCurCharacInvenW 保活，
        // +0x658 直落 Add receiver（P226 内联实参形）。
        user->getCurCharacInvenW()->m_avatarCoin.Add(static_cast<unsigned int>(item.m_addInfo2));
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
}