void Dispatcher_ExpertExtraction::send(CUser* user, ParamBase& param)
{
    stExpertExtraction& p = static_cast<stExpertExtraction&>(param);
    if (p.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a0, (unsigned char)p.m_4);
        return;
    }

    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1a0);
    guard.put_byte(1);
    guard.put_short((int)p.m_8);

    // [round5 A01 · B 轴（循环变量符号性，Main 派单②）] ORIG 0x081d9700 槽图：
    //   count=-0x18（uint，`size()/3` magic 除法）、索引累加器=-0x10（uint）、
    //   循环变量=-0xc（int）；尾测 0x081d983d..0x081d9848
    //   `mov -0xc(%ebp),%eax; cmp -0x18(%ebp),%eax; setl %al; test; jne` ⇒ **setl=有符号**
    //   （P196 前置已过）⇒ i 必须 `int` + `(int)count`；索引走独立累加器（ORIG 每元素 post-inc）。
    const unsigned int count = (unsigned int)(p.m_list.size() / 3);
    guard.put_byte((int)count);
    unsigned int idx = 0;                       // -0x10
    for (int i = 0; i < (int)count; ++i)        // -0xc（setl 有符号序）
    {
        guard.put_short(p.m_list[idx++]);
        guard.put_int(p.m_list[idx++]);
        guard.put_int(p.m_list[idx++]);
    }
    guard.finalize(true);
    user->Send(guard);
