// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
bool CAssaulter::ChangeHp(int hp)      // ORIG T 0x82e6694
{
    // b28d-solo-CAssaultMgr: IDENTICAL_AE（清单 NEAR 2 → AE；路由口径复验）
    // b28d-L3: IDENTICAL_AE — b28d-L3：路由后 AE（ORIG 0x82e6694
    if (hp < 0 || hp > 100)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x18a, 0);
        trace("ASSAULT_CHANGE_HP error (%d)", hp);
        return false;
    }
    // ORIG 0x82e6717：&& 结果 mov$1/0 物化后 test %al（无槽）⇒ 匿名 bool 表达式
    if (GlobalData::s_power_manager->IsPowerWarEventOn() &&
        m_user->getCurCharacVill() == 7)
        m_user->setPowerWarHP((unsigned short)hp);
    else
        m_hp = hp;

    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    int diff = (int)(curSec - m_lastHpSec);
    m_lastHpSec = curSec;
    // ORIG 0x82e676b：`cmpl $2; jg T; mov $0; jmp E; T: mov $1; E: leave`
    // false 先落、true 跳后岛（技巧23 双 goto 钉块序；4.4 直写折 setg）。
    if (diff > 2)
        goto RET1;
    return false;
RET1:
    return true;
}