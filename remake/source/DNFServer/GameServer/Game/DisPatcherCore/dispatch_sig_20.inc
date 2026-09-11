// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 43（ORIG 152 / ours 151），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
int DisPatcher_EnterPVP::dispatch_sig(CUser* user, PacketBuf& packet)

{
    // ORIG 0x081ed6c8 T（报告 DisPatcher_EnterPVP/dispatch_sig.md 全文转写）：
    // get_short(pvpIdx) → 清 flag/seat/buf[9] → get_byte(flag)，flag==1 时
    // get_int(seat)（>0xa→LineFunc）+ get_binary(buf,seat)；
    // GetPvp(idx,user,0)==0 → 错误包(1,0x36)+0+0x16；否则 EnterPVPRoom(user,pvp,0x36,buf)。
    // [G5-DisPatcherCore] 负结论（已回滚）：ORIG 0x081ed6d0 对 pvpIdx 取址为两步
    // `lea -0x1b(%ebp),%eax; add $0xd,%eax`（基址 -0x1b + 0xd ⇒ 地址 -0xe），独立 `short`
    // 局部只产单条 `lea -0xe(%ebp)`。试形 `struct { char m_hdr[0xd]; short m_idx; }
    // __attribute__((packed));` ⇒ 4.4 报 `不能将紧实的字段 m_idx 绑定到 short&`
    // （packed 非对齐成员不可作 get_short(short&) 实参；4.1 同样非法）⇒ 原码不是 packed
    // 成员引用，`-0x1b` 基址的构造形未解出。本件残差仅此 1 条（ours=151 / ORIG=152）。
    // [DPC-SigC 2026-09-11] P161/P311 msg 组件视图：ORIG 0x081ed6d0
    // `lea -0x1b(%ebp),%eax; add $0xd,%eax`（两步取址 ⇒ 视图 base -0x1b、成员 @+0xd
    // = -0xe，视图 sizeof 0xf），而**读取**处 0x081ed8xx 为单条 `movzwl -0xe(%ebp),%eax`。
    // 我方原独立 `short pvpIdx` 只产单条 `lea -0xe(%ebp)`，全 16 差异块皆此型帧偏移差
    // （ORIG 152 vs 我方 151）。用 pragma pack(1)（attribute 形 4.4 报错，G5 已实测；
    // H5 同法落于 SetPVPMapIndex/IncreStatusSigView）：取址走「基址 + 0xd」绕过
    // 「紧实字段不能绑定 short&」，读取走成员值语义（packed 成员读取合法）。
    // 视图占 [-0x1b,-0xd] 后，flag(-0x1c)/seat(-0x20)/buf(-0x29..-0x21) 槽位随之下移对齐。
#pragma pack(push, 1)
    struct EnterPVPMsgView
    {
        char m_hdr[0xd];
        short m_pvpIdx;
    };
#pragma pack(pop)
    EnterPVPMsgView sig;
    if (!packet.get_short(*reinterpret_cast<short*>(
            reinterpret_cast<char*>(&sig) + 0xd)))
    {
        return LineFunc(0x6fc7, __PRETTY_FUNCTION__, 0, 0);
    }
    char flag = 0;              // ORIG movb $0x0,-0x1c
    int seat = 0;               // ORIG movl $0x0,-0x20
    char buf[9] = { 0 };        // ORIG movl/movl/movb 内联清零（-0x29..-0x21）
    if (!packet.get_byte(flag))
    {
        return LineFunc(0x6fce, __PRETTY_FUNCTION__, 0, 0);
    }
    if (flag == 1)
    {
        if (!packet.get_int(seat))
        {
            return LineFunc(0x6fd2, __PRETTY_FUNCTION__, 0, 0);
        }
        if (seat > 0xa)         // ORIG cmp $0xa; jle 通过
        {
            return LineFunc(0x6fd4, __PRETTY_FUNCTION__, 0, 0);
        }
        if (!packet.get_binary(buf, seat))
        {
            return LineFunc(0x6fd7, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    PvP_Room* pvp = G_CGameManager()->GetPvp((int)sig.m_pvpIdx, user, 0);
    if (pvp == 0)
    {
        PacketGuard guard;
        guard.put_header(1, 0x36);
        guard.put_byte(0);
        guard.put_byte(0x16);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    EnterPVPRoom(user, pvp, (ENUM_CMDPACKET)0x36, buf);
    return 0;
}

   int DisPatcher_EnterPVP::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x08232b7e: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
