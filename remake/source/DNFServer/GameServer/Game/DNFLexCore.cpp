// ============================================================================
// DNFLex 基础方法（词法引擎批次；ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLex.md 与 DNFLex__stream_data_t.md）
// ----------------------------------------------------------------------------
// 本 TU 承载 DNFLex（_ZN6DNFLex*，即 DNFLexWrapper.dnfLex_ 子对象，wrapper+0x18）
// 的基础流方法，与 DNFLexWrapper.cpp / DNFLexWrapperStream.cpp 以及
// CompiledLexCore 的 CompiledDNFLexCore.cpp（_ZN14CompiledDNFLex*）互补。
//
// 本 TU 实现（ORIG 地址 / 汇编证据）：
//   GetInputStream()      _ZN6DNFLex14GetInputStreamEv  ORIG 0x8acd280
//   getStreamName()       _ZN6DNFLex13getStreamNameEv   ORIG 0x8acd310
//   CloseInputStream()    _ZN6DNFLex16CloseInputStreamEv ORIG 0x8acd5b0
//   SetHandler(...)       _ZN6DNFLex10SetHandlerEPFbcEPFvPKciEPFvS3_EPFbS3_S3_PNS_13stream_data_tEE
//                                                     ORIG 0x8acd160
//   PopInputStream(bool)  _ZN6DNFLex14PopInputStreamEb  ORIG 0x8acd490
//   LoadStream(...)       _ZN6DNFLex10LoadStreamEPKcS1_PNS_13stream_data_tE
//                                                     ORIG 0x8acd220
//   PushInputStream(...)  _ZN6DNFLex15PushInputStreamEPKcS1_PNS_13stream_data_tE
//                                                     ORIG 0x8acd7d0
//   SwitchInputStream(...) _ZN6DNFLex17SwitchInputStreamEPKcS1_PNS_13stream_data_tE
//                                                     ORIG 0x8acda50
//   stream_data_t::clear()  _ZN6DNFLex13stream_data_t5clearEv  ORIG 0x8acd140
//   stream_data_t::destroy() _ZN6DNFLex13stream_data_t7destroyEv
//                                                     ORIG 0x8acd450
//   DNFLex() / ~DNFLex()   _ZN6DNFLexC1Ev / D1Ev / D0Ev
//                          ORIG 0x8acd3a0 / 0x8acd5f0 / 0x8acd7b0
//   yylex()               _ZN6DNFLex5yylexEv          ORIG 0x8ad2860
//   LexerOutput/LexerError/EnableInclusion/DisableInclusion/IsStream/
//   tok_str/tok_str_len/getLineNumber/get_next_token
//   vtable/typeinfo       _ZTV6DNFLex / _ZTI6DNFLex / _ZTS6DNFLex
//
// 对象布局依据 ORIG DNFLex::DNFLex（0x8acd3a0）与 DNFLex::PushInputStream
// （0x8acd7d0），见 DNFLex.h。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <new>
#include <string>
#include <iostream>

#include "DNFLex.h"

// 顶层元素 = &((stream_t*)stackFinish)[-1] 位于 stackFinish - 0x18。
static DNFLex::stream_t* topStream(DNFLex* s)
{
    return reinterpret_cast<DNFLex::stream_t*>(s->stackFinish) - 1;
}

// 判断流栈非空。
static bool hasStream(DNFLex* s)
{
    return s->stackFinish != s->stackStart;
}

// ---- DNFLex::GetInputStream()（ORIG 0x8acd280）----
// 栈空返回 0；否则返回栈顶 stream_t*（finish-1）。
DNFLex::stream_t* DNFLex::GetInputStream()
{
    if (!hasStream(this))
        return 0;
    return topStream(this);
}

// ---- DNFLex::getStreamName()（ORIG 0x8acd310，tstring RVO 返回）----
// 返回顶层流的 dir+filename 拼接；栈空返回 "NONE"。
std::string DNFLex::getStreamName()
{
    if (hasStream(this))
    {
        stream_t* t = topStream(this);
        std::string result(t->dir ? t->dir : "");
        if (t->filename)
            result.append(t->filename);
        return result;
    }
    return std::string("NONE");
}

