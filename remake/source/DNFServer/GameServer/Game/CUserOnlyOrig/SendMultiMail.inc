// ---------------------------------------------------------------------------
// CUser::SendMultiMail(bool certified) T 0x0868a2dc sz0x1d9
// 报告：docs/class_func_reports/CUser/SendMultiMail.md
// 0x8e246 MSG_MULTI_MAILBOX_SEND 块（m_pad8e101+0x145 视图）：name@+0x11、
// money@+0x2f、count@+0x33、条目 stride 0xb@+0x37（itemId 首字段）。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0868a2dc：p167=CLEAR，gap=-9；语义完成、byte 层帧槽/物化族封顶。
// [Lane6 2026-09-10 P183 一试] 删 `char* block` 缓存实测**反劣**（140→149 vs ORIG 128）：
// 4.4 对 `m_pad8e101 + 0x145` 恒发两段 `add $0x8e101,%eax; add $0x145,%eax`（N1 常量
// 折叠跨版本差），且每次重算抬高循环区寄存器压力 ⇒ 循环体形态整体错位；已回滚为具名
// 缓存（ORIG 单 add 无可择源形）。
void CUser::SendMultiMail(bool certified)
{
    // [A06 2026-09-10 负结论·已回滚] ORIG 0x0868a2xx 每处各自重算
    // `mov 0x8(%ebp),%eax; add $0x8e246,%eax`（无 block 指针槽），但 4.4 对
    // `m_pad8e101 + 0x145` 内联形发**两条** add（0x8e101 + 0x145 不折）⇒ 129→138 反劣；
    // 4.4 保持具名 `char* block` 缓存形最优（P183 的 4.4 反例）。正解需 `this+0x8e246`
    // 折成单 add 的具名成员，属头改动 ⇒ 只报告。
    // [H6 2026-09-11 单旋钮·P183 4.4 正解] ORIG 0x0868a2dc 段每处各自重算块基址
    // `mov 0x8(%ebp),%eax; add $0x8e246,%eax`（**单常量 add**，无 block 具名槽）。前轮两试
    // 均败在取址形：`m_pad8e101 + 0x145` 被 4.4 拆成两条 add（$0x8e101 + $0x145）。本次改
    // `(char*)this + 绝对常量`：this 直接加常量，4.4 前端折成单条 add（形同 ORIG）。
    // [DualCUOO 2026-09-09 双判据 gap] ORIG 0x0868a2e6 先 `mov 0xc(%ebp),%eax;
    // mov %al,-0x3fac(%ebp)`（bool 规范化溢出槽）再 `movzbl -0x3fac; xor $1; test;
    // je 0868a33d`，且 0868a2fd 的 SendCmdErrorPacket(0x13c,0x87)+memset 是**if 分支**
    // ⇒ 源码是 `if (!certified) {…} else {主体}`，分支体顺序按 ORIG 交换（行为等价）。
    if (!certified) {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x87);
        memset(m_multiMailMsgData, 0, sizeof(m_multiMailMsgData));
    }
    else {
        MSG_CHECK_MAIL_RECEIVER check;
        check.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_2>(
            *reinterpret_cast<MSG_MULTI_MAILBOX_SEND*>(m_multiMailMsgData));
        int rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(this, reinterpret_cast<MSG_MULTI_MAILBOX_SEND*>(m_multiMailMsgData)->m_title11, &check);
        // [H6 2026-09-11 单旋钮·否定分支在前的块序] ORIG 0x0868a3a4 段 = `je <循环头>` +
        // **直落**失败块（SendCmdErrorPacket 后 `jmp <尾 memset>`），循环紧随其后 ⇒ 原码是
        // `if (rc != 0) { error } else { 循环 }`（与 `if (!certified)` 同一否定优先风格）。
        // 旧 `if (rc == 0) {循环} else {error}` 令 4.4 产 `jne <循环后失败块>` + 循环尾多一跳。
        if (rc != 0) {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, static_cast<unsigned char>(rc));
        }
        else {
            bool fatigueGift = false;
            // [DualCUOO 2026-09-09 双判据 gap] ORIG 0x0868a474 循环条件每轮重读
            // `movzbl 0x8e279(%eax),%eax`（= block[0x33]，无 count 缓存）；
            // 且 0x0868a3cd / 0x0868a442 两次重算 `mov 0xd(%eax),%eax`（itemId 无缓存，P183）。
            // [H6 2026-09-11 单旋钮·具名成员数组下标] ORIG 0x0868a471 `movzbl 0x8e279(%eax),%eax`
            // / 0x0868a45x `mov 0x8e275(%eax),%eax` 都是**单条 disp 装载**（基址+常量由后端折）；
            // `(char*)this + K` 形只能产 `add $K,%eax; mov (%eax)` 两条。改走具名数组成员
            // `m_pad8e101[0x178]`（char[0x2ec] @+0x8e101 ⇒ +0x8e279）与 `&m_pad8e101[0x174]`
            // （⇒ +0x8e275）即复现单 disp。
            MSG_MULTI_MAILBOX_SEND* mm = reinterpret_cast<MSG_MULTI_MAILBOX_SEND*>(m_multiMailMsgData);
            for (int i = 0; i < static_cast<int>(mm->m_count33); ++i) {
                // [H6 2026-09-11 单旋钮·短路条件复原] ORIG 0x0868a3f8 段为 `call IsAvatarItem;
                // xor $1,%eax; test %al,%al; je <false 岛 0x868a41c>` 后接 `call get_rarity;
                // cmp $1,%eax; jg <同一 false 岛>`，false 岛产 `mov $0x0,%eax` 再 `test; je`
                // ⇒ 原码是**短路合条件** `if (!IsAvatarItem() && get_rarity() <= 1)`。
                // 旧三层 if/else 赋 `skip` 产 4.4 无 xor 形 + 额外 skip 槽（-0x15）。
                CItem* item = G_CDataManager()->find_item(
                    mm->m_attach[i].m_itemIdx);
                if (item != 0) {
                    if (!item->IsAvatarItem() && item->get_rarity() <= 1) {
                        fatigueGift = true;
                    }
                }
                if (mm->m_attach[i].m_itemIdx == 0) {
                    fatigueGift = true;
                }
                IncrementTradeGold(
                                   static_cast<int>(mm->m_gold2f),
                                   static_cast<int>(fatigueGift));
            }
        }
        memset(m_multiMailMsgData, 0, sizeof(m_multiMailMsgData));
    }
}