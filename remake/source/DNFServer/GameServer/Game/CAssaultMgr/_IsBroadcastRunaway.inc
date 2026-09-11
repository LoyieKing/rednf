// ORIG T 0x82eab1a：_IsBroadcastRunaway —— PowerWar 且村庄==7 直接 false；
//   否则 getCurCharacR()!=0 决定返回值（0x82eab50 `mov $0` 在 t 为 true 分支）。
bool CAssaultPlace::_IsBroadcastRunaway(CUser* user)
{
    if (GlobalData::s_power_manager->IsPowerWarEventOn() &&
        user->getCurCharacVill() == 7)
        return false;
    // ORIG 0x82eab62：`test eax;setne al; test al; je; mov$1;...mov$0`
    // 物化尾岛（P197④ call 结果寄存器直比出 sete；N96 绑定已清）。
    if (user->getCurCharacR() != 0)
        return true;
    return false;
}