// ----------------------------------------------------------------------------
// StringDictionary::~StringDictionary()（0x8ab22be）
//   遍历 blocks_，delete[] 每个非空块；随后 ~vector<StringEntity>、~vector<uchar*>。
// ----------------------------------------------------------------------------
StringDictionary::~StringDictionary()
{
    // [b28d-48] N93/N91 封顶（NDIFF 73，84/73 条）：ORIG 无迭代器默认构造
    // （begin/end 直接调）、4.4 多 setne 物化与默认构造槽；P50 形实测反劣已回退。
    // [b28d-48] P50 先声明后赋值形实测反劣（73→84→93 条），回退 for 形；
    // `*it != 0` 改 `*it` 消 setne 物化（ORIG 0x8ab22be 为 test+je 直判）。
    // [L6 round3 · P141 实测] 删显式 `if (*it)` 包裹：ORIG 只有 `delete[]` 自带的
    // 一次空指针守卫（`test %eax,%eax; je`），我方显式 if 使其物化为
    // `test; setne %al; test %al,%al; je`（OURS-only setne/test×2/je + 3 call，+11 条）。
    // [b28e-S4 回滚] ORIG 0x08ab22c6~0x08ab233c 虽为「begin/end 各一次 + 后缀
    //   it++」，但 for-init 双声明 + it++ 实测 76→**80**（N3 编译读数）：
    //   4.4 在 for-init 双声明下于尾部多发 `~vector<StringEntity>`/`~vector<uchar*>`
    //   两次析构调用（+4 条，ORIG 无）⇒ 该形比原形更远。回滚到 for(cond 内
    //   blocks_.end(), ++it) 形（76 vs ORIG 73 为既有封顶态）。
    // [P6S4 2026-09-11 负结论 · 按 p167fix 修正口径] ①ORIG 0x08ab22ed 只调一次
    // end()（槽 -0x14，循环外提升）⇒ 试 end() 具名缓存形：4.4 反在 EH 清理块多
    // 发两次 ~vector 析构 call，blocks 5→7 ⇒ 回滚。②ORIG 循环尾用 `ppEi`（后置，
    // 带返回临时槽 -0xc）⇒ 试 `it++`：76→80 条（4.4 postfix 临时 + EH 清理块多
    // 2 次析构 call）⇒ 回滚；③`it` 循环外声明 + `++it` 实测与原 for-init 形逐条
    // 相同（76/blocks 5）⇒ 维持原形。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N3/N91/N93）→ 试=①删尾部
    // `entities_.clear(); blocks_.clear();`（ORIG 0x08ab22be 循环外仅两次成员自动
    // 析构：0x08ab234d/0x08ab2361 `add $0x14` 的 `~vector<StringEntity>` 与
    // 0x08ab2389 的 `~vector<uchar*>`；p167 EXTRA `vector<...>::clear()`×2 直证
    // 我方多写 clear）②`++it`→`it++`（ORIG 0x08ab2320 调 `_ppEi`（后置，带返回
    // 临时槽 -0xc 与 `movl $0x0,0x8(%esp)`））③end() 提升为具名 `last`（ORIG
    // 0x08ab22ed 仅一次 end()，槽 -0x14，循环外）⇒ **推翻**：n1_v=AE（73=73 条,
    // gap=0）；TU 四数 49/125/0/1/34 → 49/127/0/0/29（diff 1→0、missing −5）。
    // 关键：P6S4 三项负结论均在**带 clear 行**的旧形上测得（尾部 EH 清理块含
    // clear 临时）；删 clear 后同轴翻转 ⇒ 三项限制不适用于本形。
    std::vector<unsigned char*>::iterator it = blocks_.begin();
    std::vector<unsigned char*>::iterator last = blocks_.end();
    for (; it != last; it++)
    {
        delete[] *it;
    }
}