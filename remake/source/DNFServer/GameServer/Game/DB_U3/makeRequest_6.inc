void DB_Insert0712WinterEventCoupon::makeRequest(unsigned int accId,
                                                 unsigned int characNo,
                                                 unsigned int count)
{
    // b28c-33：ORIG 0x0842e648 两次 _makeRequest 复用同一 -0xc 槽（P160 复用律）
    // ⇒ 单一具名 stream 局部，勿声明 first/second 两个槽。
    // b28d-61 探针：外层裸早返 + 内层裸 if（ORIG 首失败出口 +0x94 nop 岛、
    // body2 尾 jmp +0x95）——裸早返/ goto 尾标签 / 空 else 三形实测均多出两对
    // jmp+nop 中继（43 insn）。
    Stream* stream = _makeRequest((ENUM_INTERNALPACKET)0x112, accId, characNo, count);
    if (stream == 0)
        return;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, stream);
    stream = _makeRequest((ENUM_INTERNALPACKET)0x113, accId, characNo, count);
    if (stream != 0)
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, stream);
