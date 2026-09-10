CFatigueBatteryHandle::CFatigueBatteryHandle() {}

// ORIG stFatigueBatteryConstantTable_t::stFatigueBatteryConstantTable_t() 0x0836f2cc
// 指令序：word+0x1a4=1 → word+0x1a6=0 → word+0x1a8=0 → memset(this+0x12,0,0x191)
// （词写与 memset 区间不相交，顺序保持 ORIG 形态）。+0x11 与 +0x1aa 保持未初始化。
