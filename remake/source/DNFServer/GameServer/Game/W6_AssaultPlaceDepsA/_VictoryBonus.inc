int CAssaultPlace::_VictoryBonus(CUser* user, char side)
{
    // [b23S3C] P0 槽映射：ORIG 预清 6 槽（低→高 -0x20..-0xc）=
    // myVictory/penaltyValue/breakBonus/winCount/enemy/循环 i；删 dead local
    // enemySideChecked 与 powerParam/slotUser/penaltyOut 缓存槽（P48 每用点重取
    // GetUser()，ORIG 循环体 3 次 call GetUser、侧写结果经 callee-saved %ebx 跨调）。
    int myVictory = 0;
    int penaltyValue = 0;
    int breakBonus = 0;
    int winCount = 0;
    CUser* enemy = 0;

    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].GetUser() == 0)
            continue;
        if (((CUser*)user)->getPowerSide()
            != ((CUser*)m_assaulters[i].GetUser())->getPowerSide())
        {
            enemy = (CUser*)m_assaulters[i].GetUser();
            break;
        }
    }

    if (((CUser*)user)->getPowerSide() == side) {
        myVictory = ((CUser*)user)->GetPowerWarPartyVictory();
        penaltyValue = G_CDataManager()->m_powerParameter
                           .GetPenalty(myVictory).m_fieldC;
        if (enemy != 0) {
            winCount = ((CUser*)enemy)->GetPowerWarPartyVictory();
            if (winCount > 4) {
                breakBonus = G_CDataManager()->m_powerParameter.GetBreakBonus(
                    winCount);
                _SendHoldVictory(user, enemy, winCount);
            }
        }
        _SendVictoryBonus(user, myVictory, penaltyValue, winCount, breakBonus);
    } else if (enemy != 0) {
        myVictory = ((CUser*)enemy)->GetPowerWarPartyVictory();
        penaltyValue = G_CDataManager()->m_powerParameter
                           .GetPenalty(myVictory).m_fieldC;
        winCount = ((CUser*)user)->GetPowerWarPartyVictory();
        if (winCount > 4) {
            breakBonus = G_CDataManager()->m_powerParameter.GetBreakBonus(
                winCount);
            _SendHoldVictory(enemy, user, winCount);
        }
        _SendVictoryBonus(enemy, myVictory, penaltyValue, winCount, breakBonus);
    }
    // b28 BL6: 4.4 右操作数先入 %eax（ORIG 尾块 eax=-0x18=breakBonus/edx=-0x1c=pen），
    // 交换加法两侧复现 ORIG lea 序（数学等价）。
    return penaltyValue + breakBonus;
