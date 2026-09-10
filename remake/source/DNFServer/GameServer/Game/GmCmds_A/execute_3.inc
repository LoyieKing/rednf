void CAddItemPollutionCmd::execute()
{
    if (m_itemIdx == -1)
    {
        // ORIG：getCurSec→lea +0x15180 直传，start/end 无栈槽（右值内联）
        WongWork::CHandlePremium::handleSetUserPCRoom(
            GetUser(),
            static_cast<long>(GlobalData::s_systemTime_.getCurSec()),
            static_cast<long>(GlobalData::s_systemTime_.getCurSec()) + 0x15180);
    }

    int limit = 100;
    CStackableItem* stackable =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(m_itemIdx));
    // [b28d-solo-GmCmds_A] P133/P225：ORIG 各条件各自 `je` 直跳统一出口；
    // 合并 `&&` 使 4.4 物化 bool（mov $1/$0 + test）⇒ 拆嵌套 if。
    if (stackable != 0)
    {
        if (stackable->is_stackable())
        {
            if (stackable->getStackableLimit() < limit)
            {
                limit = stackable->getStackableLimit();
            }
        }
    }

    ENUM_ITEMSPACE space;
    // [b28d-solo-GmCmds_A] P0/P167：ORIG 无 user 具名槽，6 处调用点各自重调
    // GetUser()（p167 MISSING GetUser×4）。
    int slot = GetUser()->AddItem(m_itemIdx, limit, static_cast<eItemAddReason>(6),
                                  space, 0);
    // [b28d-solo-2] P223 多岛形：ORIG 三处早退各落独立 nop 岛
    // （+0x2aa nop;jmp / +0x2ad nop;jmp / +0x2b0 nop），4.4 复现形 =
    // 负条件 goto 到尾部连续空标签（标签序=守卫序），正常路径尾 jmp 自动生成。
    if (slot < 0) goto L1;

    {
        // [b28d-solo-GmCmds_A] P0：ORIG 无 invenType/inven 具名槽
        // （invenType 在 %ebx 直传、inven 在 %eax 直传）。
        Inven_Item* ref = GetUser()->getCurCharacInvenW()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), slot);
        if (ref == 0) goto L2;
        if (ref->m_amp.getAbilityType() == 0)
        {
            int type = get_rand_int(4) + 1;
            if (0 < type && type < 5)
            {
                stInvestAmplifyOption_t invest;
                invest.m_amount = type;
                // [b28d-solo-GmCmds_A] P184：ORIG +0x181 二次 `cmpl $0,stackable`
                // 死守卫照抄（P171 不写即少分支）。
                if (stackable != 0)
                {
                    GlobalData::s_itemAmplifier_->investAmplifyOption(stackable, *ref,
                                                                      invest);
                    unsigned char abilityType = 0;
                    unsigned short abilityValue = 0;
                    ref->m_amp.getValues(abilityType, abilityValue);
                    ref->m_amp.assign(abilityType | 0x80, abilityValue);
                }
            }
        }
        // [b28d-solo-2] P184：ORIG +0x1f6 二次 `cmpl $0,slot; js` 死守卫，
        // 命中即返回（跳过 trace 与 SendUpdateItemList）⇒ 早退 goto 第三岛。
        if (slot < 0) goto L3;
        // [b28d-solo-2] P227/P328⑩：ORIG 三串实参在 cMyTrace ctor 之前求值
        // （getCurCharacName→%esi、NumberToString→%ebx，其后才 C1）⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x5b0, 0)("%s:'%s' GM Mode Add Item(%d).",
                                                NumberToString(GetUser()->get_acc_id(), 0),
                                                GetUser()->getCurCharacName(), slot);
        GetUser()->SendUpdateItemList(static_cast<CUser::eSendTarget>(1), space, slot);
    }
L1:;
L2:;
L3:;
