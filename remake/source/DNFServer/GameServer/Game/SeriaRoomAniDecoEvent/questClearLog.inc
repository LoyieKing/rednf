// [b28d-solo-W8LT_EventTail] AE | ORIG 0x081ba21e | P167 CLEAR（38/38）。
// 修法：①字段经 m 视图读（非 msg.）；②删 progress/p 具名局部（P0/P183）；
// ③条件 `empty()`→`size() != 0`（p167 MISSING size）；④循环体由 `text = X + sep`
// 改左结合累加 `text = text + X + sep`——补回 p167 MISSING 的 1×`operator+` 与
// 2×`~string`（ORIG 两次 op+ 的临时析构），AST 左结合形与 ORIG 逐条同构；
// ⑤memcpy dest 用具名 `sig.m_data`：4.4 复现 ORIG「先压 size/src、后 lea+add 算
// dest」的装载序（原 `(unsigned char*)&sig + 0x14` 使 4.4 提前算到 %edx）；
// ⑥`sig` 局部加 `__attribute__((aligned(4)))`：ORIG 4.1 给 pack(1) 载荷 4 字节
// 对齐槽（-0xbc），4.4 默认 1 字节对齐（-0xb9）⇒ 补属性后槽基与全帧逐条对齐。
// ⑦memcpy 无补 NUL（P171 照抄）。
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
void OneADayItemShopEvent::questClearLog(DeliveryMsg& msg)  // 0x081ba21e
{
    EventStageLogDeliveryMsg& m = reinterpret_cast<EventStageLogDeliveryMsg&>(msg);  // ORIG：无 stage_view 调用

    if (!isQuestId(m.m_logType))
        return;

    // ORIG 4.1 给 pack(1) 载荷 4 字节对齐槽（-0xbc）；4.4 默认 1 字节对齐（-0xb9）
    // ⇒ 局部加 aligned(4) 复现槽基（不改类型 sizeof/布局，仅本函数栈槽）。
    SIG_EVENT_STAGE_LOG sig __attribute__((aligned(4)));
    // ORIG：经 m 视图（-0x1c 槽）读三字段，写入顺序 [1][2][3][0][4]
    sig.m_field0[1] = m.m_accId;
    sig.m_field0[2] = m.m_characNo;
    sig.m_field0[3] = m.m_serverGroup;
    sig.m_field0[0] = 0x6d;
    sig.m_field0[4] = 1;

    std::string text;                                  // 进度 first 值 '/' 连接
    std::string sep("/");
    // ORIG：无 progress 具名局部（每用点重读 m_progressList，P157/P183）；
    // 条件用 `size() != 0`（p167 MISSING size / EXTRA empty）；循环内用
    // `operator=`（p167 MISSING aSERKSs / EXTRA pLERKSs）。
    if (m.m_progressList != 0 && m.m_progressList->size() != 0)
    {
        typedef const std::pair<int, int> ProgressPair;
        BOOST_FOREACH(ProgressPair& pr, *m.m_progressList)
        {
            // ORIG：两次 operator+（text+lex 后接 +sep）⇒ 原码为左结合累加式。
            text = text + boost::lexical_cast<std::string>(pr.first) + sep;
        }
    }
    if (text.size() < 100)
    {
        memcpy(sig.m_data, text.c_str(), text.size());  // SIG 内嵌文本区（≤100）
    }

    DB_InsertEventStageLog::makeRequest(sig);
}
