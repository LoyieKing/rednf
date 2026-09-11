// [b28d-solo-W8LT_EventTail] DIFF→N 封顶 | ORIG 0x081b6e7a | P167 CLEAR（11/11）。
// 修法：①P171 删 `mine == 0` 守卫（ORIG 只判 empty）；②P13 缓存 end() 具名局部；
// ③P28 增量段后置 `it++`（ppEi）；④P167 判据：`it->m_job` → `(*it).m_job`
// （ORIG 发 `__normal_iterator::deEv`；`->` 会误选 `ptEv`，p167 差集 MISSING deEv /
// EXTRA ptEv 一击命中）⇒ 73=73、槽集/块序全等。
// 残余 2 条（N 封顶）：`begin()/end()` 调用点 ORIG this-first（`mov -0xc,%edx` 先）
// vs 4.4.6 恒 sret-first（`lea -0x1c,%eax` 先）——P75 跨版本实参装载时刻；已穷举
// 5 形（初始化式/(*p)./引用接收/逗号声明/直接初始化括号）全部 sret-first。
// [M1S5 2026-09-11 第 6 形试证·P307 `register vector*` receiver] our 73→72、
// ndiff 46→101 ⇒ 劣化回滚；P339 不适用（槽序已与 ORIG 全等：-0x14/-0x10/-0xc/
// -0xc..-0x20）、§6.6.2 P216 不适用（两侧均无 _ZdlPv/_Unwind_Resume 差）⇒ N 维持。
int GiveGrowCreatureEvent::CheckEventInfo(CUser& user)  // 0x081b6e7a
{
    int job = user.get_charac_job();
    int now = GlobalData::s_systemTime_.getCurSec();
    if (!G_CDataManager()->isAvailableGiveGrowCreatureEventJob(job, now))
        return 3;

    // [G23S4 试形 P341·try3] 具名局部改 `void*`（槽数不变），使用点写成 cast 表达式
    //   ⇒ 期望 4.4 在 sret 调用点先物化 this（ORIG 0x081b6e8d `mov -0xc(%ebp),%edx` 先）。
    void* mine = user.getGrowthCreatureEventdata();
    // P171：ORIG 无 `mine == 0` 守卫（只判 empty）。
    if (static_cast<std::vector<GrowthCreatureEvent::eventCharacInfo>*>(mine)
            ->empty())
        return 0;

    // P13/P28：ORIG 循环前一次求值 end() 存具名局部、增量段用后置 `it++`（ppEi）。
    std::vector<GrowthCreatureEvent::eventCharacInfo>::iterator it =
        static_cast<std::vector<GrowthCreatureEvent::eventCharacInfo>*>(mine)
            ->begin();
    std::vector<GrowthCreatureEvent::eventCharacInfo>::iterator itEnd =
        static_cast<std::vector<GrowthCreatureEvent::eventCharacInfo>*>(mine)
            ->end();
    while (it != itEnd)
    {
        if ((*it).m_job == job)
            return 3;                                   // 该职业已领过
        it++;
    }
    return 0;
}