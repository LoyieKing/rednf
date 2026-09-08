// ============================================================================
// DNFLexWrapper 全局词法包装器（ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLexWrapper.md）
// ----------------------------------------------------------------------------
// 本 TU 提供：
//   getPairInt()              _ZN13DNFLexWrapper10getPairIntEv        ORIG 0x8ace630
//   getPairStr()              _ZN13DNFLexWrapper10getPairStrEv        ORIG 0x8ace5a0
//   getLastTokenDataAsString() _ZN13DNFLexWrapper24getLastTokenDataAsStringEv
//                                                                ORIG 0x8ace4d0
//   setStringTableFromBuffer() _ZN13DNFLexWrapper24setStringTableFromBufferEPKhi
//                                                                ORIG 0x8ace560
//   setStringTableFromFile()  _ZN13DNFLexWrapper22setStringTableFromFileEPKc
//                                                                ORIG 0x8ace530
//   SetHandler(...)           _ZN13DNFLexWrapper10SetHandlerEPFbcEPFvPKciEPFvS3_EPFbS3_S3_PNS_13stream_data_tEE
//                                                                ORIG 0x8acec80
//   DNFLexWrapper()           _ZN13DNFLexWrapperC1Ev/C2Ev      ORIG 0x8aced60
//   ~DNFLexWrapper()          _ZN13DNFLexWrapperD1Ev/D2Ev      ORIG 0x8acecf0
//   ~DNFLexWrapper()          _ZN13DNFLexWrapperD0Ev           ORIG 0x8aced40
// vtable/typeinfo（_ZTV13DNFLexWrapper / _ZTI13DNFLexWrapper / _ZTS13DNFLexWrapper）
// 由编译器从 DNFLexWrapper 类生成（虚析构，槽 0=D1、槽 1=D0）。
//
// 字段布局依据 ORIG DNFLexWrapper::DNFLexWrapper（0x8aced60），见 DNFLexWrapper.h：
//   +0x00 vptr   +0x04 lastToken_   +0x08 lastIntValue_
//   +0x0c lastFloatValue_           +0x10 lastStringValue_   +0x14 lexType_
//   （lexType_：0=NORMAL 文本流 / 1=COMPILED 编译态）
//   +0x18 dnfLex_（DNFLex 子对象）/+0x40bc compiledDnfLex_（CompiledDNFLex
//   子对象）/+0x6108 LexerError_ / +0x610c LoadStream_，总大小 0x6110。
// ----------------------------------------------------------------------------
// [b28d-solo-DNFLexWrapper] _ZNK9FlexLexer6YYTextEv 不属本 TU：ORIG 该符号 @0x080950c6
// （__slang_filter__FlexLexer 段 0x08093xxx..0x08095xxx，由 SlangFilter.cpp 的 YYText() 调用点
// 发射）；本 TU 全部 getter 按 ORIG 直读 dnfLex_.yytext / dnfLex_.yylineno（见各函数判词），
// 不再发射该弱符号（改动前本 TU 因调用它而发射，body 的 pop/load 序与 ORIG 相反）。
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "DNFLexWrapper.h"
#include "ScriptStringManager.h"
#include "SyncScriptDeps.h"
#include "RDARScriptStringManager.h"   // g_scriptStringManager_（ScanAll pair 分支）

extern "C" double __strtod_internal(const char*, char**, int);
extern "C" long __strtol_internal(const char*, char**, int, int);

DNFLexWrapper g_dnfLex;

void DNFLexWrapper::setLexType(ENUM_LEX_TYPE t)
{
    lexType_ = t;
}

DNFLexWrapper::ENUM_LEX_TYPE DNFLexWrapper::getLexType()
{
    return static_cast<ENUM_LEX_TYPE>(lexType_);
}

