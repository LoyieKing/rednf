// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DNFLexWrapper/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "DNFLexWrapper.h"
#include "ScriptStringManager.h"
#include "SyncScriptDeps.h"

extern "C" double __strtod_internal(const char*, char**, int);
extern "C" long __strtol_internal(const char*, char**, int, int);

// 构造（ORIG 0x8aced60）：vptr、lastToken_/lastStringValue_/lexType_ 清零，
// 依次构造 dnfLex_（DNFLex）与 compiledDnfLex_（CompiledDNFLex），最后回调
// 字段 +0x6108/+0x610c 清零（+0x08 lastIntValue_ / +0x0c lastFloatValue_ 不写，
// 与 ORIG 汇编一致）。C++ 成员按声明序自动构造子对象。
#include "DNFLexWrapper/DNFLexWrapper.inc"
    : lastToken_(0),
      lastStringValue_(0),
      lexType_(LEX_TYPE_NORMAL),
      lexerError_(0),
      loadStream_(0)
{
}

#include "DNFLexWrapper/setLexType.inc"
{
    lexType_ = t;
}

#include "DNFLexWrapper/getLexType.inc"
{
    return static_cast<ENUM_LEX_TYPE>(lexType_);
}

#include "DNFLexWrapper/getLastToken.inc"
{
    return lastToken_;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace430；P0/§4.3 具名成员直读 dnfLex_.yytext（+0x1c）
// 替代 YYText()（4.4 对弱符号 inline 恒发 call，多 add/mov/call 三条；ORIG 无此 call）。
#include "DNFLexWrapper/getLastTokenData.inc"
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yytext;
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace490；NORMAL 分支直读 dnfLex_.yylineno（+0x24）
// 替代 lineno() call；COMPILED 尾调 GetOffset 与 ORIG 同形。
#include "DNFLexWrapper/getLastLineNo.inc"
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yylineno;
    int r = -1;
    if (lexType_ == LEX_TYPE_COMPILED)
        return compiledDnfLex_.GetOffset();
    return r;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace390；直读 dnfLex_.yytext（同 getConnector/getCustomData）。
#include "DNFLexWrapper/getType.inc"
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yytext;
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace3f0；直读 dnfLex_.yytext（P0 表达式直读，无中间局部）。
#include "DNFLexWrapper/getConnector.inc"
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yytext;
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace3c0；直读 dnfLex_.yytext。
#include "DNFLexWrapper/getCustomData.inc"
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yytext;
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

#include "DNFLexWrapper/getString.inc"
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.tok_str();
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

#include "DNFLexWrapper/getToken.inc"
{
    if (lexType_ == LEX_TYPE_NORMAL)
    {
        lastToken_ = dnfLex_.get_next_token();
        return lastToken_;
    }
    if (lexType_ == LEX_TYPE_COMPILED)
    {
        CompiledDNFLex* c = &compiledDnfLex_;
        int tok = c->getToken();
        lastToken_ = tok;
        if ((unsigned)tok <= 1)
            return tok;
        unsigned d = (unsigned)tok - 2;
        if (d <= 1 || tok == DNF_TOK_PAIR_INT)
        {
            lastIntValue_ = c->getInt(0);
            return lastToken_;
        }
        if (tok == DNF_TOK_FLOAT)
        {
            lastFloatValue_ = c->getFloat(0);
            return lastToken_;
        }
        lastStringValue_ = c->getStr((bool*)0);
    }
    return lastToken_;
}

// [b28d-solo-DNFLexWrapper] N 封顶（跨版本寄存器分配 + 块布局，N7/N18a/N82 族）：ORIG 0x08ace2f0
// （-O2，68 insn）把 c 分配 %ecx 并复用为 d（`movsbl %cl,%ecx; sub $0x30,%ecx`），4.4.6 恒分配
// %eax（`movsbl %al,%eax; lea -0x30(%eax),%ecx`，mnem-ndiff 26/64）；v=0 被 4.4 下沉到 `ja` 之后
// （ORIG 在 parse 入口）、minus 块排在 skip_space 之前、maybe_neg 就地复制 epilogue
// （ORIG `pop/pop/ret` vs 我们 `jmp` 复用）。
// 已试（均 ≥26，最佳即当前 while 形）：optimize 属性矩阵 O0/O1/O2/O3 + no-if-conversion/
// no-omit-frame-pointer/no-crossjumping/no-thread-jumps/no-tree-sink/no-reorder-blocks/
// no-schedule-insns2/ira-algorithm=cb/ira-region=one|all/no-tree-pre/no-tree-ccp、变量声明序/
// register/signed char、v 位置三形（+v/dthenv/for）、Ghidra do-while 结构重写、三目/negassign。
// P167 CLEAR(0/0) 语义等价；§6.1 对齐 nop 已剥。
__attribute__((optimize("O2,no-if-conversion,no-omit-frame-pointer")))
#include "DNFLexWrapper/getDecimal.inc"
{
    char* p;
    char c;
    bool neg;
    int v;
    int d;

    if (lexType_ == LEX_TYPE_NORMAL)
    {
        p = dnfLex_.yytext;
        c = *p;
        if (c == '\0')
            goto ret0;
    loop:
        if (__builtin_expect((unsigned char)(c - 0x30) <= 9, 0))
            goto digit_start;
        if (__builtin_expect(c == ' ', 0))
            goto skip_space;
        if (__builtin_expect(c == '+', 0))
            goto plus;
        if (__builtin_expect(c == '-', 0))
            goto minus;
        goto ret0;
    }
    else if (lexType_ == LEX_TYPE_COMPILED)
        return lastIntValue_;
ret0:
    return 0;
skip_space:
    p = p + 1;
    c = *p;
    if (c != '\0')
        goto loop;
    goto ret0;
minus:
    p = p + 1;
    neg = true;
    c = *p;
    goto parse;
plus:
    p = p + 1;
    neg = false;
    c = *p;
    goto parse;
digit_start:
    neg = false;
parse:
    v = 0;
    d = (int)c - 0x30;
    while ((unsigned)d <= 9)
    {
        p = p + 1;
        v = v * 10 + d;
        d = (int)*p - 0x30;
    }
    if (neg)
        return -v;
    return v;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace780；sscanf 实参直读 dnfLex_.yytext + 2
// （替代 YYText() call），COMPILED 返回 lastIntValue_。
#include "DNFLexWrapper/getHexDecimal.inc"
{
    if (lexType_ == LEX_TYPE_NORMAL)
    {
        int v = 0;
        std::sscanf(dnfLex_.yytext + 2, "%x", &v);
        return v;
    }
    int r = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastIntValue_;
    return r;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace730；__strtod_internal 实参直读 dnfLex_.yytext
// （替代 YYText() call），COMPILED 返回 lastFloatValue_。
#include "DNFLexWrapper/getFloat.inc"
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return (float)__strtod_internal(dnfLex_.yytext, 0, 0);
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastFloatValue_;
    return 0.0f;
}

// getPairInt（ORIG 0x8ace630）：NORMAL 对 yytext+1 拷贝 0x100 字节并保证终止后
// strtol base 10（跳过成对 token 前导字符，如 "=123"）；COMPILED 返回
// lastIntValue_；其它 lexType 返回 0。
// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace630；-O2 路由下 4.4 内联 strlen 为 word-at-a-time
// （与 ORIG 同形），残余 cmov 段由 target("arch=i486") 消除（P230①）⇒ 54=54 逐条同形（P167 CLEAR 2/2）。
__attribute__((target("arch=i486")))
#include "DNFLexWrapper/getPairInt.inc"
{
    // 语义：NORMAL 用 yytext 成员直读（免 YYText() call），strncpy 0x100 + strlen 截尾 + strtol。
    if (lexType_ != LEX_TYPE_NORMAL)
    {
        int r = 0;
        if (lexType_ == LEX_TYPE_COMPILED)
            r = lastIntValue_;
        return r;
    }
    char buffer[0x100];
    std::strncpy(buffer, dnfLex_.yytext + 1, 0x100);
    buffer[std::strlen(buffer) - 1] = 0;
    return (int)__strtol_internal(buffer, 0, 10, 0);
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace5a0；target("arch=i486") 消 cmov ⇒ 41=41 逐条同形
// （.bss 静态 buffer 的 R_386_32 绝对地址按 AE 口径归一 <A>；P167 CLEAR 1/1）。
__attribute__((target("arch=i486")))
#include "DNFLexWrapper/getPairStr.inc"
{
    // 语义：NORMAL 用静态 buffer + yytext 成员直读；COMPILED 返回 lastStringValue_。
    static char buffer[0x100];
    if (lexType_ == LEX_TYPE_NORMAL)
    {
        std::strncpy(buffer, dnfLex_.yytext + 1, 0x100);
        buffer[std::strlen(buffer) - 1] = 0;
        return buffer;
    }
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

// [GiBodyLex] AE：ORIG 0x08ace4d0（33 insn）的 `return std::string(pc);` 只发 toTString +
// _ZNSsC1EPKcRKSaIcE + _Unwind_Resume 三个 call。此前本件因 ScanAll×2 的
// `__attribute__((optimize("O0")))` 触发 4.4 TU 级内联抑制而多发 _ZNSaIcEC1Ev×1 +
// _ZNSaIcED1Ev×2（p167 EXTRA=3）；ScanAll×2 已按 R3 迁至 DNFLexWrapperHelpers.cpp
// （ORIG 中与 ScanStrOrInt/ScanAnyData 同属 0x088b 段 -O0 TU），本 TU 内联抑制随之消除。
#include "DNFLexWrapper/getLastTokenDataAsString.inc"
{
    const char* pc;
    if (lexType_ == LEX_TYPE_NORMAL)
        pc = dnfLex_.yytext;
    else
    {
        pc = "";
        if (lexType_ != LEX_TYPE_COMPILED)
            goto construct;
        pc = lastStringValue_;
    }
    if (__builtin_expect(pc == 0, 0))
        goto empty;
    pc = toTString(pc);
    goto construct;
empty:
    pc = "";
construct:
    return std::string(pc);
}

// setStringTableFromBuffer（ORIG 0x8ace560）：转发到 compiledDnfLex_（+0x40bc）
// 的 ScriptStringManager::importFromBuffer(buffer, size)。
#include "DNFLexWrapper/setStringTableFromBuffer.inc"
{
    ScriptStringManager* mgr = compiledDnfLex_.getStringManager();
    return mgr->importFromBuffer(buffer, size);
}

// setStringTableFromFile（ORIG 0x8ace530）：转发到 compiledDnfLex_ 的
// ScriptStringManager::importFromFile(fileName)。
#include "DNFLexWrapper/setStringTableFromFile.inc"
{
    ScriptStringManager* mgr = compiledDnfLex_.getStringManager();
    return mgr->importFromFile(fileName);
}

// SetHandler（ORIG 0x8acec80）：分别转发到 DNFLex::SetHandler 与
// CompiledDNFLex::SetHandler。
#include "DNFLexWrapper/SetHandler.inc"
                               void (*lexerOutput)(const char*, int),
                               void (*lexerError)(const char*),
                               bool (*loadStream)(const char*, const char*,
                                                  DNFLex::stream_data_t*))
{
    dnfLex_.SetHandler(isDbcsLeadChar, lexerOutput, lexerError, loadStream);
    compiledDnfLex_.SetHandler(lexerError, loadStream);
    lexerError_ = (void*)lexerError;
    loadStream_ = (void*)loadStream;
}

// 析构（ORIG 0x8acecf0）：恢复 vptr，析构 compiledDnfLex_ 与 dnfLex_
// （ORIG 对 CompiledDNFLex D1 与 DNFLex D1 按序调用）。C++ 成员按逆声明序
// 自动析构（compiledDnfLex_ 先于 dnfLex_），与 ORIG 顺序一致。
#include "DNFLexWrapper/dtor_DNFLexWrapper.inc"
{
    // 成员析构自动完成（compiledDnfLex_ → dnfLex_）。
}
