void WarRoom::MakeStateInfo(char* buf)
{
    // [b28d-solo-WarRoom] DIFF(14) | ORIG 0x086be07a | N82附：4.1 对成员调用
    //   receiver-first（先 `mov -0xc(%ebp),%eax` 装 p，实参 scratch 用 %edx），
    //   4.4 恒 arg-first（实参 scratch %eax 后压、再装 receiver）。探针
    //   /tmp/probe_rcv.cpp t1..t10（指针/引用/解引用/cast 全形）均 arg-first ⇒ 封顶。
    //   本轮 fresh .o 复测：ndiff=11（orig 26/our 25），显式 (int) cast 类型旋钮
    //   （P265/P319）零变化 ⇒ 判 N7/P75（实参装载时刻跨版本）。
    // [b28d-solo-WarRoom] 翻正：探针 /tmp/wr_probe5.cpp t9 实证 —— 局部声明为
    //   char*（与形参同型，保留 ORIG -0xc 槽 store），调用点再 cast 到
    //   InterfacePacketBuf* ⇒ 4.4 复现 ORIG「值先物化 %edx、再装 receiver %eax、
    //   后统一压 esp 槽」形（t7/t10 的指针局部同型 cast 恒折 arg-first，不达）。
    char* p = buf;
    // [b28BM13 N18a/P75] ORIG 086be089/086be0a4/086be0bb：4.1 先把实参值物化
    // 到 %edx 再统一装 esp 槽；4.4 恒「即装即压」落 %eax（单段 mem→%edx 不可
    // 造）。register 绑定按 P0 全域禁用（旧 asm("edx") 尝试注记作废）⇒ 记 N。
    ((InterfacePacketBuf*)p)->put_short(m_index);
    ((InterfacePacketBuf*)p)->put_byte(m_state);
    ((InterfacePacketBuf*)p)->put_short(m_curSpawnStep);
