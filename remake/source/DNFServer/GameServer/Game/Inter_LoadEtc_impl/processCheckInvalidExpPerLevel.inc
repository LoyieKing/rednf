void Inter_LoadEtc::processCheckInvalidExpPerLevel(CUser& user)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 97/97：levelExpLow/High 同型化为 unsigned
    // （P211 档③）翻正第二比较装载序 + 首比较方向翻转（levelExpLow>exp）+ `100 - min(...)`
    // 单表达式（消回读槽）。
    unsigned levelExpLow = (unsigned)G_CDataManager()->get_level_exp(
        user.get_charac_level());
    unsigned levelExpHigh = (unsigned)G_CDataManager()->get_level_exp(
        user.get_charac_level() + 1);
    unsigned exp = (unsigned)user.get_charac_exp();
    // [b28d-53] ORIG 0x084e8556 段：`mov levelExpLow; cmp exp; ja body` +
    // `mov exp; cmp levelExpHigh; jbe end` = 双守卫共享尾（P257 正向包裹消
    // 早返形多发的 jmp+nop 岛）。残余：第二比较 4.4 恒折 `levelExpHigh >= exp`
    // 方向（ORIG 为 `exp <= levelExpHigh`）⇒ N18b/N22 比较方向族，97/97 封顶。
    if (levelExpLow > exp || exp > levelExpHigh)
    {
        // [b28d-53] ORIG 0x084e8577 `call; xor $1,%eax; test %al; je end`
        // ⇒ bool callee + 取反形（P168a①）。
        if (!user.isTradePunishType(1))
        {
            user.SetTradePunishType(1);
            DB_SaveUserHandicap::makeRequest(user.get_acc_id(), 0x9c42, 0xb, 0, 3, 0);
            // ORIG 0x084e860b 段：ctor 前求 varargs 并保活（N80/P227 全表达式临时链）。
            cMyTrace(__PRETTY_FUNCTION__, 0x6c7d, 5)(
                "[InvalidExp] Invalid character exp per level. "
                "(mid:%u, charac:%u, level:%d, exp:%d,%d,%d)",
                user.get_acc_id(),
                user.get_charac_no(-1),
                user.get_charac_level(),
                user.get_charac_exp(),
                levelExpLow,
                levelExpHigh);
        }
    }
}