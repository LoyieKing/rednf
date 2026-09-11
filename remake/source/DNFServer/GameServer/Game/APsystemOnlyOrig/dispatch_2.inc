// ORIG 0x0843b3b6 sz 0x335：*stream >> m_id(uint) → GetDBHandle(2,0) →
// NumberToString(m_id,0) → seLect capacity,money,cargo from account_cargo
// where m_id=%s → exec 失败返 0；get_n_rows()==0 返 1；fetch 失败返 0；
// Acquire("DBThread.cpp",0x8c53) → CStreamGuard(stream,true) → <<userIdx →
// <<slot → GetInBuffer<SIG_ACCOUNT_CARGO_DATA> → memset 0xd60 →
// get_uint(0,cap)/get_uint(1,money)/get_binary(2,buf,0x100f) 任一失败返 0 →
// get_binary_length(2) → outLen=0xd58 → uncompress_zip(sig+8,&outLen,buf,len)
// 失败返 0；outLen==0 || outLen%0xd58==0 → put(DISPATCH_Q,guard) 返 1，否则返 0。
// [b28d-4] DIFF：P222：块序/极性（ORIG je 落点 vs 4.4 jne）；ORIG 0843b3b6
bool DB_LoadAccountCargo::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned int mId = 0;
    MySQL* db;
    SIG_ACCOUNT_CARGO_DATA* sig;
    int binLen;
    unsigned int outLen;
    // [Q5W8LT 2026-09-11 负结论·回滚] 曾试删此初始化器（想消我方 41 条 4.4 inline
    // memset 展开），实测 our 189 / ORIG 225（gap +36 劣化）⇒ ORIG 0x0843b3c9 起
    // **确实**有清零（`lea -0x1043(%ebp),%edx; mov $0x100f,%ebx; ...; shr $0x2,%ecx;
    // rep stos %eax,%es:(%edi)` 共 49 条），只是 4.1 用 rep stos 而 4.4 用逐字节/字
    // 对齐循环（41 条，无 rep stos）⇒ 编译器 stringop 策略差，非源形可控。保留初始化器。
    char buf[0x100f] = { '\0' };
    static const int compressed_buff_size = 0x100f;  // ORIG _ZZ…DB_LoadAccountCargo…E20compressed_buff_size

    *stream >> mId;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("seLect capacity, money, cargo from account_cargo where m_id = %s",
                  NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() == 0)
        return 1;
    if (db->fetch() != 1)
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c53), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    sig = guard.operator->()->GetInBuffer<SIG_ACCOUNT_CARGO_DATA>();
    memset(sig, 0, 0xd60);
    if (db->get_uint(0, sig->m_capacity) != 1)
        return 0;
    if (db->get_uint(1, sig->m_money) != 1)
        return 0;
    if (db->get_binary(2, buf, compressed_buff_size) != 1)
        return 0;
    binLen = db->get_binary_length(2);
    outLen = 0xd58;
    if (uncompress_zip((char*)sig->m_cargo, outLen, buf,
                       static_cast<unsigned int>(binLen)) != 1)
        return 0;
    // [J4S1 P222] ORIG 0x0843b65c/0x0843b68c 双 `je <put 块>`、fallthrough = 失败块
    // ⇒ 原码是否定早返（put 块后置），写成正向包裹会产反极性 `jne`。
    if (outLen != 0 && outLen % 0xd58 != 0)
        return 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}