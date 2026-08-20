// ============================================================================
// DNFLex 基础方法（词法引擎批次；ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLex.md 与 DNFLex__stream_data_t.md）
// ----------------------------------------------------------------------------
// 本 TU 承载 DNFLex（_ZN6DNFLex*，即 DNFLexWrapper.dnfLex_ 子对象，wrapper+0x18）
// 的基础流方法，与 DNFLexWrapper.cpp / DNFLexWrapperStream.cpp /
// GameStubs_remaining.cpp（DNFLexWrapper / 取词方法）以及 CompiledLexCore 的
// CompiledDNFLexCore.cpp（_ZN14CompiledDNFLex*）互补，无重复定义。
//
// 本 TU 实现（ORIG 地址 / 汇编证据）：
//   GetInputStream()      _ZN6DNFLex14GetInputStreamEv  ORIG 0x8acd280
//   getStreamName()       _ZN6DNFLex13getStreamNameEv   ORIG 0x8acd310
//   CloseInputStream()    _ZN6DNFLex16CloseInputStreamEv ORIG 0x8acd5b0
//   SetHandler(...)       _ZN6DNFLex10SetHandlerEPFbcEPFvPKciEPFvS3_EPFbS3_S3_PNS_13stream_data_tEE
//                                                     ORIG 0x8acd160
//   PopInputStream(bool)  _ZN6DNFLex14PopInputStreamEb  ORIG 0x8acd490（支撑）
//   LoadStream(...)       _ZN6DNFLex10LoadStreamEPKcS1_PNS_13stream_data_tE
//                                                     ORIG 0x8acd220（支撑）
//   PushInputStream(...)  _ZN6DNFLex15PushInputStreamEPKcS1_PNS_13stream_data_tE
//                                                     ORIG 0x8acd7d0（支撑）
//   SwitchInputStream(...) _ZN6DNFLex17SwitchInputStreamEPKcS1_PNS_13stream_data_tE
//                                                     ORIG 0x8acda50
//   stream_data_t::destroy() _ZN6DNFLex13stream_data_t7destroyEv
//                                                     ORIG 0x8acd450（支撑）
// 引擎符号 yypush_buffer_state（0x8ad2420）/ yyensure_buffer_stack
// （0x8ad2350）由 FlexLexerCore.cpp 提供（同 yypop 先例）。
//
// 对象布局依据 ORIG DNFLex::DNFLex（0x8acd3a0）与 DNFLex::PushInputStream
// （0x8acd7d0）：
//   +0x00 vptr（__dnf_script__FlexLexer / FlexLexer 基类 vptr）
//   +0x04 FlexLexer.yytext（DNFLexWrapper 文档：wrapper+0x1c = dnfLex_+0x04）
//   +0x0c yylineno（FlexLexer 基类；PopInputStream: mov %eax,0xc(%ebx)）
//   +0x38 yy_start（FlexLexer 基类；PopInputStream: movl $1,0x38(%ebx)）
//   +0x80 stream_stack_._M_start（std::vector<DNFLex::stream_t>）
//   +0x84 stream_stack_._M_finish
//   +0x88 stream_stack_._M_end_of_storage
//
// DNFLex::stream_t（0x18 字节，元素 = 24；DWARF/PushInputStream 证据）：
//   +0x00 dir（TCHAR*；源流目录）
//   +0x04 filename（TCHAR*）
//   +0x08 stream_data_t{ istream* stream; int size; char* buffer }
//          （stream_data_t 起点 +0x08；size @ +0x0c；buffer @ +0x10）
//   +0x14 line_number
// 该 +0x08/+0x0c/+0x10 偏移被 DNFLexWrapperStream.cpp 的 getInputStream /
// getInputStreamSize / getInputStreamBuffer 以裸指针固定读取，故必须保持。
//
// 注意（重建契约）：ORIG 中 dir/filename 为 COW std::string（共享引用计数）。
// 本重建以普通 const char* 建模（保持 stream_t 0x18 布局），PopInputStream
// 不接管 dir/filename 的所有权/释放（由 PushInputStream / TLoadStream 批次
// 负责创建与生命周期），仅关闭 stream_data_t 的 istream 与 buffer——与
// DNFLexWrapperStream.cpp 裸偏移契约一致，避免对未拥有指针做释放。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <new>
#include <string>
#include <iostream>

