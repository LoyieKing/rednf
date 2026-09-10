bool Arad_Script::Create()
{
    // ORIG 0x0817b72a 头部 test;jne+test;je 双检 = 源形 `if (cells_ == 0)
    // delete[] cells_;`（外层 ==0 守卫 + delete[] 编译器自带 null 守卫）。
    // [b28d-solo] 载入中转由 `(int)(cols_ * (unsigned int)rows_) * 0x80u` 复现
    // （探针 gb）：ORIG 0x0817b768 `mov (%eax),%eax; mov %eax,%edx` +
    // 0x0817b775 `shl $7,%eax; mov %eax,%edx` 逐条全等。
    // [b28d-solo] memset size 实参两段（ORIG 0x0817b7aa `mov 0x10(%eax),%eax;
    // mov %eax,%edx`）由字段 `int bufBytes_` 的 int→size_t 转换复现（探针 fb）；
    // 旧 unsigned 字段恒单段。字段类型为 int 后本函数三处与 ORIG 全等。
    if (cells_ == 0)
        delete[] cells_;
    cols_ = 0x20;
    rows_ = 0x100;
    bufBytes_ = (int)(cols_ * (unsigned int)rows_) * 0x80u;
    cells_ = new char[bufBytes_];
    if (cells_ == 0)
        return 0;
    std::memset(cells_, 0, (size_t)bufBytes_);
    usedCols_ = 0;
    usedRows_ = 0;
    colIndex_.clear();
    rowIndex_.clear();
    return 1;
