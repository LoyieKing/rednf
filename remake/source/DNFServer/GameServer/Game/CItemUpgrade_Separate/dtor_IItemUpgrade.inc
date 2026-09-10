IItemUpgrade::~IItemUpgrade() {}                              // ORIG T 0x080fc842（非虚）

// ORIG T 0x080fc850：put_header(1,cmd) → 虚调槽 0 → 0=finalize+Send / 否则错误包。
// [b28d-solo-CItemUpgrade_Separate] AE：`while (result == 0) { …; return; }` + 尾随
// else 体复现 ORIG 的 `cmpl $0; sete %al; test %al,%al; je <else>` 物化 + then 尾
// 共享尾 jmp（探针 /tmp/b28d_51/p7.cpp g 逐条全等；if/else 形 4.4 恒折 jne 直跳）。
// 虚调目标 %edx 亦已对齐（原 N7 残余随 S125 CDataManager.h static 化后消失）。
