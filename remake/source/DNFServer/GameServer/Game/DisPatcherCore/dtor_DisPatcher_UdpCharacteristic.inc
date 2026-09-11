// [b28d-29] P251/P208：ORIG 0x0821ac7c 为 switch(local_e) 稀疏树（ushort 入口截断槽 -0xa 双段）。
// [Lane38 2026-09-10] 已按 switch 复现（case 0/1/2 物理序 = ORIG，fall-through = case 0）：
//   jcc 目标差由 −106/+134/+133 收敛为单常数 −3（结构对齐），228→233（残 N82 帧 0x58 vs 0x44）。
DisPatcher_UdpCharacteristic::~DisPatcher_UdpCharacteristic() {}
