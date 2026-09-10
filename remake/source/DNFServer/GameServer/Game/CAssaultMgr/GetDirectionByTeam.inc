int GetDirectionByTeam(ENUM_ASSAULT_TEAM team)
{
    // ORIG T 0x82e564c（0x30 字节）：具名槽 dir(-0x4) 初值 0，`test;je`(==0)
    // /`cmp $1;je`(==1) 各自写槽 + default 直落 `jmp ret`，统一装载返回。
    // b28d-L3 实测：if/else-if 链在 4.4.6 产 `cmpl $0,0x8(%ebp);jne` 形参内存
    // 直比（差 10）；switch 形装 eax + `cmp $1;je`（差 3，见 VARIANT-ARTIFACT
    // 说明：4.4.6 对 2-case switch 把 case 0 折为 fall-through，缺 test/je 与
    // default jmp，源形不可择）⇒ 保留 switch 形（本 TU 路由 446h46 的最优形）。
    // b28d-solo-CAssaultMgr: ENUM_ASSAULT_TEAM 当前仅 1 个枚举量（值域 {0}）⇒
    // 4.4.6 把 case 0 折进 default（缺 ORIG 的 `test;je` 与 default `jmp`，14 vs 16）。
    // switch 表达式内联 (int) 转换即恢复全宽值域：探针 g2 `switch((int)e)` 与 ORIG
    // 逐条同形（test;je / cmp $1;je / jmp 三链 + 双赋值块）。
    int dir = 0;
    switch ((int)team)
    {
    case 0: dir = 6; break;
    case 1: dir = 4; break;
    }
    return dir;
