// [b28d-7 P75] ORIG 0x085dbcce：常量/形参实参点先载 receiver(-0xc→%eax) 再 store 常量槽，4.4 恒反序；指令集合/语义全等（P75 跨版本调度），P167 CLEAR。
// [b28d-solo-PvP_Room] P75: ORIG 0x085dbcce 实参装载时刻 %edx vs %eax 跨版本调度
// [G8InvenPvP 2026-09-10 · P256 实测无效] 「引用 + 首成员访问」形
// （`InterfacePacketBuf& f = *(InterfacePacketBuf*)buf; f.put_header(0,0x2c);`）
// 产物与具名指针形**逐条相同**（30/30，ndiff 11 不变）：ORIG 的
// `mov -0xc(%ebp),%eax` 先于常量槽 store 属 P75 跨版本装载时刻差，
// 非 receiver 具名性可翻 ⇒ 维持原形、判 P75/N7 封顶。
// [L11 复核 2026-09-11 · 封顶] receiver 槽 -0xc **两侧同址**（非槽差）；残余 15 行
//   全为 4.1「先载 receiver 再 store 实参」vs 4.4「先 store 实参再载 receiver」
//   的 call-expansion 顺序（P75）⇒ 跨版本固有，勿再试引用/裸 cast 形（已实测同产）。
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
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}