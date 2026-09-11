// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d1d60 orig/ours=93/90 ndiff-blocks=18
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0xc(%ebp) | lea    -0x18(%ebp),%eax
void Dispatcher_ChangeEmotion::send(CUser* user, ParamBase& param)
{
    // [G6·send_12 2026-09-11] ORIG 0x081d1eb4 `jmp 0x081d1eb7` + 0x081d1eb6 `nop`
    //   （user==0 路径落 nop 岛后直落尾部）；4.4 对 `if (user!=0) {…}` 恒 jump-thread
    //   ⇒ P223 复现形 `if (c==e) goto L; body; L:;`（标签须在复合块之外，避免 crosses-init）。
    if (user == 0) goto L_send12_done;
    {
        // [G1Frest 2026-09-10] ORIG 0x081d1d72 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`
        //   ⇒ param 具名引用局部 -0xc（P0 槽往返）；旧码把 param 内联进表达式，
        //   帧少 4B、PacketGuard 槽由 -0x18 漂到 -0x14。
        stChangeEmotionParam& p = static_cast<stChangeEmotionParam&>(param);
        PacketGuard guard;
        guard.clear();
        guard.put_header(1, 0x102);
        guard.put_byte(1);
        guard.put_short(static_cast<int>(p.m_4));

        guard.finalize(true);
        user->Send(guard);

        guard.clear();
        guard.put_header(0, 2);
        guard.put_byte(0);
        guard.put_short(1);
        user->make_basic_info((char*)&guard, '\0');
        guard.finalize(true);
        G_GameWorld()->send_all(guard, user);
    }
    L_send12_done:;
}