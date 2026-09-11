// ORIG 0x0828764a。**判词：AE（69/69，diff_lines=0）**。
// 手法：①if-else 链 → 真 switch（case 声明序 0x11,0xe,0x1a,2，见内注）；②同
// AddGold 的实参内联（ORIG 0x082876a1 `lea 0xc8(%eax),%esi` / 0x082876b4
// `mov %eax,%ebx` 跨 MailGold→makeRequest 保活，无 -0x10/-0xc 栈槽）。
void Secu_GoldControl::SubGold(unsigned int value, eMoneySubReason reason)
{
    CheckDate();
    // [P339/switch 轴] ORIG 0x0828764a 是真 switch 展开 ——
    //   `mov 0x10(%ebp),%eax; cmp $0xe,%eax; je <import>; cmp $0xe,%eax; jg <L2>;
    //    cmp $2,%eax; je <case2>; jmp <money>; L2: cmp $0x11 ...; cmp $0x1a ...`，
    // 块物理序 MailGold(0x3a) → SubImport(0x93) → SubAuction(0xa7) → SubMoney(0xb9)
    // → case2 空块(0xd8: `nop`) ⇒ 源码为 switch 且 case 声明序 = 0x11, 0xe, 0x1a, 2。
    // 旧 if-else 链形：`cmpl $0xe,0x10(%ebp); jne`（4.4 内存直比）、case2 无空块，
    // 且 makeRequest 实参经具名 accId/uid 落 -0x10/-0xc 栈槽（ORIG 无槽）。
    switch (reason)
    {
    case 0x11:
        MailGold(value, true);
        // [P52/P134] 同 AddGold：实参直接内联进调用，让 4.4 自然升格 callee-saved
        // （ORIG 以 esi/ebx 保活 charName/get_acc_id 结果跨 call，无栈槽）。
        DB_CheckCharacName::makeRequest(m_pUser->GetUID(), m_pUser->get_acc_id(),
                                        m_mailCharName, true);
        return;
    case 0xe:
        SubImportMoney(value);
        break;
    case 0x1a:
        SubAuctionMoney(value);
        break;
    case 2:
        return;
    default:
        break;
    }
    SubMoney(value);
    CheckMoneyRange();
}