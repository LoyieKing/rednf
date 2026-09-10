long long CAdvanceAltarLog::getIndex(long long key)
{
    // ORIG 0x08157314..+0x26：sub $0x8 + mov eax,edx / mov $0,eax 死序 + sar 0x1f
    // ——4.1 对 (int)key→(long long) 截断-扩链的死序残留。探针三形（无局部/
    // 单 int 局部/单 ll 局部）：4.4 恒 sub $0x18 参数槽拷贝或丢 sar 尾 → N 封顶。
    // [b28c-27] 复核：N18a（4.1 冗余 regmove 不可造）；本形 16/12 条残差 4 条全为
    // 死序（`mov -0x4,%edx; mov %eax,%edx; mov $0,%eax; mov %edx,%eax`）。
    return static_cast<int>(key);
