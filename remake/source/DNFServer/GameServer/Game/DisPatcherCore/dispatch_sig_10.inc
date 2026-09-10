int DisPatcher_Compound::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(user, static_cast<RestrictCategory::Enum>(1), 9) != false)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1b), 0xd1);
        return 0;
    }
    // ORIG 0x081eaa5c：三 wire 字段是单 blob（base -0x20：get 传址 `lea -0x20,%eax;
    // add $0xd/11/12`，读 `mov -0x13`/`movzbl -0xf`/`movzwl -0xe` disp 直读）——
    // 单 pack(1) 视图复现（SetIP 同法）。
    #pragma pack(push, 1)
    struct CompoundMsgView {
        char m_head[13];          // +0x00..0x0c
        unsigned long m_itemId;   // +0x0d（ORIG +0x84 `get_intERm` ulong& 重载实证）
        unsigned char m_type;     // +0x11
        unsigned short m_count;   // +0x12
    };
    #pragma pack(pop)
    // ORIG blob 基 -0x20；4.4 pack(1) 基 -0x21（1B 对齐缝）、变量级 aligned(4) 反
    // 推远至 -0x24（4 对齐向远离 ebp 取整）——两向均留文本差，取奇基形（帧紧凑）。
    CompoundMsgView msg;
    if (packet.get_int(msg.m_itemId) != true)
    {
        return LineFunc(0x6c3a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_byte(msg.m_type) != true)
    {
        return LineFunc(0x6c3b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_short(msg.m_count) != true)
    {
        return LineFunc(0x6c3d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (msg.m_count == 0)
    {
        return LineFunc(0x6c40,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // P168a-①②：&& 合写在 4.4 产值物化岛（mov$1/$0;test;je 5 行）；ORIG +0x199..
    // 是两段直跳（`cmp $1,%al; je` + bool callee `test %al,%al; je` 裸形）→ 拆嵌套。
    if (msg.m_type != '\x01')
    {
        if (user->CheckItemLock(1, static_cast<int>(msg.m_itemId)))
        {
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1b), 0xd5);
            return 0;
        }
    }
    // ORIG +0x1c8..+0x1f5：右→左求值 count(movzwl 两段→%ecx)、type==1（`cmp $1,%al;
    // sete %al; movzbl %al,%edx` bool 物化经具名量传参，技9）、itemId(→%eax)；
    // 分支 `cmpl $0,-0xc; jle fail岛` ⇒ 成功 guard 块是 fall-through（技17）。
    // ORIG 实参求值右→左：count(movzwl→ecx) 先、type==1(sete 岛→edx) 次、
    // itemId(→eax) 最后——实参内联表达式直产（register asm 强制提前反致
    // movzwl/sete 块序倒置）。
    int local_10 = user->compound_item(static_cast<int>(msg.m_itemId),
                                       msg.m_type == '\x01', msg.m_count, 0x1b);
    if (local_10 > 0)
    {
        PacketGuard local_30;
        local_30.put_header(1, 0x1b);
        local_30.put_byte(0);
        local_30.put_byte(local_10);
        local_30.finalize(true);
        user->Send(local_30);
        return 0;
    }
    // P168a-②直分支形：ORIG `cmpl $0,0xc; je; cmpl $0,-0xc; jne` 两段直跳，
    // && 合写在 4.4 反产值物化岛（mov$1/$0;test;je）→ 拆嵌套。
    if (user != 0)
    {
        if (local_10 == 0)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, static_cast<APSystem::_ActionGroupIndex>(6), 0,
                static_cast<ENUM_PACKETCLASS>(0));
        }
    }
    return 0;
