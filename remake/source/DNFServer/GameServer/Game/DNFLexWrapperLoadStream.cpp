// ============================================================================
// DNFLexWrapper 流装载两方法（G4 脚本批次；ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLexWrapper.md）
// ----------------------------------------------------------------------------
// 本 TU 提供：
//   LoadStream()       _ZN13DNFLexWrapper10LoadStreamEPKcS1_PN6DNFLex13stream_data_tE
//                                                                ORIG 0x8ace2b0
//   setFileStreamData() _ZN13DNFLexWrapper17setFileStreamDataEPN6DNFLex13stream_data_tEPKc
//                                                                ORIG 0x8acede0
//
// 依赖符号（由 LexSwitchStream 在 DNFLexWrapperStream.cpp 提供，本 TU 仅
// 按 ORIG ABI 声明并调用，不做重复实现）：
//   DNFLex::stream_data_t::clear()   _ZN6DNFLex13stream_data_t5clearEv  ORIG 0x8acd140
//   setStringStreamData()            _ZN13DNFLexWrapper19setStringStreamDataEPN6DNFLex13stream_data_tEPcj
//                                                                ORIG 0x8ace9b0
//
// 字段布局依据 ORIG DNFLexWrapper::DNFLexWrapper（0x8aced60）与
// SetHandler（0x8acec80，存储 LoadStream_ @ +0x610c）：
//   +0x00 vptr   +0x04 lastToken_   +0x08 lastIntValue_
//   +0x0c lastFloatValue_           +0x10 lastStringValue_   +0x14 lexType_
//   +0x18 dnfLex_（DNFLex 子对象）/+0x40bc compiledDnfLex_（CompiledDNFLex
//   子对象）/+0x6108 LexerError_ / +0x610c LoadStream_，总大小 0x6110。
//
// DNFLex::stream_data_t（3 个 4 字节字段，与 DNFLexCore.cpp /
// DNFLexWrapperStream.cpp 裸偏移契约一致）：
//   +0x00 stream（istream*）  +0x04 size（int）  +0x08 buffer（char*）
// ============================================================================

#include <fstream>

namespace
{

// TLoadStream：SetHandler 的第四回调（bool (*)(TCHAR*, TCHAR*, stream_data_t*)，
// 见 DNFLexWrapper.md SetHandler DWARF 原型与 DNFLex::SetHandler 符号
// EPFbS3_S3_PNS_13stream_data_tEE）。
typedef bool (*TLoadStream)(char* dir, char* filename, void* s);

// DNFLexWrapper 布局代理（本 TU 独有副本；仅命名本 TU 用到的偏移）。
struct LexWrapperState
{
    void*          vptr;             // +0x00
    int            lastToken_;       // +0x04  DNFLEX_TOKEN
    int            lastIntValue_;    // +0x08
    float          lastFloatValue_;  // +0x0c
    char*          lastStringValue_; // +0x10
    int            lexType_;         // +0x14  0=NORMAL 1=COMPILED
    unsigned char  pad18[0x60f0];    // +0x18 .. +0x6107（dnfLex_ + compiledDnfLex_）
    void*          lexerError_;      // +0x6108
    TLoadStream    loadStream_;      // +0x610c
};

// DNFLex::stream_data_t 布局代理（stream @ +0x00 / size @ +0x04 / buffer @ +0x08）。
struct StreamData
{
    void* stream;   // +0x00 istream*
    int   size;     // +0x04
    char* buffer;   // +0x08
};

// ---- 底层符号（由 LexSwitchStream 在 DNFLexWrapperStream.cpp 提供）----
// DNFLex::stream_data_t::clear()          _ZN6DNFLex13stream_data_t5clearEv
// DNFLexWrapper::setStringStreamData()    _ZN13DNFLexWrapper19setStringStreamDataEPN6DNFLex13stream_data_tEPcj
extern "C" bool sub_lex_setStringStreamData(void* self, void* s, char* buffer,
                                            unsigned int bufferSize)
    asm("_ZN13DNFLexWrapper19setStringStreamDataEPN6DNFLex13stream_data_tEPcj");

}  // namespace

// LoadStream（ORIG 0x8ace2b0）：若 SetHandler 注册了 LoadStream 回调
// （+0x610c 非空），以 (dir, filename, s) 尾调用之并返回其 bool；
// 否则返回 false。ORIG 汇编：mov 0x610c(%eax),%eax → test/je → jmp *%eax。
extern "C" bool sub_lex_LoadStream(void* self, char* dir, char* filename, void* s)
    asm("_ZN13DNFLexWrapper10LoadStreamEPKcS1_PN6DNFLex13stream_data_tE");
extern "C" bool sub_lex_LoadStream(void* self, char* dir, char* filename, void* s)
{
    LexWrapperState* w = reinterpret_cast<LexWrapperState*>(self);
    if (w->loadStream_ == 0)
        return false;
    return w->loadStream_(dir, filename, s);
}

// setFileStreamData（ORIG 0x8acede0）：以 in|binary 打开 path 的 ifstream。
//   - path/s 任一为空 → 直接 false；
//   - 先 stream_data_t::clear(s)（stream/size/buffer 清零）；
//   - new ifstream(path, in|binary)；未打开（open 失败置 failbit）或 badbit
//     置位 → delete 流并返回 false（ORIG: is_open 检查 + (state & 1) 检查，
//     失败经 vtable 槽 +4 = deleting 析构 delete）；
//   - seekg(0, end) → tellg 取 size → seekg(0, beg)（ORIG 汇编 0x8acef30 起
//     第一次 seekdir=2=end，第二次 =0=beg；Ghidra 伪码误标两处 _S_beg，以
//     汇编为准）；
//   - size==0 → delete 流并委托 setStringStreamData(s, 0, 0)（空串流，
//     istrstream 空缓冲）；
//   - 否则 s->stream = ifs; s->size = size; 返回 true。
extern "C" bool sub_lex_setFileStreamData(void* self, void* s, char* path)
    asm("_ZN13DNFLexWrapper17setFileStreamDataEPN6DNFLex13stream_data_tEPKc");
extern "C" bool sub_lex_setFileStreamData(void* self, void* s, char* path)
{
    if (path == 0 || s == 0)
        return false;

    sub_DNFLex_stream_data_t_clear(s);

    std::ifstream* ifs = new std::ifstream(path, std::ios::in | std::ios::binary);
    if (!ifs->is_open() || ifs->bad())
    {
        delete ifs;
        return false;
    }

    ifs->seekg(0, std::ios::end);
    std::streampos size = ifs->tellg();
    ifs->seekg(0, std::ios::beg);

    if (size == 0)
    {
        delete ifs;
        return sub_lex_setStringStreamData(self, s, 0, 0);
    }

    StreamData* sd = reinterpret_cast<StreamData*>(s);
    sd->stream = ifs;
    sd->size = static_cast<int>(size);
    return true;
}
