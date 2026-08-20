// ============================================================================
// DNFLexWrapper 五个方法（G4 脚本批次；ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLexWrapper.md）
// ----------------------------------------------------------------------------
// 本 TU 与 GameStubs_remaining.cpp 的四个取词方法互补，不重复实现：
//   GameStubs_remaining.cpp 已提供 getToken 0x8ace7e0 / getDecimal 0x8ace2f0 /
//   getHexDecimal 0x8ace780 / getFloat 0x8ace730（_ZN13DNFLexWrapper8getTokenEv 等）。
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
//   vtable/typeinfo           _ZTV13DNFLexWrapper（ORIG 0x8e2ec28）
//                             _ZTI/_ZTS13DNFLexWrapper（0x8e2ec48/0x8e2ec38）
//
// 字段布局依据 ORIG DNFLexWrapper::DNFLexWrapper（0x8aced60）：
//   +0x00 vptr   +0x04 lastToken_   +0x08 lastIntValue_
//   +0x0c lastFloatValue_           +0x10 lastStringValue_   +0x14 lexType_
//   （lexType_：0=NORMAL 文本流 / 1=COMPILED 编译态，见 getConnector 0x8ace3f0）
//   +0x18 dnfLex_（DNFLex 子对象；FlexLexer.yytext @ +0x1c）
//   +0x40bc compiledDnfLex_（CompiledDNFLex 子对象）
//   +0x6108 LexerError_ / +0x610c LoadStream_，总大小 0x6110。
//
// setStringTableFromBuffer/FromFile 是到 CompiledDNFLex 字符串管理器的纯转发
// （ORIG 尾调用 jmp _ZN19ScriptStringManager16importFrom*）。CompiledDNFLex/
// ScriptStringManager 引擎符号由词法引擎批次提供，本 TU 仅按 ORIG ABI 声明
// 并包装调用，不做固定返回值伪实现。
// ============================================================================

#include <cstdlib>
#include <cstring>
#include <string>

static void* g_dnfLexWrapperInstance = 0;

namespace
{
struct DNFLexStorage
{
    const void* vptr;
    char* yytext_;
    char opaque[0x40a4 - 8];
};
struct CompiledDNFLexStorage { char opaque[0x604c]; };

struct LexWrapperLayout
{
    const void* vptr;
    int lastToken_;
    int lastIntValue_;
    float lastFloatValue_;
    char* lastStringValue_;
    int lexType_;
    DNFLexStorage dnfLex_;
    CompiledDNFLexStorage compiledDnfLex_;
    void (*lexerError_)(const char*);
    int (*loadStream_)(const char*, const char*, void*);
};
typedef LexWrapperLayout LexWrapperState;

enum
{
    LEX_NORMAL   = 0,
    LEX_COMPILED = 1
};

}  // namespace

// getPairStr 的局部静态缓冲区：ORIG 为函数局部静态
// （符号 _ZZN13DNFLexWrapper10getPairStrEvE6buffer，0x9501900，bss 0x100 字节，
// 见报告汇编 `movl $0x9501900`）。用变量 asm 标签对齐 ORIG 符号。
static char g_pairStrBuffer[0x100]
    __asm__("_ZZN13DNFLexWrapper10getPairStrEvE6buffer");

// getPairInt（ORIG 0x8ace630）：NORMAL 对 yytext+1 拷贝 0x100 字节并保证终止后
// strtol base 10（跳过成对 token 前导字符，如 "=123"）；COMPILED 返回
// lastIntValue_；其它 lexType 返回 0。
extern "C" char* sub_lex_getPairStr(void* self)
{
    LexWrapperState* w = reinterpret_cast<LexWrapperState*>(self);
    if (w->lexType_ == LEX_NORMAL)
    {
        const char* pc = w->dnfLex_.yytext_;
        if (!pc)
            return g_pairStrBuffer;  // 护栏：返回空缓冲区
        strncpy(g_pairStrBuffer, pc + 1, 0x100);
        g_pairStrBuffer[0xff] = '\0';
        return g_pairStrBuffer;
    }
    if (w->lexType_ == LEX_COMPILED)
        return w->lastStringValue_;
    return 0;
}

