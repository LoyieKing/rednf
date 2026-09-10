void Dispatcher_MonstercardBind::send(CUser* user, ParamBase& param)
{
    stMonstercardBind& res = static_cast<stMonstercardBind&>(param);
    if (res.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x19e,
                                 (unsigned char)res.m_4);
        return;
    }

    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x19e);
    guard.put_byte(1);
    guard.put_short(res.m_8);
    guard.put_short(res.m_a);
    guard.put_short(res.m_c);

    // ORIG 0x081d9148：`mov -0x1c(%ebp),%eax; add $0x10,%eax; mov %eax,-0x18(%ebp)`
    // 出现在 **size() 调用之前** ⇒ vector 引用的物化点就在这里（技巧13）；
    // 循环内每次 `mov -0x18(%ebp),%eax` 作 operator[] 的 this，下标用**后置 ++**。
    // 实测两形：① `size_t entryTotal = res.m_list.size();` → 4.4 双存
    //   （call 结果先落临时槽再拷入 entryTotal，帧 0x40→0x50、全槽平移 4）；
    //   ② 取合并形：size() 结果只有一个匿名槽（= ORIG -0x2c）+ groups(-0x14)。
    std::vector<int>& list = res.m_list;
    const int groups = (int)(list.size() / 3);
    guard.put_byte(groups);

    size_t idx = 0;
    for (int g = 0; g < groups; ++g)
    {
        guard.put_short(list[idx++]);
        guard.put_int(list[idx++]);
        guard.put_int(list[idx++]);
    }

    guard.finalize(true);
    user->Send(guard);
