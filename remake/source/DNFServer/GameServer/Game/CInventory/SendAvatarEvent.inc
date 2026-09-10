void CInventory::SendAvatarEvent(int a, int b, int c, int d, int e) const
{
    // b28d-solo-CInventory：249/262，比较链块序 + 槽残差 ⇒ N82 封顶。
    // b28d-37 [N82 封顶，ORIG 0x850?]：bool 物化/槽残差（249/262）。
    if (m_pParent == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x212a, 5);
        trace("CInventory::SendAvatarEven ERROR, m_pParent NULL ");
        return;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("inventory.cpp", 0x212f), true);
    int characNo = m_pParent->get_charac_no(-1);
    **guard << a;
    **guard << (int)m_pParent->GetUID();
    unsigned char b0 = (unsigned char)b;
    // P46/P286：ORIG 0x08508d09 为 `mov 0xc(%ebp),%eax` 后 cmp/je/jg 二分决策树
    // （稀疏 switch 固有形，if 链恒生内存直比线性链）⇒ 改 switch；叶块物理序 = case
    // 书写序（P324）：0x31 / 0x32 / 0x33 / 0x34 / 0x36 / 0x2c3 / 0x31a，default 直落 put。
    switch (a)
    {
    case 0x31:
    {
        char buf[0x53];
        *(int*)(buf + 0) = characNo;
        *(int*)(buf + 4) = b;
        // ORIG 布局（0x08508d5a 起）：[0]characNo、[4]b、[8](char)c、
        // [0xa]d、[0xe](char)e；pad 字节不初始化（ORIG 原样）。
        buf[8] = (char)c;
        *(int*)(buf + 0xa) = d;
        buf[0xe] = (char)e;
        guard->put_binary(buf, 0x53);
        break;
    }
    case 0x32:
    {
        char buf[0x16];
        *(int*)(buf + 0) = characNo;
        *(int*)(buf + 4) = d;
        *(int*)(buf + 8) = e;
        buf[0xc] = (char)b0;
        buf[0xd] = (char)c;
        // ORIG（0x08508dfd/+0x1cf 段）：+0x0e 槽判定用 (byte)c、写 e；
        // +0x12 槽判定用 (byte)b、写 d。结构未初始化字节按 ORIG 原样发送。
        // [判据⑫ 实测两形均反劣，已回退] ①具名局部 v0e/v12 形 d 80→72 但总量 267→273；
        // ②buf 直写先清零形 d 76/总量 273 ⇒ 维持三目直写形（ORIG 的 clr_l=2 疑非本段）。
        *(int*)(buf + 0xe) = GetClearAvatar((unsigned char)c) != 0 ? e : 0;
        *(int*)(buf + 0x12) = GetClearAvatar(b & 0xff) != 0 ? d : 0;
        guard->put_binary(buf, 0x16);
        break;
    }
    case 0x33:
    {
        char buf[0x70c];
        *(int*)(buf + 0) = 1;
        *(int*)(buf + 4) = characNo;
        buf[8] = (char)b0;
        guard->put_binary(buf, 0x70c);
        break;
    }
    case 0x34:
    {
        char buf[7];
        *(int*)(buf + 0) = characNo;
        buf[4] = (char)b0;
        *(short*)(buf + 5) = (short)c;
        guard->put_binary(buf, 7);
        break;
    }
    case 0x36:
    {
        // ORIG GetInBuffer<SIG_AVATAR_CHANGE_STAT>（0x08508ed2 段，sizeof 0x51）：
        // +0x00=b(int ui_id)、+0x04=c(int it_id)、+0x08=(char)d(slot)、
        // +0x09=characNo(int 非对齐 movl)。（参数 e 未用）
        SIG_AVATAR_CHANGE_STAT* s =
            guard->GetInBuffer<SIG_AVATAR_CHANGE_STAT>();
        s->m_uiId = b;
        s->m_itId = c;
        s->m_slot = (unsigned char)d;
        s->m_characNo = characNo;
        break;
    }
    case 0x2c3:
    {
        // ORIG 0x08508f0f：GetInBuffer<SIG_AVATAR_OPTION_CHANGE> 就地写域。
        SIG_AVATAR_OPTION_CHANGE* p =
            guard->GetInBuffer<SIG_AVATAR_OPTION_CHANGE>();
        p->m_uiId = b;
        p->m_field4 = (char)c;
        p->m_characNo = characNo;
        break;
    }
    case 0x31a:
    {
        // ORIG 0x08508f43：GetInBuffer<SIG_AVATAR_COLOR_CHANGE> 就地写域。
        SIG_AVATAR_COLOR_CHANGE* p =
            guard->GetInBuffer<SIG_AVATAR_COLOR_CHANGE>();
        p->m_characNo = characNo;
        p->m_uiId = b;
        p->m_field8 = (short)c;
        p->m_fieldA = (short)d;
        break;
    }
    default:
        break;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
