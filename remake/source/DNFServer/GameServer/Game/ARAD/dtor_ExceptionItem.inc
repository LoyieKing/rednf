ExceptionItem::~ExceptionItem() {}

// b28d-solo-ARAD verdict=IDENTICAL/AE：P283 goto 尾标签复现 4.1「begin 后 jmp 到底部
// 条件块」；具名 `int idx` 由块内提至函数级 + 具名 insert 结果 `res`（P212/P247 槽序
// 旋钮）后槽位与 ORIG 全对齐（idx -0x20 / insert sret -0x28 / .second -0x24）。
