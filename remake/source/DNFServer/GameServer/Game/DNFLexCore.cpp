// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DNFLexCore/<Func>.inc（不参与编译、内部不含 #include）。
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

// ---- DNFLex::stream_data_t::clear()（ORIG 0x8acd140）----
#include "DNFLexCore/clear.inc"
{
    stream = 0;
    size = 0;
    buffer = 0;
}

// ---- DNFLex::GetInputStream()（ORIG 0x8acd280）----
#include "DNFLexCore/GetInputStream.inc"
{
    stream_t* finish = stream_stack.end().base();
    stream_t* start = stream_stack.begin().base();
    unsigned empty = (unsigned)(finish == start);
    unsigned mask = empty - 1u;
    return reinterpret_cast<stream_t*>((unsigned)(finish - 1) & mask);
}

// ---- DNFLex::getStreamName()（ORIG 0x8acd310，tstring RVO）----
// [b28d-19] N：N7/N82 —— 同上（get_next_token 同族）；ORIG 0x08acd310。
// 【b28e-M1S2 本波复核】orig 43 / ours 64（我方多 21）。真因 = **GCC 4.4.7 在
// 「函数含多个 return 对象」时放弃 NRVO**，而 ORIG 编译器不放弃：
//   · ORIG 0x08acd330：`lea -0x18(%esi),%eax; sub $0x14,%esi; mov %eax,0x4(%esp);
//     mov %ebx,(%esp); call _ZNSsC1ERKSs`（%ebx = 0x8(%ebp) = sret）⇒ 具名 result
//     直接构造在返回槽，全程无第二次 copy、无局部 dtor；
//   · 我方 0x2f4 起：ctor 到 -0x20(%ebp) → append → `call _ZNSsC1ERKSs(sret,tmp)`
//     → 内联 COW 释放（`lock xadd` 引用计数块）⇒ 恰合 21 条差。
//   同编译器隔离探针（本 TU 全旗标 -O2 -std=gnu++98）：/tmp/probe2.s 的 `g`
//   （单一 return）走 NRVO、`h`（双 return）恒退化；/tmp/probe3.s 把 NONE 支改成
//   具名 `std::string none("NONE"); return none;` 亦退化 ⇒ 无源级形可造。
//   新钥匙核过：P339/P331（-O0 槽轴；本 TU 为 per-source -O2 路由，不适用）、
//   P226/P231/P248（均不触发 NRVO）⇒ 判 N（编译器 NRVO 策略差）封顶。
#include "DNFLexCore/getStreamName.inc"
{
    stream_t* finish = stream_stack.end().base();
    if (finish == stream_stack.begin().base())
        return std::string("NONE");
    std::string result((finish - 1)->dir);
    result.append((finish - 1)->filename);
    return result;
}

// ---- DNFLex::CloseInputStream()（ORIG 0x8acd5b0）----
#include "DNFLexCore/CloseInputStream.inc"
{
    if (stream_stack.end().base() != stream_stack.begin().base())
    {
        do
        {
            PopInputStream(true);
        } while (stream_stack.end().base() != stream_stack.begin().base());
    }
}

// ---- DNFLex::stream_data_t::destroy()（ORIG 0x8acd450）----
#include "DNFLexCore/destroy.inc"
{
    if (stream != 0)
    {
        void (**vtable)(void*) = *reinterpret_cast<void (***)(void*)>(stream);
        vtable[1](stream);
        stream = 0;
    }
    if (buffer != 0)
    {
        ::operator delete[](buffer);   // ORIG _ZdaPv（buffer 由 new[] 分配）
        buffer = 0;
    }
}

// ---- DNFLex::PopInputStream(bool)（ORIG 0x8acd490）----
#include "DNFLexCore/PopInputStream.inc"
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
#include "DNFLexCore/LoadStream.inc"
{
    if (loadStream == 0)
        return false;
    return loadStream(dir, filename, s);
}

// ---- DNFLex::PushInputStream(...)（ORIG 0x8acd7d0）----
// [b28d-19] AE（真实构建口径 = flags.make + build.make per-source `-O2 -std=gnu++98`；该口径下 178=178 逐条对齐。check_tu_routed.sh 曾漏 per-source 追加 flags 按 -O0 假报 DIFF，Main 已修）；ORIG 0x08acd7d0。
#include "DNFLexCore/PushInputStream.inc"
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
#include "DNFLexCore/SwitchInputStream.inc"
{
    CloseInputStream();
    return PushInputStream(dir, filename, s);
}

// ---- DNFLex::SetHandler(...)（ORIG 0x8acd160）----
#include "DNFLexCore/SetHandler.inc"
                        TLexerError error, TLoadStream load)
{
    isDbcsLeadChar = isDbcs;
    lexerOutput = output;
    lexerError = error;
    loadStream = load;
}

#include "DNFLexCore/LexerOutput.inc"
{
    if (lexerOutput != 0)
        lexerOutput(buf, size);
}

#include "DNFLexCore/LexerError.inc"
{
    if (lexerError != 0)
        lexerError(msg);
}

#include "DNFLexCore/EnableInclusion.inc"
{
    allow_inclusion_ = true;
}

#include "DNFLexCore/DisableInclusion.inc"
{
    allow_inclusion_ = false;
}

#include "DNFLexCore/IsStream.inc"
{
    return stream_stack.end().base() != stream_stack.begin().base();
}

#include "DNFLexCore/tok_str.inc"
{
    return strTokBuf;
}

#include "DNFLexCore/tok_str_len.inc"
{
    return static_cast<int>(m_str_tok_pos - strTokBuf);
}

#include "DNFLexCore/getLineNumber.inc"
{
    return yylineno;
}

// ---- DNFLex::get_next_token（ORIG 0x8acdaa0）----
// [b28d-19] N：N7/N82 —— 4.1 callee-saved 栈帧头保存形 + 寄存器分配序差，块内条数/助记符集全等；ORIG 0x08acdaa0。
#include "DNFLexCore/get_next_token.inc"
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

#include "DNFLexCore/DNFLex.inc"
    : __dnf_script__FlexLexer(0, 0),
      m_str_tok_pos(0), is_dbcs_(0), allow_inclusion_(1),
      isDbcsLeadChar(0), lexerOutput(0), lexerError(0), loadStream(0)
{
    std::memset(errBuf, 0, sizeof(errBuf));
    std::memset(strTokBuf, 0, sizeof(strTokBuf));
}

#include "DNFLexCore/dtor_DNFLex.inc"
{
    CloseInputStream();
}
