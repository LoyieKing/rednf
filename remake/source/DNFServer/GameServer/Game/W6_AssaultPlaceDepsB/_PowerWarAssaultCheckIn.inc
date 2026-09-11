// ORIG T 0x82e72fa _PowerWarAssaultCheckIn()
void CAssaultPlace::_PowerWarAssaultCheckIn()
{
    for (int i = 0; i <= 7; ++i) {
        // b28 BL6: P0——ORIG 无 now 槽往返（getCurSec 结果寄存器直传出参槽）；
        // 正向 Empty continue 岛形复现 ORIG 0x82e72fa test/jne+jmp 尾岛。
        if (m_assaulters[i].Empty())
            continue;
        CUser* u = m_assaulters[i].GetUser();
        u->SetPowerWarLastPlayTime(GlobalData::s_systemTime_.getCurSec());
    }
}