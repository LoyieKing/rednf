// ---------------------------------------------------------------------------
// ORIG T 0x82e75ca (size 0xd1)：_AvailableAssault(CUser*, int&)
// 年龄门槛 0xf（PowerWar 中 vill==7 时 0xc）；<0x1e 级未入场检查 → 0xe/0xd；
// 组队分支：party is_limit_minimum_age→0x5f、is_limit_minimum_level→0xff/0xfe。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] AE：P0 删 bool/uid 具名槽 + `<= 0x1d`（setle）+
//   P59 正极性 throw 顺序；补负极性包裹 `if (!is_limit_minimum_level(...)) {…}`
//   （ORIG 0x082e77b5 `xor $0x1,%eax; test %al,%al; je <函数尾>`；正极性
//   `if (f()) return;` 4.4 产 test;jne 少 xor）⇒ 149=149 对齐；p167 CLEAR。
//   ORIG 0x082e75ca
void CAssaultPlace::_AvailableAssault(CUser* user, int& result)
{
    CParty* party = 0;
    if (user->CheckInParty()) {
        party = (CParty*)user->GetParty();
    }

    int needAge = 0xf;
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e75fb 段：IsPowerWarEventOn 与
    //   getCurCharacVill==7 的 `&&` 直接物化 `mov $1/%eax ... mov $0/%eax; test; je`
    //   （P0 无具名 bool 槽；4.4 对具名 bool 会多 movb 物化 + 帧 +0x10）。
    if (GlobalData::s_power_manager->IsPowerWarEventOn()
        && ((CUserCharacInfo*)user)->getCurCharacVill() == 7)
    {
        needAge = 0xc;
    }

    if (!user->CheckLimitMiniumAge(needAge, result)) {
        throw (ENUM_ERROR)0x5f;   // ORIG __cxa_throw ENUM_ERROR(0x5f)
    }

    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e7689 `cmp $0x1d,%eax; setle`
    //   ⇒ 原码 `<= 0x1d`（写 `< 0x1e` 产 setl 不同形）；uid 无具名槽（P0 内联，
    //   ORIG movzwl %ax,%edx 直存 *result）；throw 分支 ORIG `jne <0xe>` fall-through
    //   0xd ⇒ 正极性 `== user` 写法（P59 反向布局）。
    if (((CUserCharacInfo*)user)->get_charac_level() <= 0x1d) {
        result = (int)user->get_unique_id();
        if (m_checkUser /* +0x134 */ == user) {
            throw (ENUM_ERROR)0xd;
        }
        throw (ENUM_ERROR)0xe;
    }

    if (party != 0) {
        int partyNeedAge = 0xf;
        if (GlobalData::s_power_manager->IsPowerWarEventOn()
            && ((CUserCharacInfo*)user)->getCurCharacVill() == 7)
        {
            partyNeedAge = 0xc;
        }

        if (!party->is_limit_minimum_age(partyNeedAge, result)) {
            throw (ENUM_ERROR)0x5f;
        }
        // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e77b5 `call is_limit_minimum_level;
        //   xor $0x1,%eax; test %al,%al; je <+0x25d 函数尾>` ⇒ 原码是负极性包裹
        //   `if (!f()) { ... }`（正极性 `if (f()) return;` 4.4 产 `test;jne` 少 xor）。
        //   块内 ORIG 0x082e77c1 `cmp 0xc(%ebp),%eax; jne <0xff>` ⇒ 相等落 fall-through 0xfe。
        if (!party->is_limit_minimum_level(0x1e, result)) {
            if (m_checkUser == user) {
                throw (ENUM_ERROR)0xfe;
            }
            throw (ENUM_ERROR)0xff;
        }
    }
}