// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
PackSet::index_vector_it PackSet::find(char const* name)    // sret @0x08ac4e00
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac4e00 size 0x22b）：
    // ①ORIG `lea -0x64; call pair<it,it>::C1Ev` 后 `aSEOS9_`（右值赋值）⇒ er
    //   先默认构造、equal_range 结果经临时槽赋回（P28/P50 两段式）；旧形
    //   `er = equal_range(...)` 初始化式被 4.4 折叠，缺 C1Ev 与临时槽。
    // ②ORIG 无 code 预清（直接 `call _hash; mov %eax,-0x40`）⇒ 原码 `hash_t code;`
    //   无初始化（P171 照抄不写）。
    // [2026-09-11 L2Free4to15.S4 复核] fresh .o：orig=180 our=180、gap=0、AE ndiff=**0**
    //   （clean 口径全等）；strict 残差全部为「.o 未解析本地 call 标签」
    //   （`call <_ZN7PackSet4findEPKc+0x15>` vs 链接后符号名，N124）⇒ AE 即 .o 阶段上限，
    //   p167 CALLSITE 仅 C1/C2·D1/D2 同址别名。无需再改。
    hash_t code;
    IndexEntry dummy;
    std::pair<index_vector_it, index_vector_it> er;
    char file[260];

    _correct_filename(name, file, 0x104, 0, 0);
    code = _hash(toMbcs(file));

    er = std::equal_range(m_indexes.begin(), m_indexes.end(),
                          index_pair_t(code, dummy), &_index_less);

    // b28d-solo-PackSet（P167 判据）：ORIG 第一个循环调 ptEv（`it->`）而非 deEv
    // ⇒ 原码用 `->` 访问（`(*it).m` 会多实例化 operator* 且 p167 EXTRA deEv）。
    if (er.first == m_indexes.end() || er.first->first != code)
    {
        return m_indexes.end();
    }

    // b28d-solo-PackSet：ORIG +0x181 有独立 `iterator::C1Ev` 后 `it2 = er.first`
    // ⇒ 原码先声明后赋值（P50/P28 两段式）；初始化式形被 4.4 折叠。
    index_vector_it it2;
    it2 = er.first;
    // b28d-solo-PackSet（P283）：ORIG init 后 `jmp` 到底部条件块（cond 在 inc 之后），
    // 4.4 对 `for(;;)` 恒把 cond 置顶 ⇒ 用 goto 尾标签复现。
    goto find_cond;
find_body:
    if (strcmp(it2->second.name_.c_str(), file) == 0)
    {
        return it2;
    }
    ++it2;
find_cond:
    if (it2->first == code && it2 != er.second)
    {
        goto find_body;
    }
    return m_indexes.end();
}