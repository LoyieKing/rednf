// [b28c-31] N（EH 尾块中继）：ORIG 0x081cc3c4 正常路径析构块后为
// `jmp +0xec; nop` 中继（P63 跨版本 EH 尾布局），4.4 恒 fall-through；余量仅此 2 条。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P63] ORIG 0x081cc2e6 orig/ours=72/69 ndiff-blocks=2
//   ORIG 正常路径尾 jmp+nop 中继岛（4.1 EH 尾块布局），4.4 fall-through/jump-thread 恒消除（P223 尾空标签试形实测无效）
//   首差: jmp    081cc3d2 <+0xec> | nop | nop
void Dispatcher_MailBox_Open::send(CUser* user, ParamBase& param)
{
    // [b28c-31] ORIG 0x081cc2f0：param 指针落槽 -0xc，其后每次 `mov -0xc(%ebp),%eax;
    // mov 0x4(%eax),%eax` 重读（P0/P183 无 err 缓存局部）；裸偏移改具名视图字段。
    stMailboxOpenParam* out = reinterpret_cast<stMailboxOpenParam*>(&param);
    if (out->m_4 != 0x7fffffff)
    {
        if (out->m_4 != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x63, (unsigned char)out->m_4);
        }
        else
        {
            PacketGuard guard;
            guard.put_header(1, 0x63);
            guard.put_byte(1);
            guard.put_short(out->m_8);
            guard.finalize(true);
            user->Send(guard);
        }
    }
    else
    {
        // b28d-solo-Dispatchers: P285 —— ORIG 0x081cc3d1 `jmp +0xec; nop` 体尾中继岛；
        //   4.4 对纯正向 if 恒 jump-thread，加空 else{return;} 即保留体尾 jmp+nop。
        return;
    }
}