// getLastTokenDataAsString（ORIG 0x8ace4d0）：返回 tstring（std::string）。
// NORMAL 取 yytext；COMPILED 取 lastStringValue_；其它 lexType 或指针为空
// 时返回空串（ORIG 经 toTString 后构造 std::string）。
extern "C" std::string sub_lex_getLastTokenDataAsString(void* self)
    asm("_ZN13DNFLexWrapper24getLastTokenDataAsStringEv");
extern "C" std::string sub_lex_getLastTokenDataAsString(void* self)
{
    LexWrapperState* w = reinterpret_cast<LexWrapperState*>(self);
    const char* pc = 0;
    if (w->lexType_ == LEX_NORMAL)
        pc = w->dnfLex_.yytext_;
    else if (w->lexType_ == LEX_COMPILED)
        pc = w->lastStringValue_;
    return std::string(pc ? pc : "");
}

// ---- 底层词法引擎符号（由引擎批次提供；本 TU 仅按 ORIG ABI 声明并转发）----
// CompiledDNFLex::getStringManager()           _ZN14CompiledDNFLex16getStringManagerEv
// ScriptStringManager::importFromBuffer()      _ZN19ScriptStringManager16importFromBufferEPKhi
// ScriptStringManager::importFromFile()        _ZN19ScriptStringManager14importFromFileEPKc
// DNFLex::SetHandler(...)                      _ZN6DNFLex10SetHandlerEPFbcEPFvPKciEPFvS3_EPFbS3_S3_PNS_13stream_data_tEE
//                                              （DNFLexCore.cpp，ORIG 0x8acd160）
// CompiledDNFLex::SetHandler(...)              _ZN14CompiledDNFLex10SetHandlerEPFvPKcEPFbS1_S1_PN6DNFLex13stream_data_tEE
//                                              （CompiledDNFLexCore.cpp，ORIG 0x8acfdf0）
extern "C" void* sub_CompiledDNFLex_getStringManager(void* self)
    asm("_ZN14CompiledDNFLex16getStringManagerEv");
extern "C" bool sub_ScriptStringManager_importFromBuffer(
    void* mgr, const unsigned char* buffer, int size)
    asm("_ZN19ScriptStringManager16importFromBufferEPKhi");
extern "C" bool sub_ScriptStringManager_importFromFile(void* mgr,
                                                       const char* fileName)
    asm("_ZN19ScriptStringManager14importFromFileEPKc");

// setStringTableFromBuffer（ORIG 0x8ace560）：转发到 compiledDnfLex_（+0x40bc）
// 的 ScriptStringManager::importFromBuffer(buffer, size)。
extern "C" bool sub_lex_setStringTableFromBuffer(void* self,
                                                 const unsigned char* buffer,
                                                 int size)
    asm("_ZN13DNFLexWrapper24setStringTableFromBufferEPKhi");
extern "C" bool sub_lex_setStringTableFromBuffer(void* self,
                                                 const unsigned char* buffer,
                                                 int size)
{
    LexWrapperLayout* w = reinterpret_cast<LexWrapperLayout*>(self);
    void* mgr = sub_CompiledDNFLex_getStringManager(&w->compiledDnfLex_);
    return sub_ScriptStringManager_importFromBuffer(mgr, buffer, size);
}

extern "C" bool sub_lex_setStringTableFromFile(void* self,
                                               const char* fileName)
    asm("_ZN13DNFLexWrapper22setStringTableFromFileEPKc");
extern "C" bool sub_lex_setStringTableFromFile(void* self,
                                               const char* fileName)
{
    LexWrapperLayout* w = reinterpret_cast<LexWrapperLayout*>(self);
    void* mgr = sub_CompiledDNFLex_getStringManager(&w->compiledDnfLex_);
    return sub_ScriptStringManager_importFromFile(mgr, fileName);
}



// ---- typeinfo name（ORIG 0x8e2ec38）----
extern "C" const char g_DNFLexWrapper_typeinfoName[]
    __asm__("_ZTS13DNFLexWrapper") = "13DNFLexWrapper";

