// ============================================================================
// UseCubeMission::rewardMission —— ORIG T 0x08165a2e（rewardMission.md）。
// 同 UseFatigueMission::rewardMission 骨架（行号 0x117/0x11e）。
// [b28c-L26] 残余 4 行判词：
//  ① 首判 `call isRepeatMission; xor $1,%eax; test %al; je`——callee 为 char 返回
//     （ORIG 0x08164fc2 仅 movzbl+ret），bool 化会毁 callee 本体（P168a③b/N 跨版本
//     bool ABI）；X1 `uchar ^=1` 形又会被 4.4 jump-thread 折叠掉紧随的
//     `value.m_fieldA!=0` bool 物化岛（实测 11→13 回退）⇒ 保持短路裸形，缺 1 xor。
//  ② 尾段 `call; test %al,%al; je`（char callee 直判、无 setne）——4.4 对 char 返回
//     在 if() 里恒补 setne 归一，实测本形 0 条差改裸形反增；维持。
//  ③ `mov (%eax),%eax; mov %eax,%ebx` 两段：ORIG 4.1 冗余 regmove（N18a 反向，
//     we-smaller 1 条），4.4 无源形可造。
//  ④ 其余 diff 全为 call/jmp 目标与字符串/行号地址的 .o 重定位占位（§6.3）。
// ============================================================================
bool UseCubeMission::rewardMission(CUser* user, HeroMissionValue& value)
{
    // X1 形在此处**不适用**（实测 ndiff 11→13 变差，已回退）：
    // ORIG @0x08165a2e 把 `value.m_fieldA != 0` 物化成 bool 值
    // （`test %ax,%ax; je L; mov $1,%eax; jmp end; L: mov $0,%eax; end:
    //   test %al,%al; je BODY`），而一旦把前面的 `!isRepeatMission()` 改写成
    //   `register uchar + ^=1 + goto`，4.4 的 jump threading 就会顺手把这段
    //   物化岛折叠掉（缺 5 条）。保持 `&&` 短路原写法反而更接近 ORIG。
    // ⇒ X1 适用前提：**判断之后没有依赖同一条件的 bool 物化岛**。
    // [b28c-L26 档①+② 后追加 P84] bool 化 isRepeatMission 使 xor 段归位；剩余
    // 缺项是 ORIG 把整个 `!isRepeat() && fieldA!=0` 短路结果物化成 bool 局部
    // （`mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je BODY`）⇒ 按 P84 具名 bool。
    // [b28d-solo-HeroMissionEvent] P226 扩展：ORIG 0x08165a2e 把短路结果在 al 中
    // 物化后 `test %al,%al; je`（零槽）；具名 bool 会落栈槽（mov %al,-0xa;
    // cmpb $0,-0xa）。语句表达式（内不含具名变量）复现零槽物化岛。
    if (({ !isRepeatMission() && value.m_fieldA != 0; }))
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x117,
            "[HeroMission] Already reward. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x11e,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    bool ok = rewardProcess(user);
    value.m_fieldA = value.m_fieldA + 1;
    value.m_field6 = 1;
    if (isRepeatMission())
    {
        value.reset();
    }
    return ok;
}