int DNFLexWrapper::getLastToken()
{
    return lastToken_;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace430；P0/§4.3 具名成员直读 dnfLex_.yytext（+0x1c）
// 替代 YYText()（4.4 对弱符号 inline 恒发 call，多 add/mov/call 三条；ORIG 无此 call）。
char* DNFLexWrapper::getLastTokenData()
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
int DNFLexWrapper::getLastLineNo()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yylineno;
    int r = -1;
    if (lexType_ == LEX_TYPE_COMPILED)
        return compiledDnfLex_.GetOffset();
    return r;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace390；直读 dnfLex_.yytext（同 getConnector/getCustomData）。
char* DNFLexWrapper::getType()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yytext;
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace3f0；直读 dnfLex_.yytext（P0 表达式直读，无中间局部）。
char* DNFLexWrapper::getConnector()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yytext;
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace3c0；直读 dnfLex_.yytext。
char* DNFLexWrapper::getCustomData()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yytext;
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

char* DNFLexWrapper::getString()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.tok_str();
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

int DNFLexWrapper::getToken()
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
int DNFLexWrapper::getDecimal()
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
int DNFLexWrapper::getHexDecimal()
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
float DNFLexWrapper::getFloat()
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
int DNFLexWrapper::getPairInt()
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
char* DNFLexWrapper::getPairStr()
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

// [b28d-solo-DNFLexWrapper] N 封顶（TU 级 optimize 属性污染，非本函数源码形可解）：
// ORIG 0x08ace4d0（33 insn）的 `return std::string(pc);` 只发 toTString + _ZNSsC1EPKcRKSaIcE +
// _Unwind_Resume 三个 call；本 TU 因 ScanAll×2 的 `__attribute__((optimize("O0")))` 触发 4.4
// 全局内联抑制，额外发射 _ZNSaIcEC1Ev×1 + _ZNSaIcED1Ev×2（p167 EXTRA=3）。
// 证据：同旗标（-O2 -std=gnu++98 + 4.4.6 头集/后端）小 TU 探针（仅 includes+本函数）4.4.6/4.4.7
// 均内联空 allocator ctor（0 个 allocator call）；整删 ScanAll×2 或仅删其 optimize 属性即恢复
// （TU 读数：保留 ae=22/diff=3；删属性 ae=21/diff=3；删 ScanAll ae=21/diff=1 且本件转 AE）。
// 已试无效：本函数加 optimize("O2"/"O3")、`#pragma GCC push_options/optimize("O0")/pop_options`
// 替代 ScanAll 属性（同样污染）、具名 std::string 局部。P167 语义等价。
// 迁移建议已上报 Main（ScanAll ORIG 邻域 0x088bdb56 ScanAnyData/0x088bda1c scanChangeStatusData，
// 不属 DNFLexWrapper 段 0x8ace290..0x8acecf0）。
std::string DNFLexWrapper::getLastTokenDataAsString()
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
bool DNFLexWrapper::setStringTableFromBuffer(const unsigned char* buffer, int size)
{
    ScriptStringManager* mgr = compiledDnfLex_.getStringManager();
    return mgr->importFromBuffer(buffer, size);
}

// setStringTableFromFile（ORIG 0x8ace530）：转发到 compiledDnfLex_ 的
// ScriptStringManager::importFromFile(fileName)。
bool DNFLexWrapper::setStringTableFromFile(const char* fileName)
{
    ScriptStringManager* mgr = compiledDnfLex_.getStringManager();
    return mgr->importFromFile(fileName);
}

// SetHandler（ORIG 0x8acec80）：分别转发到 DNFLex::SetHandler 与
// CompiledDNFLex::SetHandler。
void DNFLexWrapper::SetHandler(bool (*isDbcsLeadChar)(char),
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

// 构造（ORIG 0x8aced60）：vptr、lastToken_/lastStringValue_/lexType_ 清零，
// 依次构造 dnfLex_（DNFLex）与 compiledDnfLex_（CompiledDNFLex），最后回调
// 字段 +0x6108/+0x610c 清零（+0x08 lastIntValue_ / +0x0c lastFloatValue_ 不写，
// 与 ORIG 汇编一致）。C++ 成员按声明序自动构造子对象。
DNFLexWrapper::DNFLexWrapper()
    : lastToken_(0),
      lastStringValue_(0),
      lexType_(LEX_TYPE_NORMAL),
      lexerError_(0),
      loadStream_(0)
{
}

// 析构（ORIG 0x8acecf0）：恢复 vptr，析构 compiledDnfLex_ 与 dnfLex_
// （ORIG 对 CompiledDNFLex D1 与 DNFLex D1 按序调用）。C++ 成员按逆声明序
// 自动析构（compiledDnfLex_ 先于 dnfLex_），与 ORIG 顺序一致。
DNFLexWrapper::~DNFLexWrapper()
{
    // 成员析构自动完成（compiledDnfLex_ → dnfLex_）。
}
// ============================================================================
// ScanAll —— 脚本全类型扫描（ORIG T 0x088bdea9 / 0x088bde5a）
// token 码：2=dec、3=hex、4=float、5/6=type、7=string、9=pair(int,str)。
// ============================================================================

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x088bdea9；optimize("O0") 复现 4.1 -O0 帧/槽，
// token 5/6 拆两个独立 else-if（ORIG 两体重复 getType→toTString→assign 序列）⇒ 101=101。
__attribute__((optimize("O0")))
int ScanAll(int* intOut, float* floatOut, std::string* strOut)
{
    int token = g_dnfLex.getToken();
    if (token == 2)
    {
        if (intOut != 0)
            *intOut = g_dnfLex.getDecimal();
    }
    else if (token == 3)
    {
        if (intOut != 0)
            *intOut = g_dnfLex.getHexDecimal();
    }
    else if (token == 4)
    {
        if (floatOut != 0)
            *floatOut = g_dnfLex.getFloat();
    }
    else if (token == 7)
    {
        if (strOut != 0)
            *strOut = toTString(g_dnfLex.getString());
    }
    else if (token == 5)
    {
        if (strOut != 0)
            *strOut = toTString(g_dnfLex.getType());
    }
    else if (token == 6)
    {
        if (strOut != 0)
            *strOut = toTString(g_dnfLex.getType());
    }
    else if (token == 9)
    {
        int pair = g_dnfLex.getPairInt();
        if (g_dnfLex.getToken() == 0xa)
        {
            token = 7;
            if (strOut != 0)
                *strOut = g_scriptStringManager_.findString(
                    pair, g_dnfLex.getPairStr(), 0);
        }
    }
    return token;
}

// [b28d-solo-DNFLexWrapper] AE：ORIG 0x088bde5a；optimize("O0") 复现 4.1 -O0 帧 0x38、
// 双局部槽 -0x10/-0xc 与 (float)v 的 -0x1c 转换临时 ⇒ 25=25 逐条同形。
__attribute__((optimize("O0")))
int ScanAll(float* value, std::string* str)
{
    int v = 0;
    int r = ScanAll(&v, value, str);
    if (r == 2 || r == 3)
    {
        r = 4;
        *value = (float)v;
    }
    return r;
}
