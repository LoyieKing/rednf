// ----------------------------------------------------------------------------
// ORIG T 0x081466f0 _ZN6Global11CGlobalFunc15Get_State_CheckE8ch_state
// （docs/class_func_reports/Global__CGlobalFunc/Get_State_Check.md ## 汇编 +
// ## 反编译 C 全文转写）：cmp $0xd + ja（无符号比较）→ 越界直接 0；否则
// (1 << state) & 0x3420 测试位 5/6/12/13（5=队伍玩法、12(0xc)=BossTower，
// 参见同文件 UseItem/DieCharacInDungeon 的 state 分支注释）。命中 → 1，否则 0。
// ORIG 调用方（objdump xref）：DisPatcher_FinishLoad::check_state(0x82654a6)、
// 0x81c2bd4、0x81c4b5b、0x81feef2、0x8232256、0x82324c0。
// [b28d-D9] N18a：P231 裸 register 消槽成功（20/20）；残 2 条 = 4.1 冗余 mov $1,%edx; mov %edx,%ebx 不可造（ORIG 0x081466f0 sz 0x00000031）
// [P6S5 2026-09-11 本波复核] 20/20 NEAR 维持；残差直证 = ORIG 0x081466f0
//   `mov $0x1,%edx; mov %edx,%ebx`（4.1 常量先落 %edx 再中转入 %ebx）+ 我方 4.4
//   `mov $0x1,%ebx; mov 0x8(%ebp),%eax`（参数重载）⇒ 净条数相同（20），N7/N18a 族，不可造。
//   ⚠ 头轴线索（上报，未改 .h）：`CBossTowerFunc::GetItem`（本 TU，原 AE 89/89）的
//   ORIG prett 串 = "static **bool** Global::CBossTowerFunc::GetItem(CUser*, int, bool, char)"
//   （strall 实读 O-only），我方 = "static **char** …" ⇒ 返回类型拼写差；
//   因声明在 `BossTowerFunc.h:61`（子代理禁改 .h）⇒ 只上报 P6AuditOldN/P7HeaderAxis。
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
        // [S4NexonProto 复测] 追加三形（探针 /tmp/probe/gsc.cpp G2/G4/G5，本轮实测）：
        //   ④ `bit <<= state;`（enum 直作移位量，去 cast）= 与 ① 逐条同形
        //      （`mov $0x1,%ebx` 单跳 + `mov 0x8(%ebp),%eax` 重载，20 条）；
        //   ⑤ `register int bit = 1;`（与 enum 同型）= 同 ①；
        //   ⑥ `register unsigned int bit = 1;` 声明于 if **之前**（函数级）=
        //      `mov $0x1,%ebx` 前移到 cmp 之前，长度仍 20、序列异位。
        //   ⇒ `mov $0x1,%edx; mov %edx,%ebx`（4.1 常量两跳）+ %eax 跨 cmp/shl 保活
        //   在 4.4 下三形皆不可造，N18a/N7 锥实；维持 NEAR（20=20）。
        register unsigned int bit = 1;
        bit <<= static_cast<unsigned int>(state);
        if ((bit & 0x3420u) != 0)
        {
            return true;
        }
    }
    return false;
}