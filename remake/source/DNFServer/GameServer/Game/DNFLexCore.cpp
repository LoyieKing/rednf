// ============================================================================
// DNFLex 基础方法（词法引擎批次；ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLex.md 与 DNFLex__stream_data_t.md）
// ----------------------------------------------------------------------------
// 本 TU 必须以 gnu++98 -O2 编译：ORIG 的 vector::push_back 是 C++98
// finish!=end 路径；gnu++0x 会走 rvalue emplace_back，指令对不上。
//
// DNFLex 本类收口（2026-08-23）：yylex / get_next_token / getStreamName 在同一
// 4.4.6-3 -O2 下为寄存器分配 / NRVO 帧 DIFF，禁止 __asm__ 后无法再压到逐指令
// IDENTICAL；语义已对齐 ORIG。用户视为 identical（checker IDENTICAL_AE）。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>

#include "DNFLex.h"
#include "SyncScriptDeps.h"

static int dnf_yylex(DNFLex* self)
{
    void** vptr = *reinterpret_cast<void***>(self);
    typedef int (*Fn)(DNFLex*);
    return ((Fn)vptr[0x18 / sizeof(void*)])(self);
}

// ---- DNFLex::GetInputStream()（ORIG 0x8acd280）----
DNFLex::stream_t* DNFLex::GetInputStream()
{
    stream_t* finish = stream_stack.end().base();
    stream_t* start = stream_stack.begin().base();
    unsigned empty = (unsigned)(finish == start);
    unsigned mask = empty - 1u;
    return reinterpret_cast<stream_t*>((unsigned)(finish - 1) & mask);
}

// ---- DNFLex::getStreamName()（ORIG 0x8acd310，tstring RVO）----
std::string DNFLex::getStreamName()
{
    stream_t* finish = stream_stack.end().base();
    if (finish == stream_stack.begin().base())
        return std::string("NONE");
    std::string result((finish - 1)->dir);
    result.append((finish - 1)->filename);
    return result;
}

// ---- DNFLex::CloseInputStream()（ORIG 0x8acd5b0）----
void DNFLex::CloseInputStream()
{
    if (stream_stack.end().base() != stream_stack.begin().base())
    {
        do
        {
            PopInputStream(true);
        } while (stream_stack.end().base() != stream_stack.begin().base());
    }
}

// ---- DNFLex::stream_data_t::clear()（ORIG 0x8acd140）----
void DNFLex::stream_data_t::clear()
{
    stream = 0;
    size = 0;
    buffer = 0;
}

// ---- DNFLex::stream_data_t::destroy()（ORIG 0x8acd450）----
void DNFLex::stream_data_t::destroy()
{
    if (stream != 0)
    {
        void (**vtable)(void*) = *reinterpret_cast<void (***)(void*)>(stream);
        vtable[1](stream);
        stream = 0;
    }
    if (buffer != 0)
    {
        ::operator delete(buffer);
        buffer = 0;
    }
}

// ---- DNFLex::PopInputStream(bool)（ORIG 0x8acd490）----
bool DNFLex::PopInputStream(bool pop_all)
{
    stream_t* finish = stream_stack.end().base();
    stream_t* start = stream_stack.begin().base();
    if (finish == start)
        return false;
    if (!pop_all && stream_stack.size() <= 1)
        return false;

    this->yypop_buffer_state();
    stream_stack.end().base()[-1].s.destroy();
    stream_stack.pop_back();
    this->yy_start = 1;
    unsigned lineno = 1;
    if (stream_stack.end().base() != stream_stack.begin().base())
        lineno = stream_stack.end().base()[-1].line_number;
    this->yylineno = (int)lineno;
    return true;
}

// ---- DNFLex::LoadStream(...)（ORIG 0x8acd220）----
bool DNFLex::LoadStream(const char* dir, const char* filename, stream_data_t* s)
{
    if (loadStream == 0)
        return false;
    return loadStream(dir, filename, s);
}

// ---- DNFLex::PushInputStream(...)（ORIG 0x8acd7d0）----
bool DNFLex::PushInputStream(const char* dir, const char* filename, stream_data_t* s)
{
    stream_t stream;
    const char* d = "";
    std::size_t dlen = 0;
    if (dir != 0)
    {
        dlen = std::strlen(dir);
        d = dir;
    }
    stream.dir.assign(d, dlen);

    const char* f = "";
    std::size_t flen = 0;
    if (filename != 0)
    {
        flen = std::strlen(filename);
        f = filename;
    }
    stream.filename.assign(f, flen);

    if (s == 0)
    {
        if (!LoadStream(dir, filename, &stream.s))
        {
            std::sprintf(errBuf, "%s - DNFLex::ReadStream failed.", __FUNCTION__);
            LexerError(errBuf);
            return false;
        }
    }
    else
    {
        stream.s.stream = s->stream;
        stream.s.size = s->size;
        stream.s.buffer = s->buffer;
    }

    if (stream_stack.end().base() != stream_stack.begin().base())
        stream_stack.end().base()[-1].line_number = this->yylineno;

    stream_stack.push_back(stream);

    yy_buffer_state* new_buffer = this->yy_create_buffer(
        reinterpret_cast<std::istream*>(stream.s.stream), stream.s.size);
    this->yypush_buffer_state(new_buffer);
    this->yy_start = 1;
    this->yylineno = 1;
    return true;
}

// ---- DNFLex::SwitchInputStream(...)（ORIG 0x8acda50）----
bool DNFLex::SwitchInputStream(const char* dir, const char* filename, stream_data_t* s)
{
    CloseInputStream();
    return PushInputStream(dir, filename, s);
}

// ---- DNFLex::SetHandler(...)（ORIG 0x8acd160）----
void DNFLex::SetHandler(TIsDbcsLeadChar isDbcs, TLexerOutput output,
                        TLexerError error, TLoadStream load)
{
    isDbcsLeadChar = isDbcs;
    lexerOutput = output;
    lexerError = error;
    loadStream = load;
}

void DNFLex::LexerOutput(const char* buf, int size)
{
    if (lexerOutput != 0)
        lexerOutput(buf, size);
}

void DNFLex::LexerError(const char* msg)
{
    if (lexerError != 0)
        lexerError(msg);
}

void DNFLex::EnableInclusion()
{
    allow_inclusion_ = true;
}

void DNFLex::DisableInclusion()
{
    allow_inclusion_ = false;
}

bool DNFLex::IsStream()
{
    return stream_stack.end().base() != stream_stack.begin().base();
}

char* DNFLex::tok_str()
{
    return strTokBuf;
}

int DNFLex::tok_str_len()
{
    return static_cast<int>(m_str_tok_pos - strTokBuf);
}

int DNFLex::getLineNumber()
{
    return yylineno;
}

// ---- DNFLex::get_next_token（ORIG 0x8acdaa0）----
int DNFLex::get_next_token()
{
    int tok = 0;
    if (stream_stack.end().base() != stream_stack.begin().base())
    {
        tok = dnf_yylex(this);
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

DNFLex::DNFLex()
    : __dnf_script__FlexLexer(0, 0),
      m_str_tok_pos(0), is_dbcs_(0), allow_inclusion_(1),
      isDbcsLeadChar(0), lexerOutput(0), lexerError(0), loadStream(0)
{
    std::memset(errBuf, 0, sizeof(errBuf));
    std::memset(strTokBuf, 0, sizeof(strTokBuf));
}

DNFLex::~DNFLex()
{
    CloseInputStream();
}
