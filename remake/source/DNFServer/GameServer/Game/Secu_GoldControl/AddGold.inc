// ORIG 0x0828757c。**判词：AE（61/61，diff_lines=0）**。
// 手法：①case 0xe 置 switch 首（块物理序对齐，见内注 P324）；②makeRequest 三实参
// 直写进调用（删具名 accId/uid），令 4.4 自然升格 callee-saved `push %esi/%ebx`
// —— 与 ORIG 0x082875c1 `lea 0xc8(%eax),%esi` / 0x082875d4 `mov %eax,%ebx` 同形
// （旧形两局部各占 4B 栈槽、帧 0x28 vs ORIG 0x10）。
void Secu_GoldControl::AddGold(unsigned int value, eMoneyAddReason reason)
{
    CheckDate();
    switch (reason)
    {
    // [P324 叶块书写序] ORIG 0x0828757c 块物理序 = MailGold(0x28) → Import(0x81)
    // → Auction(0x95) → Money(0xa7)，即 case 0xe 在源码中排最前（4.1 按 case
    // 声明序 emit 块）；旧形把 Import 组写最前 ⇒ 三块整体位移（ndiff=0 但 diff）。
    case 0xe:
        MailGold(value, false);
        {
            // [P52/P134 实测] ORIG 0x0828757c 以 `push %esi/%ebx` 保活实参跨调用
            // （无 -0x10/-0xc 栈槽）⇒ 删具名 accId/uid 内联进调用，让 4.4 自然升格
            // callee-saved。旧形两局部各占 4B 槽且帧 0x28 vs ORIG 0x10。
            DB_CheckCharacName::makeRequest(m_pUser->GetUID(), m_pUser->get_acc_id(),
                                            m_mailCharName, false);
        }
        return;
    case 0:
    case 3:
    case 4:
    case 5:
    case 10:
    case 15:
    case 0x1a:
        AddImportMoney(value);
        break;
    case 1:
    case 2:
        return;
    case 0x12:
        AddAuctionMoney(value);
        break;
    default:
        break;
    }
    AddMoney(value);
    CheckMoneyRange();
}