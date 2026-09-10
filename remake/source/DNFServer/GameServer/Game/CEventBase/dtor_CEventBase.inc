CEventBase::~CEventBase()
{
    // ORIG 仅恢复基类 vptr；删除由 deleting destructor 负责。