namespace
{

// DNFLex 对象基类布局代理（本 TU 独有；覆盖 FlexLexer 基类全部字段与
// DNFLex 特有字段，yylex（0x8ad2860）按此访问）。
struct DnfLexState
{
    void*   vptr;            // +0x00 FlexLexer 基类 vptr
    char*   yytext;          // +0x04 FlexLexer.yytext
    int     yyleng;          // +0x08 FlexLexer.yyleng（yylex: mov %edx,0x8(%esi)）
    int     yylineno;        // +0x0c FlexLexer.yylineno
    int     yy_flex_debug;   // +0x10
    int     yy_start_stack_ptr;    // +0x14
    int     yy_start_stack_depth;  // +0x18
    int*    yy_start_stack;  // +0x1c
    void*   yyin;            // +0x20
    void*   yyout;           // +0x24
    char    yy_hold_char;    // +0x28
    char    pad29[3];
    int     yy_n_chars;      // +0x2c
    char*   yy_c_buf_p;      // +0x30
    int     yy_init;         // +0x34
    int     yy_start;        // +0x38
    int     yy_did_buffer_switch_on_eof;  // +0x3c
    int     yy_buffer_stack_top;          // +0x40
    int     yy_buffer_stack_max;          // +0x44
    void**  yy_buffer_stack;              // +0x48
    int     yy_last_accepting_state;      // +0x4c
    char*   yy_last_accepting_cpos;       // +0x50
    char*   yy_state_buf;                 // +0x54
    char*   yy_state_ptr;                 // +0x58
    char*   yy_full_match;                // +0x5c
    int*    yy_full_state;                // +0x60
    char    pad64[8];        // +0x64 .. +0x6b
    int     yy_looking_for_trail_begin;   // +0x6c
    int     yy_more_flag;    // +0x70
    int     yy_more_len;     // +0x74
    int     yy_more_offset;  // +0x78
    int     yy_prev_more_offset;          // +0x7c
    char*   stackStart;      // +0x80 stream_stack_._M_start
    char*   stackFinish;     // +0x84 stream_stack_._M_finish
    char*   stackEnd;        // +0x88 stream_stack_._M_end_of_storage
    char    errBuf[0x2000];  // +0x8c ERR_BUF（DNFLex 构造 rep stos 0x800 双字）
    char*   m_str_tok_pos;   // +0x408c STR_TOK_BUF 写入游标
    char    is_dbcs_;        // +0x4090 DBCS 双字节状态
    char    allow_inclusion_; // +0x4091 允许 include
    char    pad4092[2];      // +0x4092 .. +0x4093
    // +0x4094..+0x40a0 四个回调（SetHandler 写入）
    int (*isDbcsLeadChar)(char);                     // +0x4094
    void (*lexerOutput)(const char*, int);           // +0x4098
    void (*lexerError)(const char*);                 // +0x409c
    int (*loadStream)(const char*, const char*, void*);  // +0x40a0
    char    strTokBuf[0x2000];  // +0x40a4 STR_TOK_BUF（DNFLex 构造 rep stos）
};

// DNFLex::stream_t（0x18 字节）。
struct DnfLexStream
{
    const char* dir;         // +0x00
    const char* filename;    // +0x04
    void*       stream;      // +0x08  stream_data_t.stream（istream*）
    int         size;        // +0x0c  stream_data_t.size
    char*       buffer;      // +0x10  stream_data_t.buffer
    int         line_number; // +0x14
};
struct DnfStreamDataView
{
    void* stream;
    int size;
    char* buffer;
};

// 顶层元素 = &((DnfLexStream*)stackFinish)[-1] 位于 stackFinish - 0x18。
inline DnfLexStream* topStream(DnfLexState* s)
{
    return reinterpret_cast<DnfLexStream*>(s->stackFinish) - 1;
}

// 判断流栈非空。
inline bool hasStream(DnfLexState* s)
{
    return s->stackFinish != s->stackStart;
}

// ---- 底层符号（前向声明 / 引擎批次提供）----
// __dnf_script__FlexLexer::yypop_buffer_state()（FlexLexer 引擎）
//   _ZN23__dnf_script__FlexLexer18yypop_buffer_stateEv
// __dnf_script__FlexLexer::yypush_buffer_state()（FlexLexer 引擎）
//   _ZN23__dnf_script__FlexLexer19yypush_buffer_stateEP15yy_buffer_state
// 归属：词法引擎批次（yypop/yypush/set_line_num 等 _ZN23* 由 Main 统一分配；
// 当前由 FlexLexerCore.cpp 提供）。
}  // namespace

