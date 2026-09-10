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
