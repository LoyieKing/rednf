void CAssaultPlace::EvadeAssaultPlace(CUser* user)
{
    if ((unsigned int)(m_state - 2) > 1u) {
        if ((int)GlobalData::s_systemTime_.getCurSec() - m_abuseSec > 5)
            throw (ENUM_ERROR)0x13;
    }
    if (m_checkUser /*+0x134*/ == user)
        throw (ENUM_ERROR)0x13;

    int idx = _GetUserIndex(user, true);
    // b28 BL6 实测：交换操作数序→4.4 走 %ebx callee-saved+push 更劣（13）；
    // 保留原序，残差 4 条 = 4.4 把 this 装载选入 %eax 触发原值 spill %edx
    // （N18a 反向 regmove 假象，同 TU 探针 f0/f3/f4 均出 ORIG 形、真实函数
    // 上下文不可造）判跨版本寄存器分配。
    // [b28d-solo-W6_AssaultPlaceDepsA] 残差 N108+N7：ORIG 0x082eb4b1 `jbe +0x64`
    //   与 0x082eb4d2 `je +0x65` 双守卫分落两独立 landing（差 1）——4.4 恒合并共享
    //   出口（P223 双标签/负条件 goto 两形实测均被 jump-thread）；且 0x082eb564
    //   `cmp %edx,%eax` 角色互换（交换操作数序反引 push %ebx 劣化，已回退）。
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082eb564 `call GetTeam; mov 0x8(%ebp),%edx;
    //   mov 0x124(%edx),%edx; cmp %edx,%eax`：enum 返回值留 %eax、int 字段装 %edx。
    //   写 `(int)GetTeam()` 使 4.4 先把结果中转 %edx（`mov %eax,%edx`）⇒ 删 cast 同型比较。
    if (m_assaulters[idx].GetTeam() != m_winTeam /*+0x124*/)
        throw (ENUM_ERROR)0x13;

    LeaveAssaultPlace(_GetUserIndex(user, true), true, true);
