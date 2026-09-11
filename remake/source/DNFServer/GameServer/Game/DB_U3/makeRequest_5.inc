// ---- DB_UpdateHappyBeanInfo::makeRequest ORIG @ 0x08425a02 ----
// stream: opcode 0xbb | -1 | 逐字段 uid/accId/characNo/name（operator<< 直写）。
// 投 DB_Q（ORIG put 第 2 实参 2）。
void DB_UpdateHappyBeanInfo::makeRequest(unsigned int uid, unsigned int accId,
                                         unsigned int characNo, const char* name)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x63a3), true);
    guard.operator*()->operator<<(0xbb);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(characNo);
    guard.operator*()->operator<<((char*)name);   // ORIG ls(Pc)：const char* 直接绑 bool 重载

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}