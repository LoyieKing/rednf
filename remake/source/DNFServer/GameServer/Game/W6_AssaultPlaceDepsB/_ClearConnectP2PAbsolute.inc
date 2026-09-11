// ORIG T 0x82e92c6 _ClearConnectP2PAbsolute(int)
int CAssaultPlace::_ClearConnectP2PAbsolute(int idx)
{
    for (int i = 0; i <= 7; ++i) {
        if (idx != i) {
            // [b28c-L7] P168a①：callee bool ⇒ 裸形直产 ORIG 0x82e92e6
            // `call; test %al,%al; je`。
            if (_IsCannotConnectP2P(idx, i)) {
                if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
                    GlobalData::s_power_manager->GetPowerWarLog()->IncP2PBadCountSecede();
                }
                LeaveAssaultPlace(i, true, false);
            }
        }
    }
    return 1;
}