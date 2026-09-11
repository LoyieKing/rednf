// b28d-solo-Dispatchers verdict=NEAR(routed) [S206 收敛 DIFF→NEAR] ORIG 0x0825fabc
//   [P0/P211/P205] 已修：删 msg 指针局部（形参槽直用）、first 去预清零、+0xd 改具名
//   视图（disp 直存）、循环地址改 `msg+(i+4)*4+1` 加法序；残余仅寄存器/槽名差
//   （P75/P112 结构性，助记符序列已全等）。
int Dispatcher_SecurityLog::read(PacketBuf& packet, MSG_BASE& msg)
