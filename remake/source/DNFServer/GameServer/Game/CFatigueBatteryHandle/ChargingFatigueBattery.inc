// ORIG 0x84a8094：
//   level < m_minChargeLevel9 或 level == m_endLevel10 → 不充能返回 0；
//   否则 remain = max(DEFAULT_MAX_FATIGUE-used,0) + DEFAULT_MAX_FATIGUE*v，
//   记录 trace 后返回 Compute(remain, level, specialCnt) 的截 short。
// 注意：used/alreadyFatigue/charNo/level 均按 ORIG 汇编次序读取并进入日志。
short CFatigueBatteryHandle::ChargingFatigueBattery(CUser* user, int v)
{
    // b28d-20: ORIG 0x084a8094 — 守卫为 `!(A && B)` 形（探针 /tmp/p_fat3.cpp chE
    // 逐条复现 ORIG：`jl <mov $1>` + `jne <mov $0>` + `test %al; je 主流程`，
    // 零栈槽 bool 物化）；A = get_charac_level() >= min（call 在左，字段后读，
    // ORIG `call; movzbl 0x5c9(%edx)`）、B = get_charac_level() != end。
    // sp 引用（add $0x68）存 -0x28 槽。trace 用 P227 全表达式临时链：实参
    // traceLevel/alreadyFatigue/charNo 无槽（ORIG 落 %esi/%edi/%ebx 跨 ctor 保活）。
    ServerParameterScript& sp = G_CDataManager()->m_serverParameter;

    if (!(user->get_charac_level() >=
              static_cast<int>(sp.m_fatigueTable5c0.m_minChargeLevel9) &&
          user->get_charac_level() !=
              static_cast<int>(sp.m_fatigueTable5c0.m_endLevel10)))
        return 0;

    int specialCnt = user->GetSpecialLevelCharacCnt(0x46);
    int used = static_cast<int>(user->getCurCharacUsedFatigue() & 0xffff);
    int remain = DEFAULT_MAX_FATIGUE - used;
    if (remain < 0)
        remain = 0;
    remain += DEFAULT_MAX_FATIGUE * v;

    // ORIG movswl %ax：GetFatigueBatteryCharging 返回 short，拓宽进日志。
    cMyTrace(__PRETTY_FUNCTION__, 0x32, 0)(
        "FATIGUE BATTERY : char no(%d), alreadyFatigue(%d), remainFatigue(%d), "
        "level(%d), endLevelCnt(%d), thatDayUsedFatigue(%d)",
        user->getCurCharacNo(),
        static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging())),
        remain, user->get_charac_level(), specialCnt, used);

    return static_cast<short>(
        ComputeFatigueBatteryCharging(remain, user->get_charac_level(), specialCnt));
}