// ORIG getIndex @0x0815730e：返回 (long long)(int)key（低 32 位符号扩展；
// mov $0,%eax 后 mov %edx,%eax 残留，sar $0x1f 符号扩展为 64 位）。
// [b28d-lane6] DIFF·N18a：ORIG 0x0815730e 的 (int)key→long long 截断-扩链残留 4 条死序（mov -0x4,%edx; mov %eax,%edx; mov $0,%eax; mov %edx,%eax），4.4 恒折（b28c-27 复核）。
long long CAdvanceAltarLog::getIndex(long long key)
{
    // ORIG 0x08157314..+0x26：sub $0x8 + mov eax,edx / mov $0,eax 死序 + sar 0x1f
    // ——4.1 对 (int)key→(long long) 截断-扩链的死序残留。探针三形（无局部/
    // 单 int 局部/单 ll 局部）：4.4 恒 sub $0x18 参数槽拷贝或丢 sar 尾 → N 封顶。
    // [b28c-27] 复核：N18a（4.1 冗余 regmove 不可造）；本形 16/12 条残差 4 条全为
    // 死序（`mov -0x4,%edx; mov %eax,%edx; mov $0,%eax; mov %edx,%eax`）。
// [VerifyN3 2026-09-11 翻正·P345] 改用**移位链** `key << 32 >> 32`（低 32 位符号扩展）：
// 4.4 直出与 ORIG 逐条全等（16/16），旧 `static_cast<int>` 形恒缺 4 条死序 ⇒ 判词作废。
    return key << 0x20 >> 0x20;
}