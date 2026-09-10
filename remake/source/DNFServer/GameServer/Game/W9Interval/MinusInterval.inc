void IntervalSet::MinusInterval(const Interval& interval) // T @0x08573394
{
    // ORIG 控制流：
    //   遍历元素 iv，r = intersect(iv, sub)；不相交 → ++
    //   完全覆盖（r==iv）→ erase(it)，erase 返回值作新 it，直接回到 end 判断（无 ++）
    //   r.start==iv.start       → 当前槽改 {r.end, iv.end-r.end}，++
    //   r.end==iv.end           → 当前槽改 {iv.start, r.start-iv.start}，++
    //   中挖                    → 头部残留写回当前槽，++ 后 insert 尾部残留，再 ++
    // [P0 槽-局部双向对应律] ORIG 函数头序：`__normal_iterator()`@0x085760d4 默认构造
    // it(-0x14) → `Interval::Interval()`@0x08575e58 ×3（-0x1c/-0x24/-0x2c）→
    // `begin()`@0x085760e2 结果拷入 it。⇒ it/ret/temp/iv 四个具名局部均声明在**函数
    // 作用域**（循环内每次迭代不再构造），循环内 `iv = *it` 是 trivial 拷贝赋值。
    // [单一 temp 复用] 三个分支同用 -0x24/-0x20 槽写新区间：分支2 写 {r.end, iv.end-r.end}，
    // 分支3/中挖 写 {iv.start, r.start-iv.start}，中挖再覆盖为尾部残留 {r.end, iv.end-r.end}
    // 并直接作 `insert` 的源（`insert(...,&local_24)`@0x08573592）⇒ 原码只有一个临时
    // Interval 具名局部。
    // [丢弃写入判据] ORIG 中挖分支：`operator++(int)`@0x08576248（后置，返回值入 -0xc 后
    // 丢弃）→ `insert`@0x0857627c → **`mov -0x3c,%eax; mov %eax,-0x14`**（it = insert 返回
    // 迭代器，0x0857359a）→ 外层 `operator++()`@0x085763aa。原码为
    // `it++; it = m_intervals.insert(it, temp);`——旧码丢 insert 返回值 ⇒ it 失效后仍自增。
    // [b28d-lane15 判词·N（P71 空析构省略跨版本差）] ORIG 0x08573394 对 iv/ret/tail
    // 三个 Interval 局部**不调 D1**（尾部 +0x246 leave/ret 直落），4.4 恒调 3 处
    // D1 + 布 EH cleanup（223/202 形均试，ORIG 183）。ORIG 的 D1 符号仅被
    // `new_allocator<Interval>::destroy` 引用（0x08577566，全仓唯一调用点）；删用户
    // 析构会连带丢该 W 符号与 destroy 实例形态 ⇒ 无源码解，封顶。
    // 另：函数头四对象槽序 -0x14/-0x1c/-0x24/-0x2c 与 ORIG 一致，残差全为析构调用。
    std::vector<Interval>::iterator it;
    Interval ret;
    Interval temp;
    Interval iv;
    it = m_intervals.begin();
    while (it != m_intervals.end())
    {
        // ORIG 逐字段两次 `operator*`@0x08576158（085733ec/085733f6）⇒ 原码为两个
        // `(*it)` 表达式（`iv = *it` 只会解引用一次，且 4.4 的 `it->` 走 operator->）。
        iv.m_start = (*it).m_start;
        iv.m_size = (*it).m_size;
        // ORIG `test %al,%al; je 085735a0`（不相交直接落 ++it）⇒ 原码为
        // `if (IntersectInterval(...)) { ...三种情况... }` 包裹式，非 early-continue。
        if (IntersectInterval(&ret, iv, interval))
        {
            if (ret.m_start == iv.m_start && ret.m_size == iv.m_size)
            {
                it = m_intervals.erase(it);             // 完全覆盖 → 无 ++
                continue;
            }

            if (ret.m_start == iv.m_start)
            {
                temp.m_start = ret.m_start + ret.m_size;
                temp.m_size = (iv.m_start + iv.m_size) - temp.m_start;
                (*it).m_start = temp.m_start;
                (*it).m_size = temp.m_size;
            }
            else if (ret.m_start + ret.m_size == iv.m_start + iv.m_size)
            {
                temp.m_start = iv.m_start;
                temp.m_size = ret.m_start - temp.m_start; // ORIG 085734cf 用 temp.start
                (*it).m_start = temp.m_start;
                (*it).m_size = temp.m_size;
            }
            else
            {
                temp.m_start = iv.m_start;
                temp.m_size = ret.m_start - temp.m_start; // ORIG 0857350a 用 temp.start
                (*it).m_start = temp.m_start;
                (*it).m_size = temp.m_size;

                temp.m_start = ret.m_start + ret.m_size;  // 尾部残留（覆盖 temp）
                temp.m_size = (iv.m_start + iv.m_size) - temp.m_start;
                it++;                                     // ORIG: 后置 ++（0x08576248）
                it = m_intervals.insert(it, temp);        // ORIG: 返回值回写 it（0x0857359a）
            }
        }
        ++it;
    }
