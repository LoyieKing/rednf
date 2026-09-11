// ---- getStreamName（ORIG 0x8ad0290）：NRVO 槽上 copy dir 再 append(filename)；空/空指针 "NONE"。----
// [b28d-solo-CompiledDNFLexCore 判词·N（残余）/已修 NRVO] 4.4.6 -O2 在「混合 return」
// 下禁用 NRVO（P306 配套负例）。ORIG 0x8ad0290 主路径把 `std::string r(p->dir)` 直接
// 构造在 sret（C1ERKSs this=sret）后 append；两条 "NONE" 路径走临时 RVO（C1EPKc
// this=sret）。本波突破口（新范式，探针 /tmp/probe_src/Game/CompiledDNFLexCore.cpp
// f1..f5 实证）：**单一 return + 具名 result + 三目初始化** 即恢复 NRVO——
//   `bool none=…; std::string result = none ? std::string("NONE") : std::string(p[-1].dir);
//    if (!none) result.append(p[-1].filename); return result;`
// 4.4.6 把两个三目分支各自直接构造到 sret（C1EPKc / C1ERKSs），与 ORIG 逐条同形：
// 83 条→49 条，ndiff 78→33。p 保持 finish 不自减（`p[-1]` 两次独立取址，对齐 ORIG
// 的 `sub $0x14,%eax`/`sub $0x10,%esi` 分算）。残余（跨版本 -O2 寄存器分配/块调度）：
// ①帧 0x38 vs 0x28（ORIG 两个 NONE 块各占一个 allocator 临时槽 -0x9/-0xa，三目合并
// 后仅 -0x9；`char framePad[0x10]` 被 -O2 DCE，无效）；②ORIG `mov %esi,%eax;
// sub $0x14,%eax` 保留 esi=finish、我们直接在 esi 上减（N7 族 RA）；③主路径 epilogue
// 落点。语义等价 ⇒ 残余 N 封顶。
// [J10 复验 2026-09-11] E6 落地：改「两级三元 + 单一 return」后两处 ORIG NONE 构造点
// （0x8ad0328 栈空 / 0x8ad0300 null 流，各自 allocator 槽 -0x9/-0xa）都出现，
// p167 调用点 **7=7 CLEAR**（残余仅 C1/C2、D1/D2 同址别名伪差，P93）；指令数
// 51→64（ORIG 62，gap −11→+2）。仍 DIFF：`sub $0x14,%esi` vs ORIG
// `mov %esi,%eax; sub $0x14,%eax`、`add $0x4,%esi` CSE vs ORIG `sub $0x10,%esi` 分算、
// 帧 0x28 vs 0x38 ⇒ 语义完成、byte 层 N（-O2 RA/调度）。
std::string CompiledDNFLex::getStreamName() const
{
    // [G11-LaneCompiledDNFLexCore 2026-09-10 试形·已回滚] 曾按 ORIG 0x8ad0290 的
    // 两处独立 `_ZNSsC1EPKcRKSaIcE`（08ad0312 用 allocator 槽 -0xa、08ad033a 用 -0x9，
    // 各自出口 08ad0322/08ad033f）改写为「两条 return std::string("NONE") 早返回 +
    // 去具名 node」⇒ fresh 读数 **81 条 vs 原 49 条（ORIG 60）严重劣化** ⇒ 回滚，
    // 维持 `bool none` + 三元合并原形（N108/deque 族：4.4 恒内联 deque 访问器）。
    stream_t* p = streamStack_.end()._M_cur;
    // [J10 E6 落地 2026-09-11] 拆成两个独立 NONE 构造点（ORIG 0x8ad0328 栈空 /
    // 0x8ad0300 null 流各一，各自 sret 直构 C1EPKc），仍保持单一 return 以保 NRVO
    // （P306 负例：混合 return 在 4.4.6 -O2 下禁用 NRVO）；p167 由 6→7 calls
    // = ORIG 7（CLEAR，仅同址别名 C1/C2、D1/D2 伪差）。
    bool stack_empty = true;
    bool null_stream = false;
    if (p != streamStack_.begin()._M_cur)
    {
        stack_empty = false;
        stream_t** node = streamStack_.end()._M_node;
        if (p == streamStack_.end()._M_first)
            p = *(node - 1) + STREAM_NODE_ELEMS;
        if (p - 1 != 0)
        {
        }
        else
            null_stream = true;
    }
    std::string result = stack_empty ? std::string("NONE")
                        : null_stream ? std::string("NONE")
                                      : std::string(p[-1].dir);
    if (!stack_empty && !null_stream)
        result.append(p[-1].filename);
    return result;
}