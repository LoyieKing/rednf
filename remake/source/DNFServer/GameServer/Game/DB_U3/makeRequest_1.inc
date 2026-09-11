// ---- DB_InsertCeraItemHistory::makeRequest ORIG @ 0x0844511a ----
// stream: opcode 0x2b1 | -1 | itemIdx(int) | sender(uint) | receiver(uint)
//         | flag(char 符号扩展)
// [b28c-31] N（EH 尾布局）：ORIG 0x0844523x 的 cleanup 块（CStreamGuard D1 +
// _Unwind_Resume）在函数中段、正常路径 `jmp` 汇入 ret 块；4.4 恒置函数尾
// 并 fall-through（跨版本 EH 布局，P63 族）。
void DB_InsertCeraItemHistory::makeRequest(int itemIdx,
                                           unsigned int senderCharacNo,
                                           unsigned int receiverCharacNo,
                                           char flag)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xac1b),
                       true);
    guard.operator*()->operator<<(0x2b1);          // SIG_INSERT_CERA_ITEM_HISTORY
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(itemIdx);
    guard.operator*()->operator<<(senderCharacNo);   // ORIG lsEj（unsigned）
    guard.operator*()->operator<<(receiverCharacNo); // ORIG lsEj（unsigned）
    guard.operator*()->operator<<(flag);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}