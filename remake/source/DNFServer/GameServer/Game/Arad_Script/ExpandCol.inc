bool Arad_Script::ExpandCol()
{
    // [b28d-solo] ORIG 0x0817b816 `mov 0x4(%eax),%eax; mov %eax,%edx` 载入中转 +
    // `shl $7,%eax; mov %eax,%edx`：RHS 须与 int 字段异型（unsigned 移位，探针 ga）。
    cols_ = cols_ + 0x20;
    bufBytes_ = (unsigned int)(rows_ * (unsigned int)cols_) << 7;
    char* n = new char[bufBytes_];
    if (n == 0)
        return 0;
    std::memset(n, 0, (size_t)bufBytes_);
    delete[] cells_;
    cells_ = n;
    return 1;
}