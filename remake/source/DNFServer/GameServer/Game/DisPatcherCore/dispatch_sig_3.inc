int DisPatcher_ReturnToSelectCharacter::dispatch_sig(CUser* user, PacketBuf& /*packet*/)
{
    // [b28d-solo-DisPatcherCore] P292/P168a 档②：ORIG 尾
    // `movzbl -0x9(%ebp),%eax; xor $0x1,%eax; test %al,%al; je` ⇒ flag 是
    // **bool** 且原码为裸 `if (!flag)`（uchar 会产 test/sete/test 或 xorb 槽化）。
    bool flag = true;                           // ORIG -0x9(%ebp)
    if (user->get_state() <= 2)
    {
        flag = false;
    }
    else if (user->getCurCharacVill() == 7)
    {
        flag = false;
    }
    else
    {
        user->ReturnToSelectCharacList(false);
    }
    {   // 技12 显式块：dtor 正常收尾，免 4.4 EH-ebx 返回缓存（ORIG 尾 call; mov $0,%eax）。
        PacketGuard guard;
        guard.put_header(1, 7);
        guard.put_byte(flag);
        // [b28d-solo-DisPatcherCore] 裸 `if (!flag)`（bool 字段形，免 chk 局部槽化）。
        if (!flag)
        {
            guard.put_byte(0x13);
        }
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
