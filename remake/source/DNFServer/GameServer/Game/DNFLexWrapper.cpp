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

#include <cstdlib>
#include <cstring>
#include <string>

#include "DNFLexWrapper.h"
#include "ScriptStringManager.h"

// getPairInt（ORIG 0x8ace630）：NORMAL 对 yytext+1 拷贝 0x100 字节并保证终止后
// strtol base 10（跳过成对 token 前导字符，如 "=123"）；COMPILED 返回
// lastIntValue_；其它 lexType 返回 0。
int DNFLexWrapper::getPairInt()
{
    if (lexType_ == LEX_TYPE_NORMAL)
    {
        char buffer[0x100];
        const char* pc = dnfLex_.YYText();
        if (!pc)
            return 0;
        std::strncpy(buffer, pc + 1, 0x100);
        buffer[0xff] = '\0';
        return static_cast<int>(std::strtol(buffer, 0, 10));
    }
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastIntValue_;
    return 0;
}

// getPairStr（ORIG 0x8ace5a0）：NORMAL 对 yytext+1 拷贝 0x100 字节并保证终止后
// 返回局部静态缓冲（ORIG 符号 _ZZN13DNFLexWrapper10getPairStrEvE6buffer，
// bss 0x100 字节，见报告汇编 `movl $0x9501900`；函数局部静态由编译器生成
// 同名符号）；COMPILED 返回 lastStringValue_；其它 lexType 返回 0。
char* DNFLexWrapper::getPairStr()
{
    static char buffer[0x100];
    if (lexType_ == LEX_TYPE_NORMAL)
    {
        const char* pc = dnfLex_.YYText();
        if (!pc)
            return buffer;  // 护栏：返回空缓冲区
        std::strncpy(buffer, pc + 1, 0x100);
        buffer[0xff] = '\0';
        return buffer;
    }
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return 0;
}

// getLastTokenDataAsString（ORIG 0x8ace4d0）：返回 tstring（std::string）。
// NORMAL 取 yytext；COMPILED 取 lastStringValue_；其它 lexType 或指针为空
// 时返回空串（ORIG 经 toTString 后构造 std::string）。
std::string DNFLexWrapper::getLastTokenDataAsString()
{
    const char* pc = 0;
    if (lexType_ == LEX_TYPE_NORMAL)
        pc = dnfLex_.YYText();
    else if (lexType_ == LEX_TYPE_COMPILED)
        pc = lastStringValue_;
    return std::string(pc ? pc : "");
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
}

// 构造（ORIG 0x8aced60）：vptr、lastToken_/lastStringValue_/lexType_ 清零，
// 依次构造 dnfLex_（DNFLex）与 compiledDnfLex_（CompiledDNFLex），最后回调
// 字段 +0x6108/+0x610c 清零（+0x08 lastIntValue_ / +0x0c lastFloatValue_ 不写，
// 与 ORIG 汇编一致）。C++ 成员按声明序自动构造子对象。
DNFLexWrapper::DNFLexWrapper()
    : lastToken_(0),
      lastIntValue_(0),
      lastFloatValue_(0.0f),
      lastStringValue_(0),
      lexType_(LEX_TYPE_NORMAL),
      lexerError_(0),
      loadStream_(0)
{
    // dnfLex_ / compiledDnfLex_ 由成员默认构造（DNFLex() / CompiledDNFLex()）。
    // ORIG 不写 lastIntValue_/lastFloatValue_；此处保持默认初值。
}

// 析构（ORIG 0x8acecf0）：恢复 vptr，析构 compiledDnfLex_ 与 dnfLex_
// （ORIG 对 CompiledDNFLex D1 与 DNFLex D1 按序调用）。C++ 成员按逆声明序
// 自动析构（compiledDnfLex_ 先于 dnfLex_），与 ORIG 顺序一致。
DNFLexWrapper::~DNFLexWrapper()
{
    // 成员析构自动完成（compiledDnfLex_ → dnfLex_）。
}
