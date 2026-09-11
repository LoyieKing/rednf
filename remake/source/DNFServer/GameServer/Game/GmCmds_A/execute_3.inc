// ORIG execute 0x084aaa00：itemId==-1 先开通 PC 房特权；随后发放物品并在
// 无强化属性时随机投一次强化投资，最后日志 + 物品更新包。
// [b28d-47] verdict=DIFF(routed) [N82/P112] ORIG 0x084aaa00 orig/ours=187/180
//   残差：全帧槽号平移 + 寄存器角色差（跨版本分配）。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=40 [N82] p167 CLEAR(30/30)；limit/stackable/
// space/invest 槽位差 16，语义完成（嵌套 if 拆 && 物化、GetUser 内联、死守卫照抄）。
// [b28d-solo-2] verdict=DIFF：真缺口 = ①三处早退岛未复现（ORIG +0x2aa nop;jmp /
// +0x2ad nop;jmp / +0x2b0 nop 三岛 + 正常路径尾 jmp，共 7 条）⇒ 负条件 goto 尾
// 连续空标签（P223 多岛形，标签序=守卫序）；②SendUpdateItemList 必须在第二个
// slot 死守卫**之后**（ORIG `js +0x2b0` 命中即返回）；③trace 改全表达式临时链
// （P227/P328⑩，ORIG 三串实参先于 cMyTrace ctor 求值）。修后 orig/ours=187/187，
// 残余 2 条：investAmplifyOption 实参装载序（ORIG 右→左逐个压栈、4.4 先物化 arg0
// 到 %edx，P75 跨版本，探针 p3/p4 五形不翻）。
// [b29-Lane21] N7 封顶（446 可救，已上报 Main）：ORIG `mov (%eax),%edx; mov -0x18(%ebp),%edx;
//   0xc(%esp)…` vs 4.4.7 `%ecx/%edx` 角色互换（间接 call 目标寄存器族，P230 补）。
// [round5-A20 复核·N126] fresh .o 184/184、判据⑪ PURE SHIFT c=0x0（frame 96=96、
//   槽集全等）⇒ 残差纯为 N7 间接调用目标寄存器角色（含 investAmplifyOption 实参
//   装载序，P75）⇒ 与 CAddItemByMail 同族封顶。
// [L1FreeTop12.S2 2026-09-11] §6.5 复核：ORIG 唯一串 `"%s:'%s' GM Mode Add Item(%d)."`
//   （0x084aac6c，实参 0x4(%esp)）与源码全表达式 trace 链一致（变参序
//   NumberToString→getCurCharacName→slot 对齐报告 +0x26c 段）；§6.5.1 enc 0 命中。
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
}