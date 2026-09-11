// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x0847e2d2：主分发循环。TimerEntry 取定时器消息 →
// TimerDispatcher::dispatch；DISPATCH_Q 取 Stream → 按流头 code 分流
// PacketDispatcher（客户端包）/ InterDispatcher（服务器互连包）；池归还按
// stream->m_poolType（0/1/2 = 默认/小/大流池）。CMessageCounter(10000) 与
// CUserPacketCounter 600s 周期落盘。pUserPacketCounter_ 指向本栈实例。
// ---------------------------------------------------------------------------
// [b28d-18 NEAR] 修：curSec/lastWriteSec 角色互换（ORIG 循环外初始化 curSec、循环内更新 lastWriteSec）；p167 CLEAR 53/53；余 N82 帧槽 | ORIG 0x0847e2d2
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（53/53）；gap 1=N82 帧伪影（勿强凑，P182）。
// [G7-sub 2026-09-11 判词] DispatchThread::dispatch（ORIG T 0x0847e2d2）：私有 .o 实测
// ORIG 325 / 我方 325（gap 0），**助记符序列全等**（fdiff --mnemonic hunks=0），
// 残差仅寄存器/栈槽分配（N82 族）。p167 调用点差集仅 cMyTrace C1↔C2 同址别名
// （ORIG nm 实测 C1=C2=0x0854f746）。结论：**封顶，无改动**（本批最干净件）。
void DispatchThread::dispatch(void* arg)
{
    (void)arg;
    TimerEntry entry;
    unsigned char typeByte = 0;      // 首字节（客户端包分流码）
    unsigned short sizeWord = 0;     // +1 起的 ushort
    message_counter::CMessageCounter msgCounter(10000);
    // ORIG 0847e322/0847e7xx：lastWriteSec（-0x34）循环外置 0，curSec（-0x30）
    // 循环外以 getCurSec() 初始化；循环内更新 lastWriteSec，比较
    // `lastWriteSec - curSec > 600` 后回写 curSec（两次 getCurSec 调用）。
    int lastWriteSec = 0;
    int curSec = GlobalData::s_systemTime_.getCurSec();
    message_counter::CUserPacketCounter userCounter(
        G_CEnvironment()->get_file_name(), "UserPacketCounter");
    PacketDispatcher::pUserPacketCounter_ = &userCounter;
    message_counter::CTimeInterval* interval =
        message_counter::CTimeInterval::Create();
    // ORIG 0x0847e2d2 函数头无 `movl $0x0,-0x2?` 预清 ⇒ 原码裸声明（P0）。
    int code;   // 流头读出的分发码
    int idx;    // 流头读出的用户 id

    while (!GlobalData::s_server_stop)
    {
        GlobalData::s_systemTime_.update();
        if (G_TimerQueue()->GetTimerMess(entry))
        {
            interval->SetLastTime();
            G_TimerDispatcher()->dispatch(entry);
            msgCounter.IncreaseMessage(
                0, (unsigned char)0,
                (unsigned short)((short)entry.msg + 30000),
                interval->GetTimeInterval());
        }

        Stream* stream = GlobalData::s_msgq_mgr->get(MsgQueueMgr::DISPATCH_Q);
        if (stream == 0)
        {
            OS_API::uSleep(100);
            continue;
        }
        int length = stream->length();
        int streamType = stream->m_poolType;
        *stream >> code;
        *stream >> idx;
        lastWriteSec = GlobalData::s_systemTime_.getCurSec();
        interval->SetLastTime();
        if (code == 0)
        {
            // 客户端包线格式头：get_buffer()+0 type(uchar)/+1 size(ushort)。
            // ORIG 0847e496..0847e4b2（DispatchThread/dispatch.md）`movzbl (%eax)`
            // / `add $0x1; movzwl (%eax)` 直译；MSG_BASE 权威头为 13B 占位 blob
            // （内部字段无证据，CMDPacketStruct.h 注），布局未建模，P156 豁免。
            typeByte = (unsigned char)*stream->get_buffer();
            sizeWord = *(unsigned short*)(stream->get_buffer() + 1);
            G_PacketDispatcher()->dispatch(
                idx, stream->get_buffer(), stream->length());
        }
        else
        {
            typeByte = 0;
            sizeWord = 0;
            G_InterDispatcher()->dispatch(
                code, idx, stream->get_buffer(), stream->length());
        }
        msgCounter.IncreaseMessage(
            (unsigned int)code, typeByte, sizeWord,
            interval->GetTimeInterval());
        if (lastWriteSec - curSec > 600)
        {
            curSec = lastWriteSec;
            msgCounter.WriteLog();
            userCounter.WriteLog();
            // ORIG 0x0847e???：varargs 在 cMyTrace ctor 前求值并落 callee-saved
            // （N80/P328⑩）⇒ 单语句全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x8f, 5)(
                "timer_queue_count(%u)", G_TimerQueue()->GetQueueSize());
            G_TimerQueue()->write_timer_queue_log();
        }
        switch (streamType)
        {
        // [GlobalData-判据①] ORIG 0x0847e61d 决策树 `cmp $1;je +0x35f;
        // cmp $2;je +0x442; test %eax,%eax;je +0x3d2; jmp +0x4b0`：
        //   case 1 → +0x35f（0xfff / "Small…" / s_small_stream_pool）
        //   case 0 → +0x3d2（0x3fff / "Default…" / s_stream_pool）★旧源码把
        //            case 0 并入 case 1（错发 SmallStreamPool::Free），且把
        //            s_stream_pool 体挂在 default（错：default 应为空体
        //            +0x4b0 直落循环尾）。
        //   case 2 → +0x442（0x3ffff / "Big…" / s_big_stream_pool）
        // 叶块物理序（P324）= 书写序 = 1, 0, 2；default 空体。
        case 1:
            if (0xfff < length)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x9e, 5)(
                    "Small Stream Size over code(%d), idx(%d), size(%u)",
                    code, idx, length);
            }
            GlobalData::s_small_stream_pool->Free(stream);
            break;
        case 0:
            if (0x3fff < length)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0xa8, 5)(
                    "Default Stream Size over code(%d), idx(%d), size(%u)",
                    code, idx, length);
            }
            GlobalData::s_stream_pool->Free(stream);
            break;
        case 2:
            if (0x3ffff < length)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0xb2, 5)(
                    "Big Stream Size over code(%d), idx(%d), size(%u)",
                    code, idx, length);
            }
            GlobalData::s_big_stream_pool->Free(stream);
            break;
        default:
            break;
        }
    }
    message_counter::CTimeInterval::Destory(interval);
}