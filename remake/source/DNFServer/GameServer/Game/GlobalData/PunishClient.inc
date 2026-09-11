// [b28d-solo-GlobalData] 改善 74→68（p167 12/12 CLEAR）：P259 去 stream 具名
// 局部（ORIG 0x080de3f6 `call Acquire; movl $1,0x8(%esp); mov %eax,0x4(%esp)`）+
// info 前 8 字节结构整体赋值（ORIG 0x080de46c `mov 0x4(%eax),%edx; mov (%eax),%eax`
// 先高字后低字）+ body.field04 1 字节（ORIG 0x080de469 `mov %al,-0x1c`，T1 直证）。
// 残差（已闭）：ORIG 尾部 `mov $0x0,%ebx … mov %ebx,%eax` ⇒ 原函数返回 int 0。
// [b28d-PoolGlobalData] R2 落地：CBusinessImpl.h:34/48 void→int + `return 0`
// ⇒ 0x080de3ee ndiff 19→0（AE）。
int CBusinessImpl::PunishClient(int a, int b, unsigned int c, EPUNISHMODE mode,
                                tagAntibotPunishInfo* info, char* reason)
{
    (void)a;
    (void)b;
    (void)reason;
    // ORIG 0x080de3ee：StreamPool 取流；CStreamGuard(stream,true)；<<0x253 <<-1；
    // put_binary({uint c; EPUNISHMODE mode(byte); info 前 8 字节}, 0x10)。
    // [b28d-solo-GlobalData] P259：ORIG 0x080de3f6 无 stream 具名局部
    // （Acquire 结果经 %eax 直传 guard ctor 的 0x4(%esp)）⇒ 内联实参。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x59),
                       true);
    **guard << 0x253;
    **guard << -1;
    // [b28d-solo-GlobalData] ORIG 0x080de46c：`mov 0x4(%eax),%edx; mov (%eax),%eax;
    // mov %eax,-0x18; mov %edx,-0x14` = info 前 8 字节**结构整体赋值**
    // （4.1 先读高字后读低字）；逐字赋值会反序且多具名指针局部。
    struct PunishTail
    {
        int field08;
        int field0c;
    };
    struct
    {
        unsigned int field00;
        // ORIG 0x080de469 `mov %al,-0x1c` ⇒ field04 为 1 字节（枚举形参按
        // 4 字节栈槽读入后截断存储）；声明 EPUNISHMODE（4 字节）会生 4 字节 store。
        unsigned char field04;
        PunishTail tail;
    } body;
    body.field00 = c;
    body.field04 = (unsigned char)mode;
    // tagAntibotPunishInfo 完整布局未建模（CBusinessImpl.h 仅前向声明，
    // 「antibot 域完整布局待专门批次」）；ORIG 080de3ee 取 info 前两 dword
    // 作 8 字节块赋值，此处以等价视图类型表达。
    body.tail = *reinterpret_cast<const PunishTail*>(info);
    guard->put_binary(&body, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    // [b28d-PoolGlobalData R2] ORIG 0x080de3ee 尾 `mov $0,%ebx … ~CStreamGuard …
    // mov %ebx,%eax` ⇒ 返回 int 0 跨 guard 析构保活（头 void→int 已报 Main）。
    return 0;
}