int Dispatcher_GatheringPartyStatus::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // §6.2 语义完成：P167 调用点差集双零（59/59）；orig531/our490，gap-41 为
    // 后端帧/槽分配冗余（N82 残余），非缺语义 → byte 层 N 封顶。
    // ORIG 0x08203878：组队状态收集进 DB（PacketDispatcher_Impl_1.cpp）。
    // 接收端包体为 pack(1) 结构（sizeof==0x1b4，与 ORIG memset 尺寸一致）：
