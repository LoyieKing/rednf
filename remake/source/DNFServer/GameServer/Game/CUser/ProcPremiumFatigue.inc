void CUser::ProcPremiumFatigue()
{
    if (getCurCharacR() == 0)
        return;
    // ORIG 0x0867cbe4：正向 `if (IsHavePremiumAdvantage())` 包体（Ghidra
    // `if (cVar2 != '\0')`，产 `test %al;je <ret>`；不写早退 `==0` 形以免 xor）。
    if (IsHavePremiumAdvantage())
    {
        // P48 双分支各自重取 GetAdvantageFatigueRate（不缓存）；pcRoom 命中才加
        // dm+0x51c 周末加成。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(0) &&
            m_premium.GetAdvantageFatigueRate() != 0 && GetPCRoomNo() != 0)
        {
            setCurCharacMaxPremiumFatigue(
                static_cast<unsigned short>(
                    (short)((static_cast<int>(static_cast<unsigned short>(
                                m_premium.GetAdvantageFatigueRate())) *
                            DEFAULT_MAX_FATIGUE) / 100) +
                    G_CDataManager()->m_serverParameter
                        .m_weekendBouns4b4.m_bonusFatiguePercent));
        }
        else
        {
            setCurCharacMaxPremiumFatigue(
                static_cast<unsigned short>(
                    (static_cast<int>(static_cast<unsigned short>(
                         m_premium.GetAdvantageFatigueRate())) *
                     DEFAULT_MAX_FATIGUE) / 100));
        }
    }
