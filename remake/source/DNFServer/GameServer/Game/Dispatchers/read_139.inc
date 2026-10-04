// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081ceda4 orig/ours=51/54 ndiff-blocks=6
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax | mov    -0xc(%ebp),%eax
int Dispatcher_SaveGameOption_2::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081ceda4：同 SaveGameOption_1 混合形——typed 指针槽 -0xc、
    // get_int 实参 add $0xd、len typed 单级回读 `mov 0xd(%eax)`。
    MSG_SAVE_GAME_OPTION_2* m =
        reinterpret_cast<MSG_SAVE_GAME_OPTION_2*>(&msg);
    // [P4-p167 符号性] 同 SaveGameOption_1：ORIG callee = get_int(unsigned int&)。
    if (!packet.get_int(m->m_optionSize) ||
        m->m_optionSize >= 0xbd)
        return LineFunc(0x28bc, __PRETTY_FUNCTION__, 0, 0);
    // [b28d-solo-Dispatchers] P0：ORIG 无 lv/pbuf 槽（`mov 0xd(%eax),%eax;
    //   mov %eax,%edx; mov -0xc(%ebp),%eax; add $0x11,%eax; mov %edx,0x8(%esp);
    //   mov %eax,0x4(%esp)`）⇒ 删具名局部改内联实参（右→左求值先 len 后 buf）。
    // [b28d-solo-Dispatchers] P319：buf 实参整型加法 + 回 cast（探针 probe_gb2 d）。
    if (!packet.get_binary(
            reinterpret_cast<char*>(&m->m_option),
            (int)m->m_optionSize))
        return LineFunc(0x28bd, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}