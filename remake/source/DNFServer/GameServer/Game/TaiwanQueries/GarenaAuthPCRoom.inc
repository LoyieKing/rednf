// ORIG T 0x08471c74
void GarenaAuthPCRoom(CUser* user)
{
    if (user == 0)
        return;

    // b28d-36（修法 P104/P76）：改用全局权威类（0x1f，PacketHeader 派生）——
    // ORIG 0x08471c8c 先 call 全局 ctor `_ZN22Packet_Pcroom_ResponseC1Ev`，
    // 再字段直写 +0x12/-0x1a/-0x1b/-0x16（movl/movb/movb/movl），put_binary
    // 长度取 +0x02 packetSize（movzwl -0x25）。本地 F 视图版多 4 条 lea 基址
    // 且 ctor 落在 Taiwan 命名空间（p167 MISSING）。
    Packet_Pcroom_Response resp;
    resp.m_field12 = 1;
    resp.m_field1a = 1;
    resp.m_field1b = 0xb;
    resp.m_expireTime = static_cast<int>(time(0)) + 0x93a80;

    // b28d-36（P0/P92）：ORIG 0x08471cc2/0x08471cc8 直写 resp 字段
    // （-0x1d=resp+0xa m_uid、-0x19=resp+0xe m_accId），非具名 uid/accId 局部
    // ——具名版把 resp 基址顶深 8（-0x2f vs -0x27）且 guard 槽 -0x38 vs -0x30。
    resp.m_uid = static_cast<int>(user->GetUID());
    resp.m_accId = static_cast<int>(user->get_acc_id());   // ORIG 写入后不再读

    // [b23S3C] P48/P0：Acquire 右值直落 ctor 实参（无 stream 槽，ORIG +0x081）；
    // 写流两跳 `(*guard)->operator<<`（deEv 080c8c46 + lsEi 080c8c56 各一次，
    // +0x090/+0x0ae）；put_binary 走 operator->（ptEv +0x0d2）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DF_PCRoomServerProxy.cpp", 0x1ab),
        true);
    (*guard)->operator<<(0x17e);
    (*guard)->operator<<(resp.m_uid);
    guard.operator->()->put_binary(
        &resp, resp.packetSize);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}