// ---- 导出符号（asm 标签对齐 ORIG）----

// ---- 本 TU 支撑方法（ORIG 0x8acd450）----
// DNFLex::stream_data_t::destroy()：关闭 istream（经其基类 vtable 槽 +4，
// 即 istream 析构），释放 buffer（operator delete[]）。this = &stream_data_t
// （= &finish[-1].s，位于 stream_t + 0x8）。
extern "C" void* sub_DNFLex_GetInputStream(void* self)
{
    DnfLexState* s = reinterpret_cast<DnfLexState*>(self);
    if (!hasStream(s))
        return 0;
    return reinterpret_cast<char*>(s->stackFinish) - 0x18;   // &finish[-1]
}

// ---- DNFLex::getStreamName()（ORIG 0x8acd310，tstring RVO 返回）----
// 返回顶层流的 dir+filename 拼接；栈空返回 "NONE"。
extern "C" std::string sub_DNFLex_getStreamName(void* self)
    asm("_ZN6DNFLex13getStreamNameEv");
extern "C" std::string sub_DNFLex_getStreamName(void* self)
{
    DnfLexState* s = reinterpret_cast<DnfLexState*>(self);
    if (hasStream(s))
    {
        DnfLexStream* t = topStream(s);
        std::string result(t->dir ? t->dir : "");        // ORIG: _ZNSsC1ERKSs(副本)
        if (t->filename)                                  // ORIG: _ZNSs6appendERKSs
            result.append(t->filename);
        return result;
    }
    return std::string("NONE");   // ORIG: _ZNSsC1EPKcRKSaIcE("NONE", @0x8e2eb1f)
}

// ---- DNFLex::CloseInputStream()（ORIG 0x8acd5b0）----
// 反复弹出流栈至空（ORIG 为 do-while 调用 PopInputStream(true)）。

// ---- DNFLex::DNFLex（ORIG 0x8acd3a0，C1/C2 同址）----

// DNFLex vtable（_ZTV6DNFLex，定义于本 TU 末尾）。
extern "C" const void* g_dnfLex_vtable[] __asm__("_ZTV6DNFLex");


struct DnfLexTypeInfo
{
    const void* vptr;
    const char* typeName;
};
extern "C" const struct DnfLexTypeInfo g_dnfLex_typeinfo __asm__("_ZTI6DNFLex");
extern "C" const struct DnfLexTypeInfo g_dnfLex_typeinfo = {
    reinterpret_cast<const char*>(&g_cxxabi_classTypeInfo_vtable3) + 8,
    g_dnfLex_typeinfoName,
};

// DNFLex::yylex（ORIG 0x8ad2860）使用下方提取的 flex DFA 表；
// 保持槽 6 与 _ZN6DNFLex5yylexEv ABI 不变。

// 引擎槽 2-9 符号（FlexLexerCore.cpp 提供）。
extern "C" void* sub_FlexLexer_yy_create_buffer(void* self, void* file, int size)
    asm("_ZN23__dnf_script__FlexLexer16yy_create_bufferEPSii");

extern "C" const void* g_dnfLex_vtable[] = {
    0,
    &g_dnfLex_typeinfo,
    reinterpret_cast<const void*>(&sub_DNFLex_D1),
    reinterpret_cast<const void*>(&sub_DNFLex_D0),
    reinterpret_cast<const void*>(&sub_FlexLexer_yy_switch_to_buffer),
    reinterpret_cast<const void*>(&sub_FlexLexer_yy_create_buffer),
    reinterpret_cast<const void*>(&sub_FlexLexer_yy_delete_buffer),
    reinterpret_cast<const void*>(&sub_FlexLexer_yyrestart),
    reinterpret_cast<const void*>(&sub_DNFLex_yylex),
    reinterpret_cast<const void*>(&sub_FlexLexer_switch_streams),
    reinterpret_cast<const void*>(&sub_FlexLexer_yywrap),
    reinterpret_cast<const void*>(&sub_FlexLexer_LexerInput),
    reinterpret_cast<const void*>(&sub_DNFLex_LexerOutput),
    reinterpret_cast<const void*>(&sub_DNFLex_LexerError),
};