// ---- DNFLex::CloseInputStream()（ORIG 0x8acd5b0）----
// 反复弹出流栈至空（ORIG 为 do-while 调用 PopInputStream(true)）。
void DNFLex::CloseInputStream()
{
    while (hasStream(this))
        PopInputStream(true);
}

// ---- DNFLex::stream_data_t::clear()（ORIG 0x8acd140）----
void DNFLex::stream_data_t::clear()
{
    stream = 0;
    size = 0;
    buffer = 0;
}

// ---- DNFLex::stream_data_t::destroy()（ORIG 0x8acd450）----
// 关闭 istream（经其基类 vtable 槽 +4，即 istream 析构），释放 buffer。
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
// 弹出栈顶流：yypop_buffer_state → stream_data_t::destroy → finish-1 →
// （重建以 const char* 建模 dir/filename，无 COW 释放）→ yy_start=1、
// yylineno = 新栈顶 line_number（栈空 1）。
// 条件：栈非空 && (pop_all || 元素数 > 1)。
bool DNFLex::PopInputStream(bool pop_all)
{
    if (!hasStream(this))
        return false;
    ptrdiff_t count = (reinterpret_cast<char*>(topStream(this) + 1) - stackStart) /
                      static_cast<ptrdiff_t>(sizeof(stream_t));
    if (!pop_all && count <= 1)
        return false;

    this->yypop_buffer_state();
    stream_t* top = topStream(this);
    top->s.destroy();
    stackFinish = reinterpret_cast<char*>(top);
    this->yy_start = 1;
    this->yylineno = hasStream(this) ? topStream(this)->line_number : 1;
    return true;
}

// ---- DNFLex::LoadStream(...)（ORIG 0x8acd220）----
// 若 SetHandler 注册了 LoadStream 回调（+0x40a0 非空），以 (dir,filename,s)
// 尾调用之并返回其 bool；否则返回 false。
bool DNFLex::LoadStream(const char* dir, const char* filename, stream_data_t* s)
{
    if (loadStream == 0)
        return false;
    return loadStream(dir, filename, s);
}

// ---- DNFLex::PushInputStream(...)（ORIG 0x8acd7d0）----
// dir/filename 空指针按空串；s==0 时先 LoadStream 到局部（失败 → 写
// ERR_BUF 并 LexerError，返回 false），否则拷贝 s 三字段；把旧栈顶
// line_number 保存为 yylineno；vector push 新 stream_t；yy_create_buffer
// + yypush_buffer_state；yy_start=1、yylineno=1。返回 true。
bool DNFLex::PushInputStream(const char* dir, const char* filename, stream_data_t* s)
{
    stream_data_t local;
    if (s == 0)
    {
        if (!LoadStream(dir, filename, &local))
        {
            std::snprintf(errBuf, sizeof(errBuf),
                          "%s - DNFLex::ReadStream failed.", "PushInputStream");
            if (lexerError != 0)
                lexerError(errBuf);
            return false;
        }
    }
    else
    {
        local = *s;
    }

    if (hasStream(this))
        topStream(this)->line_number = this->yylineno;

    // vector push_back（重建：3 指针管理，扩容翻倍 + 拷贝构造）
    std::size_t count = (stackFinish - stackStart) / sizeof(stream_t);
    std::size_t cap = stackEnd ? (stackEnd - stackStart) / sizeof(stream_t) : 0;
    if (count == cap)
    {
        std::size_t newCap = cap ? cap * 2 : 4;
        stream_t* nb = static_cast<stream_t*>(::operator new(newCap * sizeof(stream_t)));
        for (std::size_t i = 0; i < count; ++i)
        {
            stream_t* dst = nb + i;
            const stream_t* src = reinterpret_cast<stream_t*>(stackStart) + i;
            new (dst) stream_t(*src);
        }
        if (stackStart)
            ::operator delete(stackStart);
        stackStart = reinterpret_cast<char*>(nb);
        stackFinish = reinterpret_cast<char*>(nb + count);
        stackEnd = reinterpret_cast<char*>(nb + newCap);
    }

    stream_t* pos = reinterpret_cast<stream_t*>(stackFinish);
    new (pos) stream_t;
    pos->dir = dir ? dir : "";
    pos->filename = filename ? filename : "";
    pos->s = local;
    pos->line_number = this->yylineno;
    stackFinish = reinterpret_cast<char*>(pos + 1);

    yy_buffer_state* new_buffer = this->yy_create_buffer(
        reinterpret_cast<std::istream*>(local.stream), local.size);
    this->yypush_buffer_state(new_buffer);
    this->yy_start = 1;
    this->yylineno = 1;
    return true;
}

