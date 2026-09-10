bool CUser::alonePlay_Mode()
{
    // ORIG 0x0868dc7a：opt→4B 槽 -0x10（先声明远槽 技巧21）；get_etc_option 结果
    // `mov %ax,-0xa(%ebp); movswl -0xa(%ebp),%eax; cmp $0x1,%eax; jne 岛` ⇒ short 具名槽 +
    // return-true/false 具名物化岛
    CGameOption* opt = GetGameOptionRef();
    const short etc = opt->get_etc_option((ENUM_OPTION_ETC)0x1b);
    // [b28d-solo-CUserOnlyOrig P77/N 封顶] ORIG 0x0868dca5 `movswl -0xa(%ebp),%eax;
    // cmp $0x1,%eax`（4.1 对 short 槽比 32 位化）；4.4 对 short 槽恒融 `cmpw $1,mem`
    // （探针 /tmp/short_probe.cpp f1/f2/f5 三形全融、f3 int 中转产额外槽 -0xc）。
    // 旧源码 `int v = etc;` 多一槽使 opt 落 -0x14、short 落 -0xe（ORIG -0x10/-0xa）
    // 且多 2 条指令；删 int 槽后槽位/帧对齐，仅余 cmpw-vs-movswl 单条跨版本差（N 封顶）。
    // [Lane6 2026-09-10 P286 一试] `if (etc == 1)` 在 4.4 恒融 `cmpw $1,mem`（short 槽），
    // 单 case switch 令 4.4 发「载入 + 寄存器比较」（ORIG 0x0868dca5 `movswl -0xa(%ebp),%eax;
    // cmp $0x1,%eax`）——语义与 if 等价。
    switch (etc) {
    case 1:
        return true;
    default:
        return false;
    }
