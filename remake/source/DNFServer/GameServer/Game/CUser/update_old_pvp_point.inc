// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0865cfac | _ZN5CUser20update_old_pvp_pointEv
void CUser::update_old_pvp_point()
{
    // ORIG 0x8??：writable `%mov eax,%ebx` 单条跨 call 存活；P0/N96 去绑定改
    // 链式单语句（LHS receiver 跨 RHS call 保活，P52），残差记 N18a。
    // b28d-solo-CUser p167 修正（调用目标）：ORIG 0x0865cfac 调
    //   getPVPResultRefW/getPVPResultRefR（非 W/R 变体）；两者实现等价但符号不同。
    getPVPResultRefW()->m_oldExpPoint = getPVPResultRefR()->m_expPoint;
}