// [b28d-solo-InterDispatcher] P226 语句表达式复现 ORIG 0x084e42e0 的 setle 物化直测
//   （`cmp $0x2,%eax; setle %al; test %al,%al; je`；具名 bool 局部 4.4 恒落槽 +2）。
int Inter_UpdatePurchaseCashItemBonus::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 0x084e42e0：state 检查 setle 布尔物化；pPkt 检查后才落槽 -0xc；
    // *recv 实参先于链式 call（R2L，ORIG +0x35 mov (%eax),%ebx 跨双 call）。
    if (({ user->get_state() <= 2; }))
        return 0;                                // ORIG +0x1c mov $0; jmp end
    Inter_UpdatePurchaseCashItemBonusPktView* pPkt =   // ORIG +0x26 物化 -0xc
        reinterpret_cast<Inter_UpdatePurchaseCashItemBonusPktView*>(data);
    if (pPkt->m_0 != 0)                          // ORIG +0x30 test %eax,%eax; je 尾岛（body fall-through 在前，技巧17）
    {
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->send_purchase_bonus_mail(user, pPkt->m_0);
    }
    return 0;
}