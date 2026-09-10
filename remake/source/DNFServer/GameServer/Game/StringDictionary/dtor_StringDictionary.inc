StringDictionary::~StringDictionary()
{
    // [b28d-48] N93/N91 封顶（NDIFF 73，84/73 条）：ORIG 无迭代器默认构造
    // （begin/end 直接调）、4.4 多 setne 物化与默认构造槽；P50 形实测反劣已回退。
    // [b28d-48] P50 先声明后赋值形实测反劣（73→84→93 条），回退 for 形；
    // `*it != 0` 改 `*it` 消 setne 物化（ORIG 0x8ab22be 为 test+je 直判）。
    // [L6 round3 · P141 实测] 删显式 `if (*it)` 包裹：ORIG 只有 `delete[]` 自带的
    // 一次空指针守卫（`test %eax,%eax; je`），我方显式 if 使其物化为
    // `test; setne %al; test %al,%al; je`（OURS-only setne/test×2/je + 3 call，+11 条）。
    for (std::vector<unsigned char*>::iterator it = blocks_.begin();
         it != blocks_.end(); ++it)
    {
        delete[] *it;
    }
    entities_.clear();
    blocks_.clear();
