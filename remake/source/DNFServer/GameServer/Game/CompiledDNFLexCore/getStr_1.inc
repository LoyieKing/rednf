// ---- getStr(char*, int)（ORIG 0x8ad0190）----
// [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：与 getStr(std::string&) 同族。
// ORIG 0x8ad0190 的 fail 块位于 `cmp $0x9; jne more` 的 fall-through（检查链中段），
// 4.4.6 -O2 恒反转为 `je fail` 并置于函数尾；overflow 路径 memcpy 实参装载序亦反
// （ORIG size,dest,src vs 我们 size,src,dest，P75 跨版本调度）。比较序 9/4/0/1 与
// 全部指令逐条相同，仅块位置/装载序差 ⇒ 语义等价，N 封顶（同 getStr 六形实测）。
// [b28d-solo 复验] 66 vs 61，ndiff 22。本波补测：overflow 的 memcpy 改用
// `r_string->c_str()` 内联实参 / 具名 `int n=maxSize-1` 两形均不改装载序
// （仍 size,src,dest；且局部 n 被 -O2 折叠）⇒ 维持 ORIG 形不可控的 P75 判词。
// [J10 复验 2026-09-11] p167 **3=3 CLEAR**（0 MISSING/0 EXTRA）⇒ 调用面闭合；
// fresh 68 vs 63（gap −5：ORIG overflow 独立 epilogue 6 条 vs 我方 tail-merge 的
// `jmp` 1 条；ORIG fail 块在 `cmp $0x9; jne body` 中段 vs 我方尾块；overflow memcpy
// 装载序 dst-first vs src-first，P75）。残余全为 -O2 块合并/调度 ⇒ 语义完成、byte 层 N。
bool CompiledDNFLex::getStr(char* buffer, int maxSize)
{
    int tok = lastToken_;
    unsigned d = (unsigned)tok - 2;
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
    const std::string* r_string = stringManager_->getString(index);
    const char* src = r_string->c_str();
    int string_length = (int)r_string->size();
    if (__builtin_expect(string_length > maxSize, 0))
        goto overflow;
    std::memcpy(buffer, src, (unsigned)string_length);
    buffer[string_length] = 0;
    return true;
overflow:
    std::memcpy(buffer, src, (unsigned)(maxSize - 1));
    buffer[string_length - 1] = 0;
    return false;
}