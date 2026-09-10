void CUser::add_guild_pvp_result(int v)
{
    PvP_Room* pRoom = (PvP_Room*)GetPVPRoom();
    if (pRoom == 0)
        return;
    int seat = pRoom->get_user_seat(this);
    if (pRoom->IsPvpObserver(seat) != 0)
        return;
    // 2026-09-10 gA（A 组）ORIG 0x0865c985..0x0865c9b3：GetRepeatEvent 返回值无判空
    //   （直接 `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; ...; call *%edx`），
    //   且 guard 为早期返回形 `call; xor $1; test %al; jne END`（P16/P168a①）
    //   ⇒ 删具名 pEvent + 发明判空，改为表达式内联 + 早退。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x21)->IsEventing((CUser*)0))
        return;
    // ORIG 0x0865c9b9..0x0865c9cb：`call; test %eax,%eax; sete %al; test %al; jne END`
    if (get_charac_guildkey() == 0)
        return;
    {
        // 2026-09-10 gA（A 组）真 bug（字段错位轴）：ORIG 0x0865c9d7 构造载荷后
        //   0x0865c9e7 `mov %eax,-0x15(%ebp)`（载荷基址 -0x1f ⇒ **+0xa**）写 guildkey、
        //   0x0865c9ed `mov %al,-0x11(%ebp)`（⇒ **+0xe**）写 point，SendPacket(...,0xf)；
        //   与 shared/packet/include/Packet_Notice_Guild_War_Point_Change.h 的 packed
        //   布局 m_guildKey@+0xa / m_point@+0xe 一致 —— 我方旧写 +0/+4 是错位。
        //   ⚠ 不得为补齐载荷空间引入局部 holder 类：那会发射
        //   `_ZZN5CUser20add_guild_pvp_resultEiEN12NoticeHolderC1/C2Ev` 两个 **ORIG 无**
        //   的符号（实测 missing_in_game 588→590，已回退）。改用真实类型 + 尾随 4B
        //   缓冲承载越界 3B 写（P171 照抄 ORIG 写偏移）。Game/ 头为无字段最小镜像
        //   （sizeof 0xc，真头字段化属 .h 类型轴，另报 Main）。
        Packet_Notice_Guild_War_Point_Change notice;
        unsigned char noticeTail[4];
        *(unsigned int*)((char*)&notice + 0xa) = (unsigned int)get_charac_guildkey();
        *(unsigned char*)((char*)&notice + 0xe) = (unsigned char)v;
        (void)noticeTail;
        // ORIG 0x0865c9fe..0x0865ca1f：代理指针无槽无判空（`mov &s_guild_proxy_mgr,%edx;
        //   mov %eax,0x4(%esp); mov %edx,(%esp); call GetServerProxy; ...; call SendPacket`）
        //   ⇒ 删具名 proxy 与发明判空，改链式内联（P183）。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendPacket((char*)&notice, 0xf);
        send_pvp_record();
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 0x6e);
        ((InterfacePacketBuf*)&packet)->put_byte(v);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        Send(packet);
        m_guildWarPoint = m_guildWarPoint + v;
    }
