DBTask::~DBTask()
{
    // ORIG 0x083fbfda：仅调 UnregistDispatcher（报告 DBTask/~DBTask.md）。
    UnregistDispatcher();
