void PackSet::freeContent(char const* name)     // 0x08ac5448
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac5448 size 0xa5）：b28c-L8 已修
    // （函数级 iterator 默认构造 + 三处 `(*it)` deEv + 嵌套 if bool 物化）；
    // 本轮 fresh 复验 near=0 且不在 DIFF 清单 ⇒ AE 保持。
    index_vector_it it;   // b28c-L8：ORIG 默认 ctor 前置（P28 两段式）
    it = find(name);
    // b28d-solo-PackSet（P167 判据，修正 b28c-L8 误判）：ORIG 0x08ac5448 三处
    // 成员访问经 `operator->`（nm 实证 0x08ac7a24 = _ZNK…ptEv），**非** deEv；
    // 旧形 `(*it).second` 使 p167 出现 3×MISSING ptEv / 3×EXTRA deEv。
    // 第二条件是独立嵌套 if（ORIG `test; setne %al; test %al,%al; je` bool
    // 物化形）——短路 && 合并形使 4.4 反产 `mov$1;jmp / mov$0` 岛（+3 条）。
    if (it != m_indexes.end())
    {
        if (it->second.content_ != 0)
        {
            delete[] it->second.content_;
            it->second.content_ = 0;
        }
    }
}