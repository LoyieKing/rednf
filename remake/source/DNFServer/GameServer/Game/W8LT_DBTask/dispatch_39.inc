// ---------------------------------------------------------------------------
// DB_InsertQueryCounterDb::dispatch —— ORIG 0x08425b0c
// ---------------------------------------------------------------------------
// ORIG：len=0/qid=0 → memset(buf,0,0x6001) → >>len → get_binary(buf,len) →
// GetDBHandle(2,0) → GetQueryCounter() → SaveQuery(buf,&qid)（失败返 0）→
// Acquire("DBThread.cpp",0x63c1) → CStreamGuard(stream,true) → <<0xbf →
// <<-1 → <<qid → <<len → put_binary(buf,len) →
// MsgQueueMgr::put(LOGDB_Q(4), guard) → 返 1。guard 具名局部产生 ORIG 的
// EH cleanup（_Unwind_Resume 重抛）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08425b0c
// [r3] 声明序改 qid→len（P331 声明序=槽序）：qid(-0x10)/len(-0x14) 复现
//   ORIG 08425b0c `lea -0x14(%ebp),%eax`（len 先读）⇒ ndiff 27→5、gap 不变。
//   残余 = 4.1 常量先装寄存器（`mov $0x6001,%edx; mov %edx,0x8(%esp)`）
//   vs 4.4 直写栈（N18a）⇒ 封顶。
bool DB_InsertQueryCounterDb::dispatch(int userIdx, int slot, Stream* stream)
{
    int qid = 0;
    int len = 0;
    // [P5S2W8LT r1] P312 数组初始化器：ORIG 0x08425b17 序列
    //   `movl $0x0,-0x10; movl $0x0,-0x14; lea -0x601d(%ebp),%eax;
    //    mov $0x6001,%edx; mov %edx,0x8(%esp); movl $0x0,0x4(%esp);
    //    mov %eax,(%esp); call memset@plt`
    // ⇒ 槽序 qid(-0x10)/len(-0x14)/buf(-0x601d) 由「声明序 qid,len,buf」决定，
    //   且 memset 由数组初始化器展开（4.4 对该形态恒走 `mov $imm,%edx` 中转；
    //   显式 memset(buf,0,0x6001) 恒直写立即数 ⇒ 差 1 条）。
    char buf[0x6001] = { '\0' };
    *stream >> len;
    stream->get_binary(buf, len);
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // [b28d-solo-W8LT_DBTask] ORIG 0x08425b9x：`call SaveQuery; xor $1,%eax;
    // test %al,%al; jne <mov $0; jmp>` ⇒ 原码 P101 早返 + 裸 bool 判定
    // （`==1` 比较形 4.4 反产无 xor 直判）；P0/P52 链式 GetQueryCounter
    // （ORIG 无 counter 槽往返，receiver 为表达式右值）。
    if (!db->GetQueryCounter()->SaveQuery(buf, qid))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x63c1), true);
    guard.operator*()->operator<<(0xbf);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(qid);
    guard.operator*()->operator<<(len);
    guard.operator->()->put_binary(buf, len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    return 1;
}