// ---- DNFLex::SwitchInputStream(...)（ORIG 0x8acda50）----
// CloseInputStream 后尾调用 PushInputStream。
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

// ---- DNFLex::LexerOutput / LexerError（ORIG 0x8acd190 / 0x8acd1c0）----
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

// ---- DNFLex::EnableInclusion / DisableInclusion（ORIG 0x8acd2d0/2c0）----
void DNFLex::EnableInclusion()
{
    allow_inclusion_ = true;
}

void DNFLex::DisableInclusion()
{
    allow_inclusion_ = false;
}

// ---- DNFLex::IsStream（ORIG 0x8acd260）----
bool DNFLex::IsStream()
{
    return hasStream(this);
}

// ---- DNFLex::tok_str / tok_str_len（ORIG 0x8acd1f0 / 0x8acd200）----
char* DNFLex::tok_str()
{
    return strTokBuf;
}

int DNFLex::tok_str_len()
{
    return static_cast<int>(m_str_tok_pos - strTokBuf);
}

// ---- DNFLex::getLineNumber（ORIG 0x8acd2b0）----
int DNFLex::getLineNumber()
{
    return yylineno;
}

// ---- DNFLex::get_next_token（ORIG 0x8acdaa0）----
// DNF_TOK_EOF == 0；经 yylex 取词；INCLUDE 时读文件名再 PushInputStream，
// EOF 时 PopInputStream(false) 并递归取词。
int DNFLex::get_next_token()
{
    int tok = 0;  // DNF_TOK_EOF
    if (hasStream(this))
    {
        tok = yylex();
        if (allow_inclusion_)
        {
            if (tok == DNF_TOK_INCLUDE)
            {
                stream_t* cur = topStream(this);
                int next = yylex();
                if (next == DNF_TOK_STRING)
                {
                    std::string filename = strTokBuf;
                    if (PushInputStream("", filename.c_str(), 0))
                    {
                        tok = yylex();
                        return tok;
                    }
                    std::snprintf(errBuf, sizeof(errBuf),
                                  "%s - %s/%s, line %d, PushInputStream failed.",
                                  "get_next_token", cur->dir ? cur->dir : "",
                                  cur->filename ? cur->filename : "", yylineno);
                }
                else
                {
                    std::snprintf(errBuf, sizeof(errBuf),
                                  "%s - %s/%s, line %d, invalid inclusion.",
                                  "get_next_token", cur->dir ? cur->dir : "",
                                  cur->filename ? cur->filename : "", yylineno);
                }
                if (lexerError != 0)
                    lexerError(errBuf);
                return DNF_TOK_ERROR;
            }
            if (tok == DNF_TOK_EOF)
            {
                if (PopInputStream(false))
                {
                    tok = yylex();
                    return tok;
                }
            }
        }
    }
    return tok;
}

// ---- DNFLex::DNFLex（ORIG 0x8acd3a0，C1/C2 同址）----
// 调用 __dnf_script__FlexLexer 基类构造（istream 0 / ostream 0），
// memset ERR_BUF(+0x8c) 与 STR_TOK_BUF(+0x208c)（各 0x800 双字），
// stream_stack 三指针清零、m_str_tok_pos=0、is_dbcs_=0、
// allow_inclusion_=1、四个回调清零。vptr 由编译器写入 _ZTV6DNFLex。
DNFLex::DNFLex()
    : __dnf_script__FlexLexer(0, 0),
      stackStart(0), stackFinish(0), stackEnd(0),
      m_str_tok_pos(0), is_dbcs_(0), allow_inclusion_(1),
      isDbcsLeadChar(0), lexerOutput(0), lexerError(0), loadStream(0)
{
    std::memset(errBuf, 0, sizeof(errBuf));
    std::memset(strTokBuf, 0, sizeof(strTokBuf));
}

