void CEventCreateDnf::SendReward(CUser* user)
{
    // b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0816426c orig/ours=177/179 ndiff=149。
    // 残差为帧/槽号平移（局部大缓冲 szTitle/szContents 槽基差，P112/N82）与
    // vector/pair STL 内联差（N89）；语义按报告逐条对齐。
    std::vector<std::pair<int, int> > items;                     // ORIG local_30

    // ORIG 0x0816428d：ARAD::Singleton<CreateDnfEventSciprtServer>::Get()；
    // lea 0x4(%eax) → Server @+0x04 的 CreateDnfEventSciprt 基类子对象
    // （b28c-32 已把原成员 m_script 改为首个非主基类，故此处走基类引用）。
    // round1 R19：ORIG 无 server 具名槽（`movzbl 0x14(%eax),%eax; movzbl %al,%ebx;
    // call Get; lea 0x4(%eax),%edx`）⇒ P0/P183 全表达式内联，删具名指针。
    static_cast<CreateDnfEventSciprt&>(*ARAD::Singleton<CreateDnfEventSciprtServer>::Get())
        .GetItemInfo(items, m_reserved);   // ORIG 第二参 = this+0x14 byte

    // ORIG 0x081642a8..0x0816431c：RDARScriptStringManager::findString(4, key, 0) + strncpy
    char szTitle[30];                                            // ORIG -0x4a（0x1d cap）
    strncpy(szTitle,
            g_scriptStringManager_.findString(4, "tw_cb_launching_title_01", 0),
            0x1d);
    char szContents[256];                                        // ORIG -0x158（0xff cap）
    strncpy(szContents,
            g_scriptStringManager_.findString(4, "game_server_msg_81", 0),
            0xff);

    // ORIG 0x08164321：循环构造 10 个 Inven_Item（0x3d 步长）→ 栈数组等价
    Inven_Item mailItems[10];
    // ORIG 0x08164348：MakeSystemMultiMailPostal(items, mailItems, 10)
    WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(items, mailItems, 10);

    // ORIG 0x0816439a：ReqDBSendNewSystemMultiMail(title, items, size, 0, characNo,
    //                  contents, strlen, 0, serverGroup, 0)
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
        szTitle, mailItems, (unsigned int)items.size(), 0,
        (unsigned int)user->getCurCharacNo(), szContents,
        (int)strlen(szContents), 0, user->GetServerGroup(), false);

    // ORIG 0x081643e1..0x081644c3：PacketGuard → put_header(1, 0x258) →
    // byte(1) → byte(size) → [int(first), byte(second)]*size → finalize(true) → Send
    // round1 R19：①ORIG 每个 put_* 前重新 `lea -0x58(%ebp),%eax`（P183 地址重物化）
    // ⇒ 删具名 `pkt` 引用；②循环上界 ORIG 在循环外算一次（`mov %eax,-0x1c`），
    // i 先声明（`movl $0x0,-0x20`）⇒ 逗号声明 i/count 并缓存 size()。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, 0x258);
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->put_byte((int)items.size());
    for (unsigned int i = 0, count = items.size(); i < count; ++i)
    {
        ((InterfacePacketBuf*)&packet)->put_int(items[i].first);
        ((InterfacePacketBuf*)&packet)->put_byte(items[i].second);   // ORIG put_byte（Ghidra 0x08164480 实证）
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