// ============================================================================
// ---- DNFLex::yylex（ORIG 0x8ad2860）----
extern "C" const short g_yy_accept[], g_yy_def[], g_yy_base[], g_yy_chk[], g_yy_nxt[];
extern "C" const int g_yy_rule_can_match_eol[];
extern "C" const int g_yy_NUL_trans[];
extern "C" int sub_DNFLex_yylex(void* self)
{
    DnfLexState* s = reinterpret_cast<DnfLexState*>(self);
    struct B { void* in; char* ch; char* pos; int size,n,own,interactive,bol,line,col,fill,status; };
    if (!s->yy_init) {
        s->yy_init=1; if (!s->yy_start) s->yy_start=1;
        if (!s->yyin) s->yyin=&std::cin; if (!s->yyout) s->yyout=&std::cout;
        if (!s->yy_buffer_stack || !s->yy_buffer_stack[s->yy_buffer_stack_top]) {
            sub_FlexLexer_yyensure_buffer_stack(self);
            void*** v=reinterpret_cast<void***>(s->vptr);
            s->yy_buffer_stack[s->yy_buffer_stack_top]=reinterpret_cast<void*(*)(void*,void*,int)>(v[3])(self,s->yyin,0x4000);
        }
        B* b=reinterpret_cast<B*>(s->yy_buffer_stack[s->yy_buffer_stack_top]);
        s->yy_n_chars=b->n; s->yy_c_buf_p=b->pos; s->yytext=b->pos; s->yyin=b->in; s->yy_hold_char=*b->pos;
    }
    for (;;) {
        *s->yy_c_buf_p=s->yy_hold_char; char* bp=s->yy_c_buf_p; int st=s->yy_start;
        for (;;) { if(g_yy_accept[st]){s->yy_last_accepting_state=st;s->yy_last_accepting_cpos=s->yy_c_buf_p;}
            int x=g_yy_base[st]+(unsigned char)*s->yy_c_buf_p;
            while(st!=g_yy_chk[x]){st=g_yy_def[st];x=g_yy_base[st]+(unsigned char)*s->yy_c_buf_p;}
            ++s->yy_c_buf_p; st=g_yy_nxt[x];
            if (*s->yy_c_buf_p == 0) {
                int nul = g_yy_NUL_trans[st];
                if (nul == 0) break;
                st = nul;
            }
            if(g_yy_base[st]==0x779) break;
        }
        int a=g_yy_accept[st];
        if(!a){
            s->yy_c_buf_p=s->yy_last_accepting_cpos;
            st=s->yy_last_accepting_state;
            a=g_yy_accept[st];
        }
        s->yytext=bp; s->yyleng=s->yy_c_buf_p-bp; s->yy_hold_char=*s->yy_c_buf_p; *s->yy_c_buf_p=0;
        if(a!=0x15&&g_yy_rule_can_match_eol[a]) for(int i=0;i<s->yyleng;i++) if(s->yytext[i]=='\n') ++s->yylineno;
        switch(a) {
        case 1:return 2; case 2:return 3; case 3:return 4; case 4:return 11; case 5:return 5; case 6:return 6; case 7:return 8; case 8:return 9; case 9:return 10;
        case 10:s->yy_start=3; break; case 11: case 14:s->yy_start=1; break; case 13:s->yy_start=5; break;
        case 16:s->is_dbcs_=0; s->yy_start=7; s->m_str_tok_pos=s->strTokBuf; break;
        case 17:
            if(!s->is_dbcs_) { if(*s->yytext=='`'){*s->m_str_tok_pos=0;s->yy_start=1;return 7;}
                if(s->m_str_tok_pos>=s->strTokBuf+0x1ffe) { if(hasStream(s)){DnfLexStream*t=topStream(s);std::snprintf(s->errBuf,sizeof(s->errBuf),"yylex - %s/%s, line %d, string buffer overflow detected.",t->dir,t->filename,s->yylineno);} else std::snprintf(s->errBuf,sizeof(s->errBuf),"yylex - the input stream is empty."); if(s->lexerError)s->lexerError(s->errBuf); return 1; }
                *s->m_str_tok_pos++=*s->yytext; s->is_dbcs_=s->isDbcsLeadChar&&s->isDbcsLeadChar(*s->yytext);
            } else {*s->m_str_tok_pos++=*s->yytext;s->is_dbcs_=0;} break;
        case 19:
            if(hasStream(s)){DnfLexStream*t=topStream(s); if(!*s->yytext) std::snprintf(s->errBuf,sizeof(s->errBuf),"yylex - %s/%s, line %d, unrecognized character 'NULL'.",t->dir,t->filename,s->yylineno); else std::snprintf(s->errBuf,sizeof(s->errBuf),"yylex - %s/%s, line %d, unrecognized character '%c'.",t->dir,t->filename,s->yylineno,*s->yytext); if(s->lexerError)s->lexerError(s->errBuf); return 1;} break;
        case 20:if(s->lexerOutput)s->lexerOutput(s->yytext,s->yyleng);break;
        case 21:{int r=sub_FlexLexer_yy_get_next_buffer(self); if(r==2){B*b=reinterpret_cast<B*>(s->yy_buffer_stack[s->yy_buffer_stack_top]);s->yy_c_buf_p=b->ch+s->yy_n_chars;sub_FlexLexer_yy_get_previous_state(self);continue;} if(r==0){sub_FlexLexer_yy_get_previous_state(self);continue;} if(r==1){s->yy_did_buffer_switch_on_eof=0;int w=reinterpret_cast<int(*)(void*)>(reinterpret_cast<void**>(s->vptr)[8])(self);if(!w){if(!s->yy_did_buffer_switch_on_eof)reinterpret_cast<void(*)(void*,void*)>(reinterpret_cast<void**>(s->vptr)[5])(self,s->yyin);break;}s->yy_c_buf_p=s->yytext;a=(s->yy_start-1)/2+22; if(a==22||a==24||a==25)return 0; if(a==23){if(s->lexerError)s->lexerError("comment nest mismatch");return 1;}} return 0;}
        case 22: case 24: case 25:return 0; case 23:if(s->lexerError)s->lexerError("comment nest mismatch");return 1;
        default:if(s->lexerError)s->lexerError("fatal flex scanner internal error--no action found");break;
        }
    }
}

