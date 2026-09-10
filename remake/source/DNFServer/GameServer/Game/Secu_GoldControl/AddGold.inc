void Secu_GoldControl::AddGold(unsigned int value, eMoneyAddReason reason)
{
    CheckDate();
    switch (reason)
    {
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
    case 0x12:
        AddAuctionMoney(value);
        break;
    default:
        break;
    }
    AddMoney(value);
    CheckMoneyRange();