// libstdc++ __class_type_info vtable（弱符号，链接期解析；ORIG 值为
// 0x0936b4e0，DNFLexWrapper typeinfo 的 vptr 字段指向其 +8 处）。
extern "C" const void* g_cxxabi_classTypeInfo_vtable
    asm("_ZTVN10__cxxabiv117__class_type_infoE");

// ---- typeinfo（ORIG 0x8e2ec48；Itanium ABI：{ vptr, type_name }）----
struct DNFLexWrapperTypeInfo
{
    const void* vptr;
    const char* typeName;
};
extern "C" const struct DNFLexWrapperTypeInfo g_DNFLexWrapper_typeinfo
    __asm__("_ZTI13DNFLexWrapper");
extern "C" const struct DNFLexWrapperTypeInfo g_DNFLexWrapper_typeinfo = {
    reinterpret_cast<const char*>(&g_cxxabi_classTypeInfo_vtable) + 8,
    g_DNFLexWrapper_typeinfoName,
};

// ---- vtable（ORIG 0x8e2ec28）：[-2] offset-to-top=0、[-1] typeinfo、
//      [0]=D1、[1]=D0；vptr = &vtable[2]（对应 ORIG vptr 0x8e2ec30）----
extern "C" const void* g_DNFLexWrapper_vtable[]
    __asm__("_ZTV13DNFLexWrapper");
extern "C" const void* g_DNFLexWrapper_vtable[] = {
    0,
    &g_DNFLexWrapper_typeinfo,
    reinterpret_cast<const void*>(&sub_lex_D1),
    reinterpret_cast<const void*>(&sub_lex_D0),
};

// 构造体（ORIG 0x8aced60）：vptr、lastToken_/lastStringValue_/lexType_ 清零，
// 依次构造 dnfLex_(+0x18) 与 compiledDnfLex_(+0x40bc)，最后回调字段
// +0x6108/+0x610c 清零（+0x08 lastIntValue_ / +0x0c lastFloatValue_ 不写，
// 与 ORIG 汇编一致）。
static void lexWrapperCtorBody(void* self)
{
    LexWrapperLayout* w = reinterpret_cast<LexWrapperLayout*>(self);
    w->vptr = reinterpret_cast<const void*>(&g_DNFLexWrapper_vtable[2]);
    w->lastToken_ = 0;
    w->lastStringValue_ = 0;
    w->lexType_ = LEX_NORMAL;
    sub_DNFLex_C1(&w->dnfLex_);
    try
    {
        sub_CompiledDNFLex_C1(&w->compiledDnfLex_);
    }
    catch (...)
    {
        sub_DNFLex_D1(&w->dnfLex_);
        throw;
    }
    w->lexerError_ = 0;
    w->loadStream_ = 0;
}

extern "C" void sub_lex_C1(void* self)
{
    lexWrapperCtorBody(self);
}
extern "C" void sub_lex_C2(void* self)
{
    lexWrapperCtorBody(self);
}

// 析构体（ORIG 0x8acecf0）：恢复 vptr，析构 compiledDnfLex_(+0x40bc) 与
// dnfLex_(+0x18)（ORIG 对 DNFLex D1 为尾调用 jmp 0x8acd5f0）。
static void lexWrapperDtorBody(void* self)
{
    LexWrapperLayout* w = reinterpret_cast<LexWrapperLayout*>(self);
    w->vptr = reinterpret_cast<const void*>(&g_DNFLexWrapper_vtable[2]);
    try
    {
        sub_CompiledDNFLex_D1(&w->compiledDnfLex_);
    }
    catch (...)
    {
        sub_DNFLex_D1(&w->dnfLex_);
        throw;
    }
    sub_DNFLex_D1(&w->dnfLex_);
}

extern "C" void sub_lex_D1(void* self)
{
    lexWrapperDtorBody(self);
}
extern "C" void sub_lex_D2(void* self)
{
    lexWrapperDtorBody(self);   // ORIG D2 与 D1 同址 0x8acecf0
}

// 删除析构（ORIG 0x8aced40）：D1 后 operator delete（ORIG 尾调用 jmp _ZdlPv）。
extern "C" void sub_lex_D0(void* self)
{
    sub_lex_D1(self);
    ::operator delete(self);
}
