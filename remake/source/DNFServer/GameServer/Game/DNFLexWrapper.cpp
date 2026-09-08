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

char* DNFLexWrapper::getLastTokenData()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return const_cast<char*>(dnfLex_.YYText());
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

int DNFLexWrapper::getLastLineNo()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.lineno();
    int r = -1;
    if (lexType_ == LEX_TYPE_COMPILED)
        return compiledDnfLex_.GetOffset();
    return r;
}

char* DNFLexWrapper::getType()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return const_cast<char*>(dnfLex_.YYText());
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

char* DNFLexWrapper::getConnector()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return const_cast<char*>(dnfLex_.YYText());
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}

char* DNFLexWrapper::getCustomData()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return const_cast<char*>(dnfLex_.YYText());
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

__attribute__((optimize("O2,no-if-conversion,no-omit-frame-pointer")))
// [b28d-L1] N 封顶（-O2 路由）：ORIG 0x08ace2f0；4.1 内联 strlen/字符分类 vs 4.4-O2 call 形态差（N98）。P167 CLEAR（0/0）语义等价。
int DNFLexWrapper::getDecimal()
{
    char* p;
    char c;
    bool neg;
    int v;
    int d;

    if (lexType_ == LEX_TYPE_NORMAL)
    {
        p = const_cast<char*>(dnfLex_.YYText());
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
    if ((unsigned)d > 9)
        goto maybe_neg;
digit_loop:
    p = p + 1;
    v = v * 10 + d;
    d = (int)*p - 0x30;
    if ((unsigned)d <= 9)
        goto digit_loop;
maybe_neg:
    if (neg)
        return -v;
    return v;
}

int DNFLexWrapper::getHexDecimal()
{
    if (lexType_ == LEX_TYPE_NORMAL)
    {
        int v = 0;
        std::sscanf(dnfLex_.YYText() + 2, "%x", &v);
        return v;
    }
    int r = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastIntValue_;
    return r;
}

float DNFLexWrapper::getFloat()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return (float)__strtod_internal(dnfLex_.YYText(), 0, 0);
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastFloatValue_;
    return 0.0f;
}

// getPairInt（ORIG 0x8ace630）：NORMAL 对 yytext+1 拷贝 0x100 字节并保证终止后
// strtol base 10（跳过成对 token 前导字符，如 "=123"）；COMPILED 返回
// lastIntValue_；其它 lexType 返回 0。
// [b28d-L1] N 封顶（-O2 路由）：ORIG 0x08ace630；4.1 内联 strlen 为 word-at-a-time（N98）而 4.4-O2 恒发 call，另含 callee-saved 装载序差。P167 CLEAR（2/2）语义等价。
int DNFLexWrapper::getPairInt()
{
    // [b28c-11] N 封顶（54/56）：残余 = ORIG 4.1 把 `strlen(buffer)` 内联为
    // word-at-a-time 序列（0x8ace688 起），4.4 -O0 恒发 call strlen（探针
    // /tmp/l11p/sl.cpp：strlen 与 __builtin_strlen 在 4.4 -O0 均为 call）⇒
    // 帧 0x114/0x128 与 2 条 call 不可复现。语义等价（已改 dnfLex_.yytext 成员直读
    // 消 YYText() call，见 FlexLexer.h yytext public 化）。
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

// [b28d-L1] N 封顶（-O2 路由）：ORIG 0x08ace5a0；同 getPairInt 的 4.1 内联 strlen vs 4.4 call（N98）+ 装载序差。P167 CLEAR（1/1）语义等价。
char* DNFLexWrapper::getPairStr()
{
    // [b28c-11] N 封顶（41/41）：同 getPairInt 的 4.1 内联 strlen vs 4.4 call
    // （0x8ace5d3 起 word-at-a-time 展开）；帧 0x18/0x28 差同源。
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

std::string DNFLexWrapper::getLastTokenDataAsString()
{
    const char* pc;
    if (lexType_ == LEX_TYPE_NORMAL)
        pc = dnfLex_.YYText();
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

// [b28d-L1] N89 封顶（-O2 路由）：ORIG 0x088bdea9；P167 GAP 全为 -O2 内联/优化决策差（ORIG 发 toTString/getType/string::operator= call，4.4 内联 toTString 并选 string::assign）。ORIG 每次重取 g_dnfLex 地址 vs 4.4 callee-saved 缓存同属 -O2 分配差。语义等价。
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
    else if (token == 5 || token == 6)
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

// [b28d-L1] N82 封顶（-O2 路由）：ORIG 0x088bde5a；P167 CLEAR（1/1）⇒ 语义完成；残余为寄存器/装载形态差。
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
