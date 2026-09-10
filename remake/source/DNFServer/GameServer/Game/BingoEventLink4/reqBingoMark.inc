int BingoEvent::reqBingoMark(StackableAction::input_param& param)
{
    if (param.m_user == 0)
    {
        return 0x13;
    }
    if (param.m_item == 0)
    {
        return 0x13;
    }
    // [b23S3C] ORIG 0x080c9ff6 `call getBingoData` 共 8 次（+0x39/+0xc4/+0xd7/+0x10b/
    // +0x129/+0x13b/+0x172/+0x1ee），无 `data` 具名指针槽 ⇒ P48 每用点重取；
    // 字段访问一律经 BingoData* 出单条 disp 形（`movb $0x1,0x6(%eax)`/`movzbl 0x4(%eax)`/
    // `mov %dl,0x4(%eax)`）。calNewValue 的 &m_eventId ORIG 落 %ebx 跨 operator-> 存活
    // （P197②），残余记 N。
    // +0x04（连线数，signed char）> 2 即已达 3 次标记上限。
    if (static_cast<signed char>(
            static_cast<BingoData*>(param.m_user->getBingoData())->m_pad4) > 2)
    {
        return 0x7;
    }
    // CStackableItem +0x3a0 = m_vec3a0（vector<int>），第 0 项为请求标记号（推断）。
    // [b23S3C] ORIG `add $0x3a0,%eax; mov %eax,(%esp)` 直落 operator[] 的 this 实参，
    // 无 itemVec 具名槽（P0/P48）⇒ 索引表达式内联；同时少一槽使 PacketGuard 帧基
    // 由 -0x30 回到 ORIG 的 -0x2c。
    int markNo = reinterpret_cast<CStackableItem*>(param.m_item)->m_vec3a0[0];
    if (markNo < 1 || markNo > 0x32)
    {
        return 0x17;
    }
    int randValue = m_bingo->getBingoRandomVaule(markNo);
    if (randValue == 0)
    {
        return 0x13;
    }
    unsigned int oldBoard =
        static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId;
    int newLines = m_bingo->calNewValue(
        static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId, randValue);
    if (static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId != oldBoard)
    {
        static_cast<BingoData*>(param.m_user->getBingoData())->m_needSave = true;
        int diff = newLines - static_cast<signed char>(
            static_cast<BingoData*>(param.m_user->getBingoData())->m_pad4);
        if (diff > 0)
        {
            // b28c-L10 判词（N，P209 族）：ORIG 0x080ca37c clamp 段为
            // `mov -0x10(%ebp),%eax; cmp $0x3,%eax; jle` + `mov $0x3,%eax;
            // mov %eax,-0x10(%ebp)`（载寄存器）；4.4 恒 mem 直比
            // `cmpl $0x3,-0x10(%ebp); jle` + `movl $0x3,-0x10(%ebp)`。
            // 探针 /tmp/l10_probe10/11/12.cpp 十二形全败：仅「unsigned 变量 +
            // (int) cast」可载寄存器，但外层条件码变 je/jbe（ORIG 为 jle）。
            // [SweepBingoEventLink4] 追加探针 /tmp/probe_clamp{,2,3}.cpp 三十余形复核：
            // 指针形 `int* p=&diff; if(*p>3)`（fI）可出载寄存器比较，但 store 恒折
            // `movl $0x3,(%eax)`；三目 `diff = diff>3?3:diff;` 产 `cmovle`（本 TU
            // 未路由 -march=i486）；ORIG 的 `mov $0x3,%eax; mov %eax,-0x10(%ebp)`
            // 共享 store 属 N76/N97 跨版本形 ⇒ N 成立。
            // 语义等价（diff>0 已保证非负）。
            if (diff > 3)
            {
                diff = 3;
            }
            static_cast<BingoData*>(param.m_user->getBingoData())->m_pad4 =
                static_cast<char>(newLines);
            for (int i = 0; i < diff; ++i)
            {
                sendOneMatchLineReward(*param.m_user);
            }
        }
    }
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1c3);
    packet.put_byte(randValue);
    packet.put_int(static_cast<int>(
        static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId));
    packet.finalize(true);
    param.m_user->Send(packet);
    return 0;
