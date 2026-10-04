// ==== Dispatcher_ObjectBringUp::process (ORIG 0x081e14d4) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e14d4 orig/ours=137/144 ndiff-blocks=26
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x74,%esp | mov    0x10(%ebp),%eax | mov    %eax,-0x18(%ebp)
int Dispatcher_ObjectBringUp::process(CUser* user, MSG_BASE& msg,
                                      ParamBase& param)
{
    stObjectBringUp& out = static_cast<stObjectBringUp&>(param);

    EventClassify::CEventScriptMng* scriptMng = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    EventClassify::CObjectBringUp* entity =
        static_cast<EventClassify::CObjectBringUp*>(scriptMng->get_event_entity(0x19b));
    if (entity == 0)
    {
        out.m_4 = 0xb2;
        return 0;
    }

    if (!entity->is_eventing())                             // vtable+0x08 虚槽直调（P5）
    {
        out.m_4 = 0xb2;
        return 0;
    }

    const int err = check_error(user, msg);
    out.m_4 = err;
    if (err != 0)
    {
        return 0;
    }

    const unsigned short slot =
        reinterpret_cast<const MSG_OBJECT_BRING_UP&>(msg).m_field0e;
    const Inven_Item slotItem =
        user->getCurCharacInvenR()->GetInvenSlot(1, static_cast<int>(slot));
    const unsigned int itemIdx = static_cast<unsigned int>(slotItem.m_addInfo);

    if (G_CDataManager()->find_item(static_cast<int>(itemIdx)) == 0)
    {
        out.m_4 = 0x11;
        return 0;
    }

    // ORIG 0x081e15e4: materialOk = entity->verifyMaterialItemByRule(user, itemIdx)
    const bool materialOk = entity->verifyMaterialItemByRule(user, itemIdx);
    if (!materialOk)
    {
        out.m_4 = 0x17;
        return 0;
    }
    user->incCurUseBringUpMaterialCount();                  // ORIG 调用点 0x0822fac4
    out.m_a = user->getCurUseBringUpMaterialTotal();        // ORIG 调用点 0x08110d12

    user->getCurCharacInvenW()->delete_item(
        static_cast<INVEN_TYPE>(1), static_cast<int>(slot), 1,
        static_cast<eItemDelReason>(3), true);

    // +0x0c 处 vector<int>：sendBringUpReward 输出（每轮消费 4 个 int）
    std::vector<int>& rewards = out.m_items;
    // ORIG 0x081e1660: rewardOk = entity->sendBringUpReward(user, rewards, out.m_b)
    const bool rewardOk = entity->sendBringUpReward(user, rewards, out.m_b);

    if (!rewardOk)
    {
        out.m_4 = 1;
        return 0;
    }

    out.m_8 = slot;
    return 0;
}