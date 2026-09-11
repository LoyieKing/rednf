// ---- DNFLex::get_next_token（ORIG 0x8acdaa0）----
// [b28d-19] N：N7/N82 —— 4.1 callee-saved 栈帧头保存形 + 寄存器分配序差，块内条数/助记符集全等；ORIG 0x08acdaa0。
int DNFLex::get_next_token()
{
    int tok = 0;
    if (stream_stack.end().base() != stream_stack.begin().base())
    {
        tok = dnf_yylex(this);
        // [P8S2 2026-09-11 复核 · p167「EXTRA sprintf」+ §6.5 串 O-only 同源判定]
        // 我方本函数确有两条错误串（.o 字节实证：`.rodata.str1.1` off 4816
        // `%s - %s/%s, line %d, PushInputStream failed.`、off 4864 `…invalid inclusion.`，
        // 即 strall 的 O-only 系分区段符号（`.rodata.str1.1` 而非 `.LC*`）漏检假阳性）。
        // p167 报「我方多 1 次 sprintf」的真相 = 4.1 **tail-merge 了公共 call 点**：
        // ORIG 第二分支 0x8acdc22 `jmp 0x8acdb87` 复用第一分支的
        // `movl $0x8e2ebfc,0x8(%esp)` + 0x8acdb8a `call sprintf@plt`（全函数仅 1 处
        // sprintf，两处 format 0x8e2eb44/0x8e2eb6c 分别经 0x4(%esp) 设置后跳共享 call）；
        // 我方 4.4 两分支各自发射 sprintf（.o 段内 a4b、ade 两处 R_386_PC32 sprintf）。
        // 源码层无等价形可造（合并需把 toMbcs×2 与 yylineno 装载也提到公共点，
        // 而 ORIG 各分支内独立调用 toMbcs 0x8acdb53/63、0x8acdbea/fc）⇒ 维持 N。
        if (allow_inclusion_)
        {
            if (tok == DNF_TOK_INCLUDE)
            {
                stream_t* cur = GetInputStream();
                int next = dnf_yylex(this);
                if (next == DNF_TOK_STRING)
                {
                    if (PushInputStream("", toTString(strTokBuf), 0))
                        return dnf_yylex(this);
                    std::sprintf(errBuf,
                                 "%s - %s/%s, line %d, PushInputStream failed.",
                                 __FUNCTION__, toMbcs(cur->dir),
                                 toMbcs(cur->filename), yylineno);
                }
                else
                {
                    std::sprintf(errBuf,
                                 "%s - %s/%s, line %d, invalid inclusion.",
                                 __FUNCTION__, toMbcs(cur->dir),
                                 toMbcs(cur->filename), yylineno);
                }
                LexerError(errBuf);
                // [b28e-M1S2] N：ORIG 0x08acdba0（LexerError 出口）有**独立 epilogue**
                // （mov %esi,%eax / 三个 callee-saved 恢复 / mov %ebp,%esp / pop %ebp /
                // ret，共 7 条 + 其后 nop），我方 4.4-O2 把两条 return 的 epilogue
                // tail-merge 成单一共享出口（+0x3e）⇒ 少 4 条；LD 试「同具名 tok 返回」
                // 形（tok = DNF_TOK_ERROR; return tok;）读数不变（108/112）⇒ 回滚。
                // 同件另一处 N7 寄存器差（ORIG `cmp $0xb,%esi` / 我方 `cmp $0xb,%eax`）。
                return DNF_TOK_ERROR;
            }
            if (tok == DNF_TOK_EOF)
            {
                if (PopInputStream(false))
                    return dnf_yylex(this);
            }
        }
    }
    return tok;
}