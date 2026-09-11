// ============================================================================
// CParty::_onGetItem —— ORIG 0x085b949c
// ============================================================================
void CParty::_onGetItem(CUser* user, unsigned int itemIdx, unsigned int count)
{
    CItem* item = G_CDataManager()->find_item((int)itemIdx);   // -0x14
    if (item == 0)
        return;

    if (((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
            ->is_eventing(0x193))
        ((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
            ->process_add_item_count_db(user, (int)itemIdx, (int)count);

    if (item->is_stackable())
    {
        // ORIG 0x085b951f：`mov -0x14,%eax; mov %eax,-0x10(%ebp)` 第二具名指针
        // 槽（-0x10），后续 vtable+0xc 虚调用经 p（call *%edx 槽形，N7 残差另计）；
        // boxKind 判定为 switch 线性降序树（0x289749→2 / 0x28982d→1 / 0x380→0，
        // 入口 `movl $0,-0xc` 单初始化 + 各 case 单 mov+jmp 合并，0x085b9541..）。
        CItem* p = item;
        if (p->GetItemType() == 0x1a)                // vtable 槽 +0xc
        {
            int boxKind = 0;                            // -0xc
            switch (itemIdx)
            {
            // ORIG body 内存序 0x380@0x85b9560 → 0x28982d@0x85b9569 →
            // 0x289749@0x85b9572（值升序、cmp 降序线性探）⇒ case 按升序书写。
            case 0x380:    boxKind = 0; break;
            case 0x28982d: boxKind = 1; break;
            case 0x289749: boxKind = 2; break;
            default: break;
            }
            CUser::SendRandomBoxStatistic(boxKind, RANDOMBOX_ACTION_CREATE);
        }
    }
}