// flex 扫描器主循环。依赖 DFA 表（ORIG .rodata 0x8e2f180 起，flex 生成）：
//   yy_accept @ 0x8e2f180（short[64]）  yy_def @ 0x8e2f200（short[64]）
//   yy_base @ 0x8e2f280（short[64]）    yy_chk @ 0x8e2f300（short[2176]）
//   yy_nxt @ 0x8e30400（short[2176]）   yy_NUL_trans @ 0x8e2f0a0（int[56]）
//   yy_rule_can_match_eol @ 0x8e31500（int[26]）
// 表内容为 ORIG 数据区逐字节提取（只读常量），保证 DFA 语义与 ORIG 一致。
// ============================================================================
extern "C" const short g_yy_accept[] = {
    0, 7, 7, 0, 0, 0, 0, 0, 0, 21, 19, 18, 18, 7, 19, 19,
    19, 1, 1, 19, 19, 19, 16, 12, 12, 12, 15, 14, 14, 17, 17, 18,
    0, 1, 10, 13, 0, 0, 0, 0, 0, 0, 11, 6, 3, 2, 9, 8,
    0, 5, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0,
};

extern "C" const short g_yy_def[] = {
    0, 57, 57, 58, 58, 59, 59, 60, 60, 56, 56, 56, 56, 56, 61, 56,
    56, 56, 56, 62, 56, 63, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
    61, 56, 56, 56, 56, 56, 62, 56, 56, 63, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 56, 0, 56, 56, 56, 56, 56, 56, 56,
};

extern "C" const short g_yy_base[] = {
    0, 0, 54, 4, 17, 1, 2, 163, 162, 174, 1913, 9, 52, 1913, 0, 20,
    36, 67, 79, 0, 103, 0, 1913, 1913, 1913, 116, 1913, 1913, 1913, 1913, 1913, 129,
    108, 116, 1913, 1913, 140, 150, 86, 173, 37, 53, 1913, 1913, 184, 205, 1913, 1913,
    45, 1913, 35, 24, 40, 36, 4, 1913, 1913, 307, 562, 817, 1072, 1327, 1535, 1657,
};

extern "C" const short g_yy_chk[] = {
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
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 0, 0, 0, 0, 0, 0,
};

extern "C" const short g_yy_nxt[] = {
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
    56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 0, 0, 0, 0, 0, 0,
};

extern "C" const int g_yy_NUL_trans[] = {
    0, 10, 10, 23, 23, 26, 26, 29, 29, 0, 0, 0,
    0, 0, 32, 0, 0, 0, 0, 0, 0, 41, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0,
    0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

extern "C" const int g_yy_rule_can_match_eol[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 1, 0, 0, 1, 1, 0, 0, 1701607801, 120, 0,
    0, 149099956,
};
