std::string CompiledDNFLex::getStreamName() const
{
    // [G11-LaneCompiledDNFLexCore 2026-09-10 试形·已回滚] 曾按 ORIG 0x8ad0290 的
    // 两处独立 `_ZNSsC1EPKcRKSaIcE`（08ad0312 用 allocator 槽 -0xa、08ad033a 用 -0x9，
    // 各自出口 08ad0322/08ad033f）改写为「两条 return std::string("NONE") 早返回 +
    // 去具名 node」⇒ fresh 读数 **81 条 vs 原 49 条（ORIG 60）严重劣化** ⇒ 回滚，
    // 维持 `bool none` + 三元合并原形（N108/deque 族：4.4 恒内联 deque 访问器）。
    stream_t* p = streamStack_.end()._M_cur;
    bool none = true;
    if (p != streamStack_.begin()._M_cur)
    {
        stream_t** node = streamStack_.end()._M_node;
        if (p == streamStack_.end()._M_first)
            p = *(node - 1) + STREAM_NODE_ELEMS;
        if (p - 1 != 0)
            none = false;
    }
    std::string result = none ? std::string("NONE") : std::string(p[-1].dir);
    if (!none)
        result.append(p[-1].filename);
    return result;
