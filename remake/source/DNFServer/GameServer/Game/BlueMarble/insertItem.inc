// [b28d-solo-BlueMarble NEAR(N7/N109)] P0 去缓存/register 化 + P235 union 视角后
// 177=177 gap 0。修法：addInfo/msg268/msg269/mailRet 四个具名局部内联
// （ORIG 分别直入 %ebx、直作 strncpy 实参、`test;sete;test;je` 无槽比较）、
// serverGroup/len 纯 register 复现 callee-saved 保活（P231/P248）、characNo
// 内联实参、MSG 载荷改 char[0x145]+union{raw,MSG_MAILBOX_SEND msg} 视角
// （P235；完整类型成员直访问使三字段 store 折叠为 -0x236/-0x232 绝对 disp）。
// 本轮修复 ②：strncpy dst 改 `local_273.raw + 0x11`（数组退化 + 常量偏移）
// 逐条复现 ORIG 0x080d8273 `lea -0x26f; add $0x11`；具名结构成员
// `msg.m_name11` 恒折 `lea 0x11(%eax)` ⇒ 原 N102/N4 判词作废（DIFF→NEAR）。
// 余 N7/N109：%esi/%ebx 角色互换（ORIG serverGroup→ebx、len→esi；4.4 按首次
// 使用恒 serverGroup→esi；声明序交换、声明/赋值分离两形实测均不翻）。
// ORIG 0x080d800e
int BlueMarble::insertItem(int seat, Inven_Item& item, eItemAddReason reason)
{
    // ORIG 0x080d801c `mov 0x7(%eax),%ebx`（Inven_Item::m_addInfo2）直入
    // callee-saved、无 addInfo 槽（P0）⇒ 内联实参。
    G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0, item,
                                              item.m_addInfo2);
    // ORIG 0x080d80xx：链式直调（getCurCharacInvenW 返回值 %edx 直接作
    // this 压栈，无 inven 局部槽——旧缓存形多 1 条 reload）。
    int ret = ((CInventory*)m_users[seat].getUser()->getCurCharacInvenW())
                  ->insertItemIntoInventory(item, reason, true, false);
    if (ret < 0)
    {
        char local_2e[30];
        char local_12e[256];
        // ORIG 0x080d8127/0x080d8162：findString 返回值直作 strncpy 第 2 参
        // （无 msg268/msg269 槽，P0）。
        strncpy(local_2e, g_scriptStringManager_.findString(
                    4, "game_server_msg_268", 0), 0x1d);
        strncpy(local_12e, g_scriptStringManager_.findString(
                    4, "game_server_msg_269", 0), 0xff);
        // ORIG 0x080d819e `mov %eax,%ebx` / 0x080d81b6 `mov %eax,%esi`：
        // serverGroup/len 跨后续调用保活于 callee-saved 且无栈槽
        // （P231/P248 三条件齐备）⇒ 纯 register 提示。
        // [M3S4 实测-3形 · 177=177 NEAR 封顶] 残差仅 %ebx/%esi 角色互换（ORIG
        // serverGroup→%ebx、len→%esi；我方反向）：①len 内联实参（只留 serverGroup
        // register）同互换；②连同旧判词的声明序交换/声明赋值分离共 3 形 ⇒ 4.4
        // callee-saved 分配序差，无源码形可翻。
        register ENUM_SERVER_GROUP serverGroup =
            m_users[seat].getUser()->GetServerGroup();
        register size_t len = strlen(local_12e);
        // ORIG 0x080d822d `test %eax,%eax; sete %al; test %al,%al; je`：
        // mailRet 无槽（P0），比较内联进 if。
        if (WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                local_2e, item, 0,
                (unsigned)m_users[seat].getUser()->get_charac_no(-1),
                local_12e, (int)len, 0xf, serverGroup, false, false) == 0)
        {
            // ORIG 栈槽 local_273（0x145）= MSG_MAILBOX_SEND 载荷（权威布局
            // PacketTypes.h：m_name11@+0x11 / m_itemId39@+0x39 / m_extra3d@+0x3d；
            // 写指令 `mov %eax,-0x236/-0x232(%ebp)` 均 4 字节 ⇒ 字段宽一致）。
            // 载体用 char[0x145]（ORIG 该槽 1 字节对齐：-0x26f(%ebp)）+ 匿名
            // union 视角：完整类型成员直访问让 4.4 折叠 disp（P235），
            // 避免逐处 reinterpret_cast 的 base lea；MSG_MAILBOX_SEND 为
            // pack(1) POD（无 ctor/dtor）⇒ union 平凡、sizeof 0x145/align 1 不变。
            union MailPayload {
                char raw[0x145];
                MSG_MAILBOX_SEND msg;
            };
            MailPayload local_273;
            memset(local_273.raw, 0, 0x145);
            // ORIG 0x080d8273/0x080d8278 `lea -0x26f; add $0x11`：数组退化 + 常量偏移
            // 形（P214①/N102 反例解；`msg.m_name11` 结构成员恒折 disp 成 lea 0x11(%eax)）。
            // raw 为 union 字节视角（+0x11 即 m_name11 起始），偏移由字段自身证明。
            strncpy(local_273.raw + 0x11, local_2e, 0x1d);
            local_273.msg.m_itemId39 =
                static_cast<unsigned long>(item.m_addInfo);  // +0x39 附件物品 id
            local_273.msg.m_extra3d = item.m_addInfo2;       // +0x3d 附件 add_info
            m_users[seat].getUser()->m_historyLog.SendMail(&local_273.msg, 0);
        }
    }
    else
    {
        sendUpdateItemInfo(seat, ret);
    }
}