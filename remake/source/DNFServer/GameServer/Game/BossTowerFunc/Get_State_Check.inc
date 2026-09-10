bool Global::CGlobalFunc::Get_State_Check(ch_state state)
{
    // b28c-L8 复测（N6 位折叠判据）：ORIG 0x081466f0 保 `mov $0x1,%edx;
    // mov %edx,%ebx; mov %eax,%ecx; shl %cl,%ebx; mov %ebx,%eax; and $0x3420`
    // 的**左移位图**形。4.4 前端会把 `(1 << state) & 0x3420` 折成
    // `0x3420 >> state` 的 shr 形（N6），唯一阻断法是让被移位值成为占槽局部
    // （实测具名 bit 形保 shl，但引入 store/load）⇒ 封顶 N6。
    // 本形同时按 P222 取正向包裹，使越界路径直落尾部共享 `mov $0` 岛
    // （ORIG 两处失败均 `ja/je` 跳同一 +0x29 岛）。
    // [b28d-D9] P231：ORIG 0x081466f0 的 bit 全程寄存器（%ebx）无栈槽
    // （`mov $0x1,%edx; mov %edx,%ebx; shl %cl,%ebx`）；裸 register 存储类
    // 提示改变 4.4 分配偏好（非 asm 绑定，P0 合规）。
    if (static_cast<unsigned int>(state) <= 0xd)
    {
        // [round5 复测] 已穷举三形（均 20 条 NEAR，无法归零）：
        // ①`bit = 1; bit <<= state;`（两句式）= 20 条 NEAR —— 缺 4.1 常量两跳
        //   `mov $0x1,%edx; mov %edx,%ebx`，多一次 state 重载（净 0，序列异）；
        // ②一句式 `bit = 1U << state` = 21 条 DIFF —— 两跳复现，但 4.4 对
        //   `(unsigned)state` 二次使用恒**重载** `mov 0x8(%ebp),%eax`，ORIG 靠
        //   register 保活（%eax 跨 cmp/移位）。
        // ③`register unsigned int st = (unsigned)state;`（承载 state）= 23 条
        //   DIFF —— 4.4 把 register 变量落 %ebx，多 `mov %ebx,%eax` 中转。
        // ⇒ 残差 = N18a（4.1 常量两跳不可造）+ N7（4.1 register 值保活 %eax
        //   vs 4.4 重载/%ebx），取 NEAR 形（20=20 长度全等）。
        register unsigned int bit = 1;
        bit <<= static_cast<unsigned int>(state);
        if ((bit & 0x3420u) != 0)
        {
            return true;
        }
    }
    return false;
