// ---- getStr(std::string&)（ORIG 0x8ad00d0）：按字符串表索引取串。----
// [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：跨版本 -O2 共享 `return false` 块布局。
// 本 TU 路由 4.4.6 -O2 -std=gnu++0x（dnf_game_cxx.sh: *Game/CompiledDNFLexCore.cpp）。
// ORIG 0x8ad00d0 把 fail 块放检查链中段（`cmp $0x9; jne more` 的 fall-through），
// 4.4.6 -O2 恒放函数尾并把该判断反转为 `je fail`。六形实测均不可翻：
// ①goto fail/more ②短路 && 链 ③独立守卫 ④显式 return+goto ⑤__builtin_expect(0/1)
// ⑥per-function optimize("no-reorder-blocks")（退化为 -O0 形态）。
// 比较序 9/4/0/1 与全部指令逐条相同，仅块位置差 2 处 ⇒ 语义等价，N 封顶。
// [b28d-solo 复验] 32 vs 32，ndiff 7（全 TU 最小残余，仅 fail 块 3 行位置差）。
// 本波补测：`__builtin_expect(d<=1,1)` 只把 fail 块搬到第一守卫后（成 `ja more`
// 反向布局，2 处跳转方向差），`expect(...,0)` 与原形逐条相同 ⇒ 保持自然形。
bool CompiledDNFLex::getStr(std::string& stringBuffer)
{
    int tok = lastToken_;
    unsigned d = (unsigned)tok - 2;
    // [b28d-G11 负结论] ORIG 08ad00e2/08ad00e7：`jbe 08ad00e9` + `jne 08ad00f0`
    // ⇒ 共享 return-false 块 08ad00e9 物理紧跟第 2 判定、body 在 +0x20；
    // 4.4.6 -O2 恒把该块排到函数尾（全 `je` 形）。实测三形同形（fresh .o 32/32、
    // diff 8 行恒定）：① 平铺独立守卫；② `__builtin_expect(tok==PAIR_INT,0)`
    // 与 `__builtin_expect(d<=1,0)`（P306 isEnd 法）；③ 正条件包裹 /
    // `d<=1 || tok==PAIR_INT` 合并短路。⇒ 块布局由后端 reorder 决定，N 封顶。
    // [J10 复验 2026-09-11] E1/E5 两形（`expect(d<=1,0)` 与正向包裹
    // `if (d>1){body; return …;} return false;`）fresh .o 与基线**逐字节相同**
    // （33/33、ndiff 22 不动）；E4（三守卫合并 || 链）把检查序由 ORIG 的 4/0/1
    // 变 0/1/4 反劣 ⇒ 全部回滚，维持 N（-O2 reorder 无源码解）。
    if (d <= 1)
        return false;
    if (tok == DNF_TOK_PAIR_INT)
        return false;
    if (tok == DNF_TOK_FLOAT)
        return false;
    if (tok == DNF_TOK_EOF)
        return false;
    if (tok == DNF_TOK_ERROR)
        return false;
    int index = readIntFromBuffer(offset_);
    return stringManager_->getString(index, &stringBuffer);
}