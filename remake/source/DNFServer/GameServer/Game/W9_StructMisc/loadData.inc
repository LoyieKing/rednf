// ORIG T 0x086434fc：先虚调 _reset()，拷入 6 dword 数据区；
// 若当日跨天(按 serverEnvir.m_fatigueTime(+0x37c) 口径)再虚调 ResetDaily()；
// 有历史层且 lastEnterTime<0x4e275df1 时清层标记 + alter 脏标记。恒返 true。
// [b28d-23 N·N93] 槽/帧差（69/80）⇒ N。ORIG 0x086434fc
bool TOD_UserState::loadData(CUser* pUser, char* pData)
{
    _reset();
    SIG_REQUEST_TOD_USER_STATE& in =
        *reinterpret_cast<SIG_REQUEST_TOD_USER_STATE*>(pData);
    // [L8 2026-09-10 判据⑫/P83] ORIG 0x08643522 段是**结构整体赋值**（一次
    // `mov -0xc(%ebp),%edx` 后连发 6 条 `mov N(%edx),%ecx; mov %ecx,8+N(%eax)`，
    // N=0,4,8,0xc,0x10,0x14）⇒ 原码 `m_data = in;`（m_data 类型即
    // SIG_REQUEST_TOD_USER_STATE，sizeof 0x18，static_assert 已锁）。
    // 旧逐字段赋值（7 条）多 2 个具名临时槽（我方 -0x10/-0x14 vs ORIG 仅 -0xc）
    // 并多发 11 条指令（80 vs 69）。
    m_data = in;

    // [L8 2026-09-10 P0/P183] ORIG 0x0863545..0x086355a 段：curSec 结果升格 %esi、
    // lastPlayTick 升格 %ebx（跨后续 call 保活）后直接 `mov %esi,0x8(%esp)` /
    // `mov %ebx,0x4(%esp)` —— ORIG **无槽往返** ⇒ 原码无具名局部；我方两个
    // `const long` 局部落 -0x10/-0xc 槽并多 4 条 load/store。
    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               pUser->getCurCharacLastPlayTick(),
                               GlobalData::s_systemTime_.getCurSec()))
    {
        ResetDaily();
    }
    if (m_data.m_lastEnterLayer != 0)
    {
        // [AxisB4 2026-09-10 轴B] ORIG 0x086435a3 `cmp $0x4e275df0,%eax; jg`
        // 为**有符号**序（Ghidra C：`*(int*)(this+0x18) < 0x4e275df1`）；
        // 旧文 `static_cast<unsigned int>(...) < 0x4e275df1u` 产 `ja` 属真语义分叉
        // （m_lastEnterTime 为负时判据相反）。向 ORIG 对齐去掉 unsigned cast。
        if (m_data.m_lastEnterTime < 0x4e275df1)
        {
            m_data.m_lastEnterLayer = 0;
        }
        alter();
    }
    return true;
}