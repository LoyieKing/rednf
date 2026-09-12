// [b28d-4] DIFF：N1/N107：ORIG lea+add 两步寻址 vs 4.4 折单条；ORIG 08424ce6
// [L4-r1] 3 处 cMyTrace 改 P227 全表达式临时链 ⇒ ndiff 389→380（余
//   jle/jg 极性 + 6 条 mov 中转，属 N82/N4 族封顶）。
bool DB_ReqBlackCount::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    // 声明序照 ORIG 栈槽（低→高）：problemChildTime(-0x70) offsetPoint(-0x6c)
    // blackPoint(-0x68) trace 三实例(-0x64/-0x54/-0x44) sig(-0x34) db(-0x30)
    // result(-0x2c) nRows(-0x28) passCntLimit(-0x24) passCnt(-0x20)
    // computed(-0x1c)。guard 于组包处构造。
    // 修正（V331b16 核验）：ORIG **确有**三条预清 —— 0x08424dd6 `movl $0x0,-0x68`、
    // 0x08424ddd `movl $0x0,-0x6c`、0x08424e34 `movl $0x0,-0x70`（分别紧邻
    // get_int(0)/get_int(1)/get_uint(2)）⇒ 原码三变量带 `= 0`；裸声明会让
    // get_int/get_uint 失败路径读到未初始化栈值（`if (problemChildTime != 0)` 仍会执行）。
    // 旧注 "P171/P328⑤：ORIG 头部无预清" 已被 ORIG 汇编证伪。
    unsigned int problemChildTime = 0;
    int offsetPoint = 0;
    int blackPoint = 0;
    SIG_REQ_BLACK_COUNT* sig;
    MySQL* db;
    int result;
    int nRows;
    unsigned int passCntLimit;
    unsigned int passCnt;
    int computed;

    sig = stream->GetOutBuffer<SIG_REQ_BLACK_COUNT>();
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    result = 0;
    db->set_query(
        "seLect black_point,offset_point,unix_timestamp(problem_child_time) from charac_black_info where charac_no=%d",
        (int)sig->m_characNo);
    if (db->exec(true) != 1)
        return 0;
    nRows = db->get_n_rows();
    if (nRows > 1)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x622c,
                              "DB_ReqBlackCount::dispatch error! data(%d)",
                              nRows);
    }
    else if (nRows == 0)
    {
        result = 1;
    }
    if (db->fetch() != 1)
        result = 2;
    if (db->get_int(0, blackPoint) != 1)
        result = 3;
    if (db->get_int(1, offsetPoint) != 1)
        result = 3;
    if (db->get_uint(2, problemChildTime) != 1)
        result = 3;
    if (problemChildTime != 0)
    {
        // G_CDataManager()+0x3b4：黑名单消减周期（秒/周参数）；+0x3b8：单次消减点数。
        passCntLimit =
            ((DB_CDataManagerBlkView*)G_CDataManager())->m_3b4 * 0x15180;
        if (passCntLimit != 0 &&
            GlobalData::s_systemTime_.getCurSec() >
                (int)(problemChildTime + passCntLimit))
        {
            passCnt = (GlobalData::s_systemTime_.getCurSec() -
                       problemChildTime) /
                      passCntLimit;
            if (blackPoint - offsetPoint > 0)
            {
                computed = blackPoint - offsetPoint -
                           ((DB_CDataManagerBlkView*)G_CDataManager())->m_3b8 *
                               (int)passCnt;
                // [L4-r1] P227 全表达式临时链（ORIG ctor 前取 varargs + 跨 ctor 保活）。
                cMyTrace(__PRETTY_FUNCTION__, 0x6259, 0)(
                    "BlackList Modify : c(%d) iPassCnt(%d) iOffsetPoint(%d) iBlackPoint(%d) uProblemChildTime(%d) iComputedBlackPoint(%d)",
                    (int)sig->m_characNo, passCnt, offsetPoint, blackPoint,
                    problemChildTime, computed);
                if (computed < 1)
                {
                    db->set_query(
                        "upDate charac_black_info set offset_point = black_point , problem_child_time = 0 where charac_no=%d",
                        (int)sig->m_characNo);
                    offsetPoint = blackPoint;
                    problemChildTime = 0;
                    cMyTrace(__PRETTY_FUNCTION__, 0x6269, 0)(
                        "BlackList Modify : c(%d) iPassCnt(%d) iOffsetPoint(%d) iBlackPoint(%d) uProblemChildTime(%d)",
                        (int)sig->m_characNo, passCnt, offsetPoint, blackPoint,
                        problemChildTime);
                }
                else
                {
                    db->set_query(
                        "upDate charac_black_info set offset_point = offset_point  + %d, problem_child_time = from_unixtime(%d) where charac_no=%d",
                        ((DB_CDataManagerBlkView*)G_CDataManager())->m_3b8 *
                            (int)passCnt,
                        passCntLimit * passCnt + problemChildTime,
                        (int)sig->m_characNo);
                    offsetPoint =
                        ((DB_CDataManagerBlkView*)G_CDataManager())->m_3b8 *
                            (int)passCnt +
                        offsetPoint;
                    cMyTrace(__PRETTY_FUNCTION__, 0x6262, 0)(
                        "BlackList Modify : c(%d) iPassCnt(%d) iOffsetPoint(%d) iBlackPoint(%d) uProblemChildTime(%d)",
                        (int)sig->m_characNo, passCnt, offsetPoint, blackPoint,
                        problemChildTime);
                }
                if (db->exec(true) != 1)
                {
                    LogManager::logFormat(
                        1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x626c,
                        "BlackList Modify db exec error: iPassCnt(%d) iOffsetPoint(%d) iBlackPoint(%d) uProblemChildTime(%d)",
                        passCnt, offsetPoint, blackPoint, problemChildTime);
                }
            }
        }
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6271), true);
    guard.operator*()->operator<<(0xb3);
    guard.operator*()->operator<<(slot);
    guard.operator*()->operator<<((int)sig->m_characNo);
    guard.operator*()->operator<<(result);
    int diff = blackPoint - offsetPoint;
    guard.operator*()->operator<<(diff & ~(diff >> 31));
    guard.operator*()->operator<<(problemChildTime);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}