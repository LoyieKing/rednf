// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::make_ready_info（原 TU 第 2000 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 P75] ORIG 0x085dbcce：常量/形参实参点先载 receiver(-0xc→%eax) 再 store 常量槽，4.4 恒反序；指令集合/语义全等（P75 跨版本调度），P167 CLEAR。
// [b28d-solo-PvP_Room] P75: ORIG 0x085dbcce 实参装载时刻 %edx vs %eax 跨版本调度
int PvP_Room::make_ready_info(char* buf, int seat)
{
    // P75/N7 封顶（ORIG 0x085dbcd4）：ORIG(4.1) 对常量/形参实参点先载 receiver
    // (-0xc→%eax) 再 store 常量槽，计算值实参点先物化值(%edx) 后载 receiver；
    // 4.4 恒先 store 实参后载 receiver 且 scratch 用 %eax 顶替 %edx。指令集合/
    // 语义全等，装载时刻与寄存器角色为跨版本调度固有差（P75 判例），勿再耗轮次。
    InterfacePacketBuf* local_f4 = (InterfacePacketBuf*)buf;
    local_f4->put_header(0, 0x2c);
    local_f4->put_byte(seat);
    local_f4->put_byte(P_SEATSTATE(seat));
    local_f4->finalize(true);
}
