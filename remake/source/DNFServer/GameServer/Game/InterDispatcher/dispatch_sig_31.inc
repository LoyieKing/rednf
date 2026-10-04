// [b28d-lane2] verdict=DIFF ndiff=146（ORIG 0x084cf0c4）：槽序/帧差(N82/P212/P221)+混合残差；gap-18。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=120（ORIG 0x84cf0c4）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [R10a07 ④ 修复] 包缓冲布局错：ORIG 是「int count + int items[5]」连续结构（0x084cf1be
//   `mov -0x2c(%ebp),%eax; mov %edx,-0x28(%ebp,%eax,4)` = items[count] 于结构首址+4；
//   0x084cf256 `lea -0x2c(%ebp),%edx` = put_binary 实参取**结构首址** ⇒ 线格式包首 4B=count、
//   数据自 +4 起）。旧重建写 `int eventItems[6]` + 独立 count（NEW 0x815a51a 写 -0x34+count*4、
//   0x815a5c7 put_binary 取 -0x34）⇒ 包首 4B=首个数据元素、全部数据相对 ORIG 偏移 −4（发包内容错）。
struct Inter_LoadEventItemBuf
{
    int m_count;      // +0x00 校验通过条数（随结构整体入包）
    int m_items[5];   // +0x04 逐条 eventItem
};

int Inter_LoadEventItem::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_LoadEventItemPktView* pPkt = reinterpret_cast<Inter_LoadEventItemPktView*>(data);
    CUser* target = G_CGameManager()->GetUserByAccId(
        pPkt->m_accId);        // msg+0x0 accId
    if (target == 0)
        return 0;

    Inter_LoadEventItemBuf buf;                         // ORIG 0x084cf10b memset 基址 = &count
    memset(&buf, 0, 0x18);
    for (int i = 0;
         i < pPkt->m_itemCount; ++i)  // msg+0x8 条数
    {
        // P183：ORIG 无 skip 具名槽（084cf12f `cmp $0x3e7,%eax` 后 `je 主体`、
        //   次条件不满足时 `jne 循环尾`）⇒ 原码是单个短路条件，不设 bool skip；
        //   等价化简 (m10==999) || (state>2 && curCharacNo==m_4)。
        // [L9-G PATCH#4 负结论 · 已回滚] 删 `(unsigned char)` cast（callee 当时仍 `unsigned int`）
        //   实测**反劣**：fn_diff 差异点 135 → 143 —— 因 4.4 对 int 返回值的 `if(f())` 恒发
        //   `test %eax,%eax; setne %al; test %al,%al; je` 4 段，与 ORIG 2 段不符。
        // [R19ty T-1 2026-09-13] callee 返回类型已按 P292 证据改 `bool`（GlobalData.h +
        //   W6_SecMiscDeps/RecvInfoFromDB.inc），故此处**必须**删 cast：ORIG 0x084cf1b5
        //   `call; test %al,%al; je`（0x084cf1ba/0x084cf1bc）无 setcc；保留 cast 会重新引入
        //   `test %al,%al; setne %al`（我方旧 NEW 0x815e4e8 段 `0f 95 c0` 实证）。
        if (pPkt->m_rows[i].m_a == 999
            || (target->get_state() > 2 &&
                target->getCurCharacNo() == pPkt->m_characNo))
        {
            if (WongWork::CWebEvent::RecvInfoFromDB(
                    target,
                    (unsigned int)pPkt->m_rows[i].m_a,
                    (unsigned int)pPkt->m_rows[i].m_b,
                    (unsigned int)pPkt->m_rows[i].m_c,
                    (unsigned int)pPkt->m_rows[i].m_item))
            {
                buf.m_items[buf.m_count] = pPkt->m_rows[i].m_item;
                buf.m_count = buf.m_count + 1;
            }
        }
    }

    if (buf.m_count != 0)
    {
        // [R15a11 P259] ORIG 0x084cf219 `call Acquire; movl $0x1,0x8(%esp); mov %eax,0x4(%esp)`
        //   无 stream 具名槽（我方旧形 -0xc 槽 + 2 条 store/load、并使 i/target/pPkt 整体 +4）
        //   ⇒ Acquire 结果内联进 guard 构造实参。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                               "InterDispatcher.cpp", 0x2c1f), true);
        guard.operator*()->operator<<(0x4d);
        guard.operator*()->operator<<(-1);
        guard->put_binary(&buf, 0x18);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
    return 0;
}