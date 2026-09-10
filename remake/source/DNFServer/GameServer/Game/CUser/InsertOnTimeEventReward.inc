bool CUser::InsertOnTimeEventReward(unsigned int itemNo, unsigned int itemCount)  // b28c-L5 SIGFIX（P174）
{
    // b28d-solo-CUser p167：ORIG 0x08681c30 内联 rep stos（我方 EXTRA memset ×2）。
    char logDir[256] = {0};
    sprintf(logDir, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());

    // 2026-09-10（G3CUser-B，P0 槽-局部律）：ORIG 0x08681cbb `call get_charac_no;
    //   mov %eax,%esi`（characNo **无槽**、%esi 保活）与 0x08681ccd..0x08681cdd
    //   `call get_acc_id; call NumberToString; mov %eax,%ebx`（accStr **无槽**、%ebx
    //   保活）——O0 下具名标量必占栈槽，ORIG 无槽 ⇒ 原码把两调用直接写在实参位
    //   （cdecl 右→左：`%u` 的 get_charac_no 先算、`%s` 的 NumberToString 后算，
    //   与 ORIG 0x08681cbb→0x08681cdd 次序一致）。删我方 characNo/accStr 两具名量
    //   （其 store/load 另致帧 0x2dc→0x2ec、+0x10）。
    {
        CMyFileLog log(
            "bool CUser::InsertOnTimeEventReward(unsigned int, unsigned int)",
            0x869f);
        log(logDir, "On Time Event:InsertOnTimeEventReward(%s, %u)",
            NumberToString(get_acc_id(), 0), get_charac_no(-1));
    }

    if (itemNo == 0)
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x120);
    packet.put_byte(1);
    int pktIndex = packet.get_index();   // ORIG 记录后未再使用
    (void)pktIndex;
    packet.put_byte(1);

    // 2026-09-10（G3CUser-B，语句位置轴）：ORIG 0x08681d98 的 find_item 检查位于
    //   `put_byte(1)`（0x08681d76）与 Inven_Item 默认构造（0x08681daa）**之间**，
    //   而 put_header/put_byte/get_index/put_byte 四步在 0x08681d3a..0x08681d76 先行；
    //   我方原先把本检查放在 PacketGuard 构造之前 ⇒ 整段顺序与 ORIG 逆置。位置照抄。
    //   ORIG 0x08681da0 `cmpl $0x0,-0x20(%ebp); je 0x0868201c`（返回 0，不发包）。
    if (G_CDataManager()->find_item((int)itemNo) == 0)
        return 0;

    {
        Inven_Item item;                 // 默认构造（ORIG ctor 0x080cb854）
        item.m_addInfo = (int)itemNo;    // +0x02 预填 item index（ORIG -0x88 写）
        // b28d-solo-CUser p167：ORIG 0x08681c30 直接读 `dm+0xc` 成员
        //   （`CItemList::create_item(*(CItemList**)(dm+0xc), ...)`，无 getItemList
        //   调用；我方 EXTRA ×1）⇒ 改具名成员访问。
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)0, item, (int)itemCount);

        bool give =
            (item.m_addInfo != 0) || (item.get_add_info() != 0);
        if (give)
        {
            int slot = getCurCharacInvenW()->insertItemIntoInventory(
                item, (eItemAddReason)0xd, true, true);
            if (slot < 0)
            {
                char title[20];          // strncpy(src, 0x14)，不补零
                const char* sTitle = g_scriptStringManager_.findString(
                    4, "game_server_msg_132", (bool*)0);
                strncpy(title, sTitle ? sTitle : "", 0x14);

                char msg[256] = {0};
                const char* sMsg = g_scriptStringManager_.findString(
                    4, "game_server_msg_133", (bool*)0);
                strncpy(msg, sMsg ? sMsg : "", 0xff);

                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, item, 0, (unsigned int)getCurCharacNo(),
                    msg, (int)strlen(msg), 0, GetServerGroup(), false, false);

                item.reset();
            }
            else
            {
                SendUpdateItem(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
            }

            packet.put_int((int)itemNo);
            packet.put_short((short)itemCount);
        }
    }

    packet.finalize(true);
    Send(packet);
    return 1;
