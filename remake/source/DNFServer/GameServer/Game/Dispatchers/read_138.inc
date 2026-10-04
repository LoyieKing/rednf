// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081cea7a orig/ours=51/54 ndiff-blocks=6
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax | mov    -0xc(%ebp),%eax
int Dispatcher_SaveGameOption_1::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cea7a：msg typed 指针槽 -0xc（首行物化）；get_int 实参
    // `mov -0xc,%eax; add $0xd,%eax`（char* 算术形），len 读取一律 typed
    // 单级回读 `mov 0xd(%eax),%eax`（技巧14/32 混合形，具名 len 局部槽
    // 反产 -0x10 存取对）。
    MSG_SAVE_GAME_OPTION_1* m =
        reinterpret_cast<MSG_SAVE_GAME_OPTION_1*>(&msg);
    // [P4-p167 符号性] ORIG callee 直证 get_int(unsigned int&)（m_optionSize 本就
    //   声明为 unsigned int，PacketTypes.h:1308）——只换 cast 目标类型，地址算式不变。
    if (!packet.get_int(m->m_optionSize) ||
        m->m_optionSize >= 0x49)
        return LineFunc(0x2860, __PRETTY_FUNCTION__, 0, 0);
    // ORIG 0x081ceaef：len 先经 %edx 保护（`mov %eax,%edx`），buf 计算覆写
    // %eax（`mov -0xc,%eax; add $0x11,%eax`），store 序 0x8(%esp)→0x4(%esp)。
    // 双 register 绑定（值绑 %edx + buf 地址绑 %eax）才锁定该装载序：单绑 %edx 时
    // 4.4 会把值搬回 %eax、把地址挤到 %edx（probe p9/pa r4 实证）。
    // b28d-D0 试形：双实参内联（去 lv/pbuf 具名局部）⇒ 4.4 复现 ORIG 0x081ceaef
    //   `mov %eax,%edx`（值留 callee-saved）+ buf 地址后算的装载序。
    // [b28d-solo-Dispatchers] P319：ORIG 0x081ceaef `mov %eax,%edx; mov -0xc(%ebp),%eax;
    //   add $0x11,%eax`（值→edx、buf→eax）⇒ buf 实参改「整型加法 + 回 cast」
    //   （探针 /tmp/probe_gb2.cpp d 逐条全等；纯指针形恒值留 eax、buf 落 edx）。
    if (!packet.get_binary(
            reinterpret_cast<char*>(&m->m_option),
            (int)m->m_optionSize))
        return LineFunc(0x2862, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}