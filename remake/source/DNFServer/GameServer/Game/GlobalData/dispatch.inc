void DBThread::dispatch(void* arg)
{
    (void)arg;
    DBTask* task = new DBTask;
    if (task == 0)
    {
        std::cerr << "[DBThread::dispatch] Memory allocation fail!" << std::endl;
        OS_API::Exit(0);
    }

    // 形参类型精确匹配（不加调用处强制转换）：ORIG 0x083ff873 的 HandleEvents
    // 实参装载序为 `&signal → queueIdx → this`（右→左），4.4 在调用处写
    // `(MsgQueueMgr::QUEUE_IDX)queueIdx` 会把 queueIdx 的读取提前（寄存器换 %edx）。
    MsgQueueMgr::QUEUE_IDX queueIdx = (MsgQueueMgr::QUEUE_IDX)2;
    if (isLogDB())
    {
        queueIdx = (MsgQueueMgr::QUEUE_IDX)4;
    }
    else
    {
        // ORIG 0x083ff819：isLogDB 为假时仍有一条 `movl $0x2,-0x28(%ebp)`
        // 显式赋值（即源码写了冗余 else 分支），非 fall-through。
        queueIdx = (MsgQueueMgr::QUEUE_IDX)2;
    }

    message_counter::CMessageCounter msgCounter(20000);
    int curSec = 0;
    int lastWriteSec = 0;
    message_counter::CTimeInterval* interval =
        message_counter::CTimeInterval::Create();
    int signal = 0;

    while (!GlobalData::s_server_stop)
    {
        curSec = GlobalData::s_systemTime_.getCurSec();
        interval->SetLastTime();
        signal = 0;
        if (!task->HandleEvents(queueIdx, signal))
        {
            break;
        }
        if (signal != 0)
        {
            msgCounter.IncreaseMessage(
                (unsigned int)signal, (unsigned char)0, 0,
                interval->GetTimeInterval());
            if (curSec - lastWriteSec > 600)
            {
                lastWriteSec = curSec;
                msgCounter.WriteLog();
            }
        }
    }
    message_counter::CTimeInterval::Destory(interval);
    delete task;
