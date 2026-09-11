bool Arad_Script::ExpandRow()
{
    // [b28d-solo] ORIG 0x0817b892 shl 段为 `mov %eax,%edx; shl $0x7,%edx`（先搬后移，
    // 与 Create 的 `shl;mov` 相反）⇒ RHS 与 int 字段**同型**（int 常量 128，探针 gd）；
    // `* 128u` 异型反产 `shl;mov`（探针 ge）。memset size 段 ORIG `mov m,%eax;
    // sub nsz,%eax; mov %eax,%edx` 为 int-int 减法（字段改 int 后 cast 可去）。
    rows_ = rows_ * 2;
    bufBytes_ = rows_ * cols_ * 128;
    char* n = new char[bufBytes_];
    if (n == 0)
        return 0;
    int nsz = bufBytes_;
    std::memset(n + nsz, 0, (size_t)(bufBytes_ - nsz));
    std::memcpy(n, cells_, (size_t)nsz);
    delete[] cells_;
    cells_ = n;
    return 1;
}