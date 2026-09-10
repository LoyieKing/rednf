void CUser::SendMultiMail(bool certified)
{
    // [A06 2026-09-10 负结论·已回滚] ORIG 0x0868a2xx 每处各自重算
    // `mov 0x8(%ebp),%eax; add $0x8e246,%eax`（无 block 指针槽），但 4.4 对
    // `m_pad8e101 + 0x145` 内联形发**两条** add（0x8e101 + 0x145 不折）⇒ 129→138 反劣；
    // 4.4 保持具名 `char* block` 缓存形最优（P183 的 4.4 反例）。正解需 `this+0x8e246`
    // 折成单 add 的具名成员，属头改动 ⇒ 只报告。
    char* block = m_pad8e101 + 0x145;                        // +0x8e246
    // [DualCUOO 2026-09-09 双判据 gap] ORIG 0x0868a2e6 先 `mov 0xc(%ebp),%eax;
    // mov %al,-0x3fac(%ebp)`（bool 规范化溢出槽）再 `movzbl -0x3fac; xor $1; test;
    // je 0868a33d`，且 0868a2fd 的 SendCmdErrorPacket(0x13c,0x87)+memset 是**if 分支**
    // ⇒ 源码是 `if (!certified) {…} else {主体}`，分支体顺序按 ORIG 交换（行为等价）。
    if (!certified) {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x87);
        memset(block, 0, 0x1a6);
    }
    else {
        MSG_CHECK_MAIL_RECEIVER check;
        check.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_2>(
            *reinterpret_cast<MSG_MULTI_MAILBOX_SEND*>(block));
        int rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(this, block + 0x11, &check);
        if (rc == 0) {
            bool fatigueGift = false;
            // [DualCUOO 2026-09-09 双判据 gap] ORIG 0x0868a474 循环条件每轮重读
            // `movzbl 0x8e279(%eax),%eax`（= block[0x33]，无 count 缓存）；
            // 且 0x0868a3cd / 0x0868a442 两次重算 `mov 0xd(%eax),%eax`（itemId 无缓存，P183）。
            for (int i = 0; i < static_cast<int>(static_cast<unsigned char>(block[0x33])); ++i) {
                CItem* item = G_CDataManager()->find_item(
                    *reinterpret_cast<int*>(block + 0x37 + i * 0xb));
                if (item != 0) {
                    bool skip;
                    if (item->IsAvatarItem()) {
                        skip = false;
                    }
                    else if (item->get_rarity() > 1) {
                        skip = false;
                    }
                    else {
                        skip = true;
                    }
                    if (skip) {
                        fatigueGift = true;
                    }
                }
                if (*reinterpret_cast<int*>(block + 0x37 + i * 0xb) == 0) {
                    fatigueGift = true;
                }
                IncrementTradeGold(reinterpret_cast<MSG_MULTI_MAILBOX_SEND*>(block)->m_gold2f,  // +0x2f
                                   static_cast<int>(fatigueGift));
            }
        }
        else {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, static_cast<unsigned char>(rc));
        }
        memset(block, 0, 0x1a6);
    }
