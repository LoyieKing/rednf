// ============================================================================
// DNFLex（_ZTV6DNFLex：12 函数槽，覆写 D1/D0、yylex、LexerOutput、LexerError；
// 其余继承自 __dnf_script__FlexLexer）。
// 基类 FlexLexer / __dnf_script__FlexLexer 由 vendored flex 2.5.35 的
// FlexLexer.h 提供。
// 数据（+0x80 起；FlexLexer 基类占 +0x00..+0x7f）：
//   +0x80 stream_stack 三指针 +0x8c ERR_BUF[0x2000] +0x208c STR_TOK_BUF[0x2000]
//   +0x408c m_str_tok_pos +0x4090 is_dbcs_ +0x4091 allow_inclusion_
//   +0x4094 isDbcsLeadChar +0x4098 lexerOutput +0x409c lexerError
//   +0x40a0 loadStream，总大小 0x40a4。
// ============================================================================
#ifndef DNFLEX_H
#define DNFLEX_H

#include <string>
#include <iostream>

// ---- FlexLexer / __dnf_script__FlexLexer 基类（flex 2.5.35 权威头）----
#ifndef yyFlexLexer
#define yyFlexLexer __dnf_script__FlexLexer
#endif
#include "FlexLexer.h"

// ---------------------------------------------------------------------------
// DNFLEX_TOKEN（DNF 脚本词法记号，共享枚举）。
// 数值证据：DNFLex::yylex 规则返回（0x8ad2860）、CompiledDNFLex::getInt
// （cmp $0x9）、getFloat（cmp $0x4）、getToken INCLUDE（cmp $0xb）。
// ---------------------------------------------------------------------------
enum DNFLEX_TOKEN
{
    DNF_TOK_EOF         = 0,
    DNF_TOK_ERROR       = 1,
    DNF_TOK_DECIMAL     = 2,
    DNF_TOK_HEXADECIMAL = 3,
    DNF_TOK_FLOAT       = 4,
    DNF_TOK_TYPE        = 5,
    DNF_TOK_CONNECTOR   = 6,
    DNF_TOK_STRING      = 7,
    DNF_TOK_CUSTOM_DATA = 8,
    DNF_TOK_PAIR_INT    = 9,
    DNF_TOK_PAIR_STR    = 10,
    DNF_TOK_INCLUDE     = 11
};

// ---------------------------------------------------------------------------
// DNFLex（DNFLexCore.cpp 提供方法实现；_ZTV6DNFLex 由本 TU 生成）。
// ---------------------------------------------------------------------------
class DNFLex : public __dnf_script__FlexLexer
{
public:
    // ---- 嵌套类型 ----
    struct stream_data_t
    {
        void* stream;   // +0x00 istream*
        int   size;     // +0x04
        char* buffer;   // +0x08
        void clear();
        void destroy();
    };
    struct stream_t
    {
        const char* dir;        // +0x00
        const char* filename;   // +0x04
        stream_data_t s;        // +0x08
        int line_number;        // +0x14
    };

    // ---- 回调类型 ----
    typedef bool (*TIsDbcsLeadChar)(char);
    typedef void (*TLexerOutput)(const char*, int);
    typedef void (*TLexerError)(const char*);
    typedef bool (*TLoadStream)(const char*, const char*, stream_data_t*);

    DNFLex();                        // 0x8acd3a0
    virtual ~DNFLex();               // 0x8acd5f0 D1 / 0x8acd7b0 D0

    // 覆写 vtable 槽 8 / 12 / 13
    virtual int yylex();                              // 0x08ad2860
    virtual void LexerOutput(const char* buf, int size);  // 0x08acd190
    virtual void LexerError(const char* msg);         // 0x08acd1c0

    // ---- 流方法 ----
    stream_t* GetInputStream();       // 0x8acd280
    std::string getStreamName();      // 0x8acd310（tstring RVO）
    void CloseInputStream();          // 0x8acd5b0
    bool PopInputStream(bool pop_all);// 0x8acd490
    bool LoadStream(const char* dir, const char* filename, stream_data_t* s);  // 0x8acd220
    bool PushInputStream(const char* dir, const char* filename, stream_data_t* s); // 0x8acd7d0
    bool SwitchInputStream(const char* dir, const char* filename, stream_data_t* s); // 0x8acda50
    void SetHandler(TIsDbcsLeadChar isDbcs, TLexerOutput output,
                    TLexerError error, TLoadStream load);   // 0x8acd160

    // ---- 词法状态 ----
    void EnableInclusion();           // 0x8acd2d0
    void DisableInclusion();          // 0x8acd2c0
    bool IsStream();                  // 0x8acd260
    char* tok_str();                  // 0x8acd1f0
    int   tok_str_len();              // 0x8acd200
    int   getLineNumber();            // 0x8acd2b0
    int   get_next_token();           // 0x8acdaa0（DNFLEX_TOKEN）

    // ---- 数据（+0x80 起；FlexLexer 基类占 +0x00..+0x7f）----
    char*  stackStart;    // +0x80 stream_stack_._M_start
    char*  stackFinish;   // +0x84 stream_stack_._M_finish
    char*  stackEnd;      // +0x88 stream_stack_._M_end_of_storage
    char   errBuf[0x2000];   // +0x8c ERR_BUF（构造 rep stos 0x800 双字）
    char   strTokBuf[0x2000]; // +0x208c STR_TOK_BUF（构造 rep stos 0x800 双字）
    char*  m_str_tok_pos;   // +0x408c STR_TOK_BUF 写入游标
    char   is_dbcs_;        // +0x4090
    char   allow_inclusion_; // +0x4091
    char   pad4092[2];      // +0x4092 .. +0x4093
    TIsDbcsLeadChar isDbcsLeadChar;  // +0x4094
    TLexerOutput    lexerOutput;     // +0x4098
    TLexerError     lexerError;      // +0x409c
    TLoadStream     loadStream;      // +0x40a0
    // 总大小 0x40a4
};

// ---------------------------------------------------------------------------
// Flex DFA 表（ORIG .rodata：yy_accept@0x8e2f180、yy_def@0x8e2f200、
// yy_base@0x8e2f280、yy_chk@0x8e2f300、yy_nxt@0x8e30400、
// yy_NUL_trans@0x8e2f0a0、yy_rule_can_match_eol@0x8e31500）。
// 定义于 DNFLexCore.cpp；FlexLexerEngine.cpp（yy_get_previous_state 等）引用。
// ---------------------------------------------------------------------------
extern "C"
{
extern const short          g_yy_accept[];
extern const short          g_yy_def[];
extern const short          g_yy_base[];
extern const short          g_yy_chk[];
extern const short          g_yy_nxt[];
extern const int            g_yy_NUL_trans[];
extern const unsigned char  g_yy_rule_can_match_eol[];
}

#endif  // DNFLEX_H