// ---- DNFLex::~DNFLex（ORIG 0x8acd5f0 D1 / 0x8acd7b0 D0）----
// CloseInputStream 弹空 → 释放 stream_stack 缓冲 → 基类析构。
DNFLex::~DNFLex()
{
    CloseInputStream();
    if (stackStart != 0)
        ::operator delete(stackStart);
    stackStart = stackFinish = stackEnd = 0;
}

// ---------------------------------------------------------------------------
// ---- DNFLex::yylex（ORIG 0x8ad2860，flex DFA 主循环）----
// 证据：docs/class_func_reports/DNFLex/yylex.md（Ghidra 反编译）+ ORIG 汇编。
// 规则 → 记号映射（switch iVar13）：1→DECIMAL(2) 2→HEXADECIMAL(3) 3→FLOAT(4)
// 4→INCLUDE(11) 5→TYPE(5) 6→CONNECTOR(6) 7→CUSTOM_DATA(8) 8→PAIR_INT(9)
// 9→PAIR_STR(10)；规则 0x11（反引号串结束）→STRING(7)；EOF 规则
// 0x16/0x18/0x19→EOF(0)；0x17→comment nest mismatch→ERROR(1)。
// ---------------------------------------------------------------------------
int DNFLex::yylex()
{
    char* yy_cp;
    char* yy_bp;
    unsigned char yy_hold;
    int yy_current_state;
    int rule;

    if (yy_init == 0)
    {
        yy_init = 1;
        if (yy_start == 0)
            yy_start = 1;
        if (yyin == 0)
            yyin = const_cast<std::istream*>(&std::cin);
        if (yyout == 0)
            yyout = const_cast<std::ostream*>(&std::cout);

        yy_buffer_state** stack =
            reinterpret_cast<yy_buffer_state**>(yy_buffer_stack);
        yy_buffer_state* b = (stack != 0) ? stack[yy_buffer_stack_top] : 0;
        if (b == 0)
        {
            yyensure_buffer_stack();
            b = yy_create_buffer(reinterpret_cast<std::istream*>(yyin), 0x4000);
            stack = reinterpret_cast<yy_buffer_state**>(yy_buffer_stack);
            stack[yy_buffer_stack_top] = b;
        }
        yy_n_chars = b->yy_n_chars;
        yy_cp = b->yy_buf_pos;
        yyin = b->yy_input_file;
        yy_c_buf_p = yy_cp;
        yytext = yy_cp;
        yy_hold = static_cast<unsigned char>(*yy_cp);
    }
    else
    {
        yy_cp = yy_c_buf_p;
        yy_hold = static_cast<unsigned char>(*yy_cp);
    }

    for (;;)  // LAB_08ad2896
    {
        *yy_cp = static_cast<char>(yy_hold);
        yy_bp = yy_cp;

        // ---- DFA 扫描（LAB_08ad28a3）----
        yy_current_state = yy_start;
        for (;;)
        {
            int base = g_yy_base[yy_current_state];
            int st = yy_current_state;
            for (;;)
            {
                if (g_yy_accept[st] != 0)
                {
                    yy_last_accepting_state = st;
                    yy_last_accepting_cpos = yy_cp;
                }
                if (st != g_yy_chk[base + yy_hold])
                {
                    do
                    {
                        st = g_yy_def[st];
                        base = g_yy_base[st];
                    } while (g_yy_chk[base + yy_hold] != st);
                }
                yy_cp += 1;
                yy_current_state = g_yy_nxt[base + yy_hold];
                base = g_yy_base[yy_current_state];
                if (base == 0x779)
                    break;
                yy_hold = static_cast<unsigned char>(*yy_cp);
            }
            break;
        }

        // ---- 接受/回退（LAB_08ad292b）----
        rule = g_yy_accept[yy_current_state];
        if (rule == 0)
        {
            yy_cp = yy_last_accepting_cpos;
            rule = g_yy_accept[yy_last_accepting_state];
        }

        yytext = yy_bp;
        yyleng = static_cast<int>(yy_cp - yy_bp);
        yy_hold = static_cast<unsigned char>(*yy_cp);
        *yy_cp = '\0';
        yy_c_buf_p = yy_cp;

        // 行号计数（规则 0x15 EOF 除外）。
        if (rule != 0x15 && g_yy_rule_can_match_eol[rule] != 0 && yyleng > 0)
        {
            const char* p = yytext;
            for (int i = 0; i < yyleng; ++i)
            {
                if (p[i] == '\n')
                    ++yylineno;
            }
        }

        switch (rule)
        {
        case 0:
            // 回退到最近接受位置重扫。
            *yy_cp = static_cast<char>(yy_hold);
            yy_cp = yy_last_accepting_cpos;
            yy_current_state = yy_last_accepting_state;
            rule = g_yy_accept[yy_current_state];
            yytext = yy_bp;
            yyleng = static_cast<int>(yy_cp - yy_bp);
            yy_hold = static_cast<unsigned char>(*yy_cp);
            *yy_cp = '\0';
            yy_c_buf_p = yy_cp;
            break;

        case 1:  return DNF_TOK_DECIMAL;
        case 2:  return DNF_TOK_HEXADECIMAL;
        case 3:  return DNF_TOK_FLOAT;
        case 4:  return DNF_TOK_INCLUDE;
        case 5:  return DNF_TOK_TYPE;
        case 6:  return DNF_TOK_CONNECTOR;
        case 7:  return DNF_TOK_CUSTOM_DATA;
        case 8:  return DNF_TOK_PAIR_INT;
        case 9:  return DNF_TOK_PAIR_STR;

        case 10:
            yy_start = 3;
            break;
        case 0xb:
        case 0xe:
            yy_start = 1;
            break;
        case 0xc:
        case 0xf:
        case 0x12:
            break;
        case 0xd:
            yy_start = 5;
            break;
        case 0x10:
            is_dbcs_ = 0;
            yy_start = 7;
            m_str_tok_pos = strTokBuf;
            break;

        case 0x11:  // 反引号字符串累积
            if (is_dbcs_ == 0)
            {
                char c = yytext[0];
                if (c == '`')
                {
                    *m_str_tok_pos = '\0';
                    yy_start = 1;
                    return DNF_TOK_STRING;
                }
                if (strTokBuf + 0x1ffe <= m_str_tok_pos)
                {
                    if (IsStream())
                    {
                        stream_t* s = GetInputStream();
                        std::snprintf(errBuf, sizeof(errBuf),
                                      "%s - %s/%s, line %d, string buffer overflow detected.",
                                      "yylex", s->dir, s->filename, yylineno);
                        LexerError(errBuf);
                    }
                    else
                    {
                        std::snprintf(errBuf, sizeof(errBuf),
                                      "%s - the input stream is empty.", "yylex");
                        LexerError(errBuf);
                    }
                    return DNF_TOK_ERROR;
                }
                *m_str_tok_pos = c;
                ++m_str_tok_pos;
                is_dbcs_ = (isDbcsLeadChar != 0 && isDbcsLeadChar(c)) ? 1 : 0;
            }
            else
            {
                *m_str_tok_pos = yytext[0];
                ++m_str_tok_pos;
                is_dbcs_ = 0;
            }
            break;

        case 0x13:  // 未识别字符
            if (IsStream())
            {
                stream_t* s = GetInputStream();
                char c = yytext[0];
                if (c == '\0')
                    std::snprintf(errBuf, sizeof(errBuf),
                                  "%s - %s/%s, line %d, unrecognized character 'NULL'.",
                                  "yylex", s->dir, s->filename, yylineno);
                else
                    std::snprintf(errBuf, sizeof(errBuf),
                                  "%s - %s/%s, line %d, unrecognized character '%c'.",
                                  "yylex", s->dir, s->filename, yylineno, c);
                LexerError(errBuf);
                return DNF_TOK_ERROR;
            }
            break;

        case 0x14:  // ECHO
            LexerOutput(yytext, yyleng);
            break;

        case 0x15:  // EOF 处理
        {
            *yy_cp = static_cast<char>(yy_hold);
            yy_buffer_state** stack =
                reinterpret_cast<yy_buffer_state**>(yy_buffer_stack);
            yy_buffer_state* b = stack[yy_buffer_stack_top];
            if (b->yy_buffer_status == 0)
            {
                yy_n_chars = b->yy_n_chars;
                b->yy_buffer_status = 1;
                b->yy_input_file = reinterpret_cast<std::istream*>(yyin);
                b = stack[yy_buffer_stack_top];
            }

            if (yy_c_buf_p <= b->yy_ch_buf + yy_n_chars)
            {
                yy_c_buf_p = yy_cp - 1;
                yy_current_state = yy_get_previous_state();
                int nul = g_yy_NUL_trans[yy_current_state];
                if (nul == 0)
                {
                    yy_cp = yy_c_buf_p;
                    // 重扫（回到接受处理，保留当前 yytext/yyleng 语义）。
                    rule = g_yy_accept[yy_current_state];
                    yy_hold = static_cast<unsigned char>(*yy_cp);
                    *yy_cp = '\0';
                    yy_c_buf_p = yy_cp;
                    if (rule == 0)
                    {
                        yy_cp = yy_last_accepting_cpos;
                        rule = g_yy_accept[yy_last_accepting_state];
                    }
                    break;
                }
                yy_cp = yy_c_buf_p + 1;
                yy_c_buf_p = yy_cp;
                yy_hold = static_cast<unsigned char>(*yy_cp);
                continue;  // 回到 DFA 扫描
            }

            int r = yy_get_next_buffer();
            if (r == 1)
            {
                yy_did_buffer_switch_on_eof = 0;
                if (yywrap() == 0)
                {
                    if (yy_did_buffer_switch_on_eof == 0)
                        yyrestart(reinterpret_cast<std::istream*>(yyin));
                    break;
                }
                // yywrap 非零：按 yy_start 映射伪规则（0x16/0x17/0x18/0x19）。
                rule = (yy_start - 1) / 2 + 0x16;
                if (rule == 0x17)
                {
                    std::snprintf(errBuf, sizeof(errBuf), "comment nest mismatch");
                    LexerError(errBuf);
                    return DNF_TOK_ERROR;
                }
                return DNF_TOK_EOF;
            }
            if (r == 2)
            {
                yy_c_buf_p = b->yy_ch_buf + yy_n_chars;
                yy_get_previous_state();
                yy_cp = yy_c_buf_p;
                rule = g_yy_accept[yy_current_state = yy_last_accepting_state];
                break;
            }
            if (r == 0)
            {
                yy_c_buf_p = yy_cp - 1;
                yy_get_previous_state();
                yy_cp = yy_c_buf_p;
                yy_hold = static_cast<unsigned char>(*yy_cp);
                continue;
            }
            break;
        }

        case 0x16:
        case 0x18:
        case 0x19:
            return DNF_TOK_EOF;

        case 0x17:
            std::snprintf(errBuf, sizeof(errBuf), "comment nest mismatch");
            LexerError(errBuf);
            return DNF_TOK_ERROR;

        default:
            LexerError("fatal flex scanner internal error--no action found");
            break;
        }

        yy_cp = yy_c_buf_p;
        yy_hold = static_cast<unsigned char>(*yy_cp);
    }
}

