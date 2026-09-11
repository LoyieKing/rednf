// ----------------------------------------------------------------------------
// 公告更新（含物品条件校验 + 广播到 DB 队列）
// ----------------------------------------------------------------------------
void CSharedServerMessageManager::UpdateServerMessage(CUser* pUser, int nIndex,
                                                      char* pMsg, char* pMsg2, int nEndTime)
{
    // [b28d-solo-CSharedServerMessageManager 判词 · AE] ORIG 0x08600ec0（fresh 383=383，
    // TU near=0 diff=0，p167 50/50 CLEAR）。原 N93 判词（pCur 槽 -0x1c/-0x24 方向）作废：
    // 根因 ①P292/P214① 正极性 if/else（IsOnIndex 直判，消 xor 段）；②P259/P226 内联实参
    // （Acquire 直落 CStreamGuard ctor、GetUID 直落 operator<<，消 pStream/nUID 槽）；
    // ③P251/P279 `switch(nMapped)`（寄存器比较链，叶块序 3→0→2 按 ORIG 地址序）；
    // ④P211/P254 字段同型化（stSERVER_MESSAGE/SIG_UPDATE_SERVER_MESSAGE 的 m_lenMsg/m_lenMsg2
    // 改 unsigned int + 显式 cast ⇒ LHS-first 直存）；⑤P221 全表达式临时链（trace 槽 -0x2c）。
    stSERVER_MESSAGE* pCur = getCurMessage(nIndex);
    if (pCur == 0)
    {
        // [b28d-S112] P292/P214①：ORIG 0x08600f? 为 `call IsOnIndex; test %al,%al;
        // je <trace 岛>`（无 xor $1）⇒ 原码正极性 `if (IsOnIndex) { body } else { trace }`；
        // 负极性 `if (!f()){trace;return;}` 恒多 xor 段并反置块布局。
        if (m_adapter.IsOnIndex(nIndex))
        {
            int nKind = m_adapter.GetShareKindWithIndex(nIndex);
            if (nKind == -1)
                return;

            stSERVER_MESSAGE local;
            local.m_kind = (unsigned char)nKind;
            local.m_index = (unsigned char)nIndex;
            local.m_lenMsg2 = (unsigned int)strlen(pMsg2);
            strncpy(local.m_msg2, pMsg2, local.m_lenMsg2 + 1);
            local.m_lenMsg = (unsigned int)strlen(pMsg);
            strncpy(local.m_msg, pMsg, local.m_lenMsg + 1);
            local.m_endTime = nEndTime;

            m_event.m_vecMsg.push_back(local);
            ++m_event.m_count;
            pCur = &local;
        }
        else
        {
            // [b28d-S112] P221/P227：ORIG trace 槽 -0x2c 浅于 guard 槽 -0x34（互斥路径
            // 槽方向）；具名对象 4.4 恒反，改全表达式临时链复现 ORIG 槽深。
            cMyTrace(__PRETTY_FUNCTION__, 0x126, 0)(
                "SERVER_MESSAGE : NOT EXIST INDEX and NOT EXIST DATA");
            return;
        }
    }
    else
    {
        // [b28d-S112] P211/P254：字段 unsigned int + 显式 (unsigned int) cast ⇒ LHS-first
        // 直存（ORIG 0x0860106f `mov -0x1c(%ebp),%edx; mov %eax,0x4(%edx)`）；无 cast 恒
        // RHS-first + eax→edx 中转（探针 /tmp/gs_probe3.cpp a1/a2）。
        pCur->m_lenMsg = (unsigned int)strlen(pMsg);
        strncpy(pCur->m_msg, pMsg, pCur->m_lenMsg + 1);
        pCur->m_lenMsg2 = (unsigned int)strlen(pMsg2);
        strncpy(pCur->m_msg2, pMsg2, pCur->m_lenMsg2 + 1);
        pCur->m_endTime = nEndTime;
    }

    if (!IsSetServerMessage())
        setLoadingComplete();

    int nMapped = m_adapter.GetMappedEventID(pCur->m_index);
    int nSlot = 0;
    // [b28d-S112] P251/P279：ORIG 0x086010c6 段为 `mov -0x18(%ebp),%eax; cmp $0x2,%eax;
    // je …; cmp $0x3,%eax; je …; test %eax,%eax; je …; jmp`（寄存器多路比较链，无内存直比）
    // ⇒ 源形是 `switch (nMapped)`；if-else 链在 4.4 恒折 `cmpl $imm,mem`（P286）。
    // 叶块排放顺序按 ORIG 地址序 = case 书写顺序（P46）：3 → 0 → 2。
    switch (nMapped)
    {
    case 3:
        nSlot = pUser->getCurCharacInvenR()->check_item_exist(0x28d3aa);
        if (nSlot == -1)
            return;
        if (!pUser->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, nSlot, 1,
                                                      eItemDelReason_3, true))
            return;
        if (!pUser->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_INVEN, nSlot))
            return;
        break;
    case 0:
        nSlot = pUser->getCurCharacInvenR()->check_item_exist(0x28d3aa);
        if (nSlot == -1)
            return;
        if (!pUser->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, nSlot, 1,
                                                      eItemDelReason_0x1d, true))
            return;
        if (!pUser->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_INVEN, nSlot))
            return;
        break;
    case 2:
        nSlot = pUser->getCurCharacInvenR()->check_item_exist(0x1d9d);
        if (nSlot == -1)
            return;
        if (!pUser->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, nSlot, 1,
                                                      eItemDelReason_3, true))
            return;
        if (!pUser->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_INVEN, nSlot))
            return;
        break;
    }

    // [b28d-S112] P259/P0：ORIG 0x086012bd 无 pStream 具名局部（Acquire 返回值经 %eax
    // 直落 CStreamGuard ctor 实参）；nUID 亦无槽（`call GetUID; mov %eax,%ebx` 跨 operator*
    // 保活，P226 内联实参）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("SharedServerMessageManager.cpp", 0x16c),
                       true);
    **guard << 0x16c;
    **guard << (int)pUser->GetUID();

    SIG_UPDATE_SERVER_MESSAGE* pOut = guard->GetInBuffer<SIG_UPDATE_SERVER_MESSAGE>();
    pOut->m_index = pCur->m_index;
    pOut->m_server = G_CEnvironment()->m_field378;
    if (pCur->m_kind == 0)
        pOut->m_channel = G_CEnvironment()->get_channel_no();
    else if (pCur->m_kind == 1)
        pOut->m_channel = 0;
    pOut->m_kind = pCur->m_kind;
    pOut->m_lenMsg2 = pCur->m_lenMsg2;
    pOut->m_lenMsg = pCur->m_lenMsg;
    strncpy(pOut->m_msg, pCur->m_msg, pOut->m_lenMsg + 1);
    strncpy(pOut->m_msg2, pCur->m_msg2, pOut->m_lenMsg2 + 1);
    pOut->m_endTime = pCur->m_endTime;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}