int Dispatcher_GoblinPadStatus::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x82177ae (81 insn)：状态字节 0=未激活 / 1=失败≥3 / 2=已认证 / 3=未认证
    // ORIG：isActivate/getFailCnt/isCertified 各前均重新 getPad()（不缓存）
    unsigned char status = 0;
    if (user->getPad()->isActivate())
    {
        if (user->getPad()->getFailCnt() >= 3)
        {
            status = 1;
        }
        else if (user->getPad()->isCertified())   // ORIG 0x82177: if/else 双 movb 直存槽
        {
            status = 2;
        }
        else
        {
            status = 3;
        }
    }
    else
    {
        // ORIG 0x821780x：链尾独立 movb $0x0,-0x9(%ebp) 块（4.1 保留，非初始值折叠）
        status = 0;
    }
    PacketGuard response;
    response.put_header(1, 0xc4);
    response.put_byte(1);
    response.put_byte(status);
    response.finalize(true);
    user->Send(response);
    return 0;