// ---- Flex DFA 表（ORIG .rodata 数据转写，见 DNFLex.h 注释地址）----
extern "C" const short g_yy_accept[57] =
{
    0, 7, 7, 0, 0, 0, 0, 0, 0, 21, 19, 18, 18, 7, 19, 19,
    19, 1, 1, 19, 19, 19, 16, 12, 12, 12, 15, 14, 14, 17, 17, 18,
    0, 1, 10, 13, 0, 0, 0, 0, 0, 0, 11, 6, 3, 2, 9, 8,
    0, 5, 0, 0, 0, 0, 0, 4, 0,
};
extern "C" const short g_yy_def[64] =
{
    0, 57, 57, 58, 58, 59, 59, 60, 60, 56, 56, 56, 56, 56, 61, 56,
    56, 56, 56, 62, 56, 63, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    61, 56, 56, 56, 56, 56, 62, 56, 56, 63, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 0, 56, 56, 56, 56, 56, 56, 56,
};
extern "C" const short g_yy_base[64] =
{
    0, 0, 54, 4, 17, 1, 2, 163, 162, 174, 1913, 9, 52, 1913, 0, 20,
    36, 67, 79, 0, 103, 0, 1913, 1913, 1913, 116, 1913, 1913, 1913, 1913, 1913, 129,
    108, 116, 1913, 1913, 140, 150, 86, 173, 37, 53, 1913, 1913, 184, 205, 1913, 1913,
    45, 1913, 35, 24, 40, 36, 4, 1913, 1913, 307, 562, 817, 1072, 1327, 1535, 1657,
};
extern "C" const short g_yy_chk[2170] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 5, 6, 1, 5, 6,
    0, 3, 11, 11, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 4, 0,
    1, 1, 0, 0, 0, 0, 1, 0, 1, 11, 0, 1, 1, 1, 3, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 12, 12, 2,
    2, 12, 54, 2, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 0,
    0, 0, 0, 16, 12, 0, 2, 2, 0, 0, 0, 1, 2, 0, 2, 0,
    1, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 17, 2, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 1, 18,
    18, 18, 18, 18, 18, 18, 18, 18, 18, 53, 31, 31, 52, 51, 31, 50,
    48, 2, 41, 40, 38, 32, 2, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 31, 33, 25, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 9, 8,
    7, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 17, 36, 36, 36, 36,
    36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    20, 0, 0, 0, 0, 0, 0, 37, 37, 37, 37, 37, 37, 39, 39, 39,
    39, 39, 39, 39, 39, 39, 39, 39, 44, 44, 44, 44, 44, 44, 44, 44,
    44, 44, 0, 0, 0, 0, 0, 37, 37, 37, 37, 37, 37, 45, 45, 45,
    45, 45, 45, 45, 45, 45, 45, 0, 0, 0, 0, 0, 0, 0, 45, 45,
    45, 45, 45, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 45,
    45, 45, 45, 45, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
    57, 57, 57, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
    58, 58, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 61, 61, 0, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 0, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
    61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 62,
    62, 62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0, 0, 0, 0,
    62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
    62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0, 62, 0,
    62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
    62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 0, 63, 63, 0, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 0, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
};
extern "C" const short g_yy_nxt[2170] =
{
    0, 56, 56, 56, 56, 56, 56, 56, 56, 11, 12, 27, 27, 11, 28, 28,
    56, 24, 31, 31, 56, 56, 31, 56, 56, 56, 56, 56, 56, 56, 24, 56,
    11, 13, 56, 56, 56, 56, 13, 56, 14, 31, 56, 15, 13, 15, 25, 16,
    17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 25, 20, 31, 31, 11,
    12, 31, 55, 11, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 56,
    56, 56, 56, 35, 31, 56, 11, 13, 56, 56, 56, 21, 13, 56, 14, 56,
    22, 15, 13, 15, 56, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18,
    19, 36, 20, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 36, 13, 33,
    33, 33, 33, 33, 33, 33, 33, 33, 33, 54, 31, 31, 53, 52, 31, 51,
    50, 21, 49, 48, 46, 43, 22, 39, 39, 39, 39, 39, 39, 39, 39, 39,
    39, 31, 36, 42, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 56, 30,
    30, 56, 56, 56, 13, 56, 56, 56, 56, 56, 56, 37, 44, 44, 44, 44,
    44, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    40, 56, 56, 56, 56, 56, 56, 45, 45, 45, 45, 45, 45, 39, 39, 39,
    39, 39, 39, 39, 39, 39, 39, 47, 44, 44, 44, 44, 44, 44, 44, 44,
    44, 44, 56, 56, 56, 56, 56, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    45, 45, 45, 45, 45, 45, 45, 56, 56, 56, 56, 56, 56, 56, 45, 45,
    45, 45, 45, 45, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 45, 45,
    45, 45, 45, 45, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 56, 32, 32, 56, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 56, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 38,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 56, 56, 56, 56, 56, 56, 56,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 56, 56, 56, 56, 38, 56,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 56, 41, 41, 56, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 56, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
    41, 41, 41, 41, 41, 41, 41, 41, 41, 9, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
};
extern "C" const int g_yy_NUL_trans[56] =
{
    0, 10, 10, 23, 23, 26, 26, 29, 29, 0, 0, 0,
    0, 0, 32, 0, 0, 0, 0, 0, 0, 41, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0,
    0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
extern "C" const unsigned char g_yy_rule_can_match_eol[84] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0,
};
