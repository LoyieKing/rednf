// ORIG T 0x0842688c（dispatch.md 全文）：基类 dispatch 失败返 0；
// GetDBHandle(DB_HANDLE_2,0) → GetOutBuffer<SIG_QUERY_CHARAC_INFO>（首字节即
// 角色名）→ escape_string → charac_info 查 7 列；exec 失败 → _sendResult(2,...)；
// get_n_rows()==0 两次独立判定（P48 双调用）→ _sendResult(0x15,...)；
// fetch/get_int(0) 失败 → _sendResult(2,...)；delete_flag!=0 → 0x15；
// 成功 → 读列 1..6（m_id/charac_no/job/lev/grow_type/sex）→
// 成功路径 _getGiftRecvCount 前有 local_58[0x14] "member_login" 写后死存储
// （memset+memcpy 后未读，还原 ORIG 帧 sub $0xdc）→
// _getGiftRecvCount(m_id) 作尾参 → _sendResult(0, slot, sig, ...) → 返 1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0842688c
bool DB_ReqQueryCharacInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    SIG_QUERY_CHARAC_INFO* sig = stream->GetOutBuffer<SIG_QUERY_CHARAC_INFO>();

    char name[0x3c] = { '\0' };
    db->escape_string(name, (const char*)sig);
    db->set_query(
        "seLect delete_flag,m_id,charac_no,job,lev,grow_type,sex from charac_info where charac_name='%s'",
        name);
    if (!db->exec(true))
    {
        _sendResult(2, slot, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }

    if (db->get_n_rows() == 0)
    {
        _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }
    // [P5S2W8LT r2] ORIG 0x08426a2f `test %eax,%eax; setne %al; je 0x08426cb6`：
    // 第 3 次 get_n_rows 检查是**正极性**（正文 fall-through），且
    // `_sendResult(0x15)` 块被本检查与 deleteFlag!=0 共用（块位 0x08426cb6 落在
    // 函数后部；第 2 次检查的 sete/0x084269cb 块保持原样）⇒ 原两次同形早退
    // 使此处产 sete 且块位错。
    if (db->get_n_rows() != 0)
    {
    // [G4W8LTDBTask 2026-09-10 负结论] 试删 7 处零初始化（P328⑤ 形）：
    // our 258→251（gap |+4|→|−3|）但 ndiff 47→53 劣化 ⇒ 已回滚。
    int deleteFlag = 0;
    if (!db->fetch() || !db->get_int(0, deleteFlag))
    {
        _sendResult(2, slot, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }
    if (deleteFlag == 0)
    {

    unsigned int mId = 0;
    int characNo = 0;
    int job = 0;
    int lev = 0;
    int growType = 0;
    int sex = 0;
    db->get_int(1, mId);
    db->get_int(2, characNo);
    db->get_int(3, job);
    db->get_int(4, lev);
    db->get_int(5, growType);
    db->get_int(6, sex);

    // ORIG 0x08426c0e(+0x37a) 实证：local_58[0x14] 缓冲区 memset+memcpy
    // "member_login" 后从未读取（写后死存储，-0x54(%ebp) 仅作 memset/memcpy
    // 目标）。为逐字节还原 ORIG 帧布局（sub $0xdc）与这两次调用保留之。
    char memberLogin[0x14];
    memset(memberLogin, 0, 0x14);
    memcpy(memberLogin, "member_login", 0xd);

    int giftCnt = _getGiftRecvCount(mId);
    _sendResult(0, slot, (const char*)sig, characNo, job, lev, growType, sex,
                giftCnt);
    return 1;
    }
    // [P5·2026-09-11] deleteFlag != 0 路径：ORIG 0x08426b2f 是**独立**的
    //   `_sendResult(0x15,…)` 块（其后 `jmp 08426d0d` 直落 epilogue），与函数尾
    //   0x08426d03 的 15 块**不共享** —— 批2 的共享尾块令我方少 1 个 call 点
    //   （p167fix 报 MISSING 1，ORIG 6 处 call vs 我方 5 处）。按 ORIG 拆回独立块：
    //   执行期两条路径仍各调 1 次（无「少发包」），仅恢复 call 点布局。
    _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0, 0);
    return 1;
    }
    _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}