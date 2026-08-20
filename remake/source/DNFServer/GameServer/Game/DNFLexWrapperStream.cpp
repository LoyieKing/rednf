// ============================================================================
// DNFLexWrapper stream 访问方法（G4 脚本批次；ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLexWrapper.md）
// ----------------------------------------------------------------------------
// 本 TU 提供以下包装器方法（其余 DNFLexWrapper 方法分别由
// GameStubs_remaining.cpp、DNFLexWrapper.cpp 与 DNFLexWrapperLoadStream.cpp
// 提供，本 TU 不重复实现）：
//   getInputStream()        _ZN13DNFLexWrapper14getInputStreamEv      ORIG 0x8ace900
//   getInputStreamBuffer()  _ZN13DNFLexWrapper20getInputStreamBufferEv ORIG 0x8ace970
//   getInputStreamSize()    _ZN13DNFLexWrapper18getInputStreamSizeEv   ORIG 0x8ace930
//   getStreamName()         _ZN13DNFLexWrapper13getStreamNameEv        ORIG 0x8ace8a0
//   closeInputStream()      _ZN13DNFLexWrapper16closeInputStreamEv     ORIG 0x8acea70
//   setStringStreamData()   _ZN13DNFLexWrapper19setStringStreamDataEPN6DNFLex13stream_data_tEPcj
//                                                        ORIG 0x8ace9b0
//   switchInputStream()     _ZN13DNFLexWrapper17switchInputStreamEPKcS1_PN6DNFLex13stream_data_tE
//                                                        ORIG 0x8aceaa0
//   switchChannelInputStream() _ZN13DNFLexWrapper24switchChannelInputStreamEPKcPN6DNFLex13stream_data_tE
//                                                        ORIG 0x8acf060
// 另按 LexLoadStream 分工提供引擎符号 DNFLex::stream_data_t::clear
// （_ZN6DNFLex13stream_data_t5clearEv，ORIG 0x8acd140）。
//
// 字段布局依据 ORIG DNFLexWrapper::DNFLexWrapper（0x8aced60）：
//   +0x00 vptr   +0x04 lastToken_   +0x08 lastIntValue_
//   +0x0c lastFloatValue_           +0x10 lastStringValue_   +0x14 lexType_
//   （lexType_：0=NORMAL 文本流 / 1=COMPILED 编译态，见 getConnector 0x8ace3f0）
//   +0x18 dnfLex_（DNFLex 子对象）/+0x40bc compiledDnfLex_（CompiledDNFLex
//   子对象）/+0x6108 LexerError_ / +0x610c LoadStream_，总大小 0x6110。
//
// 这些方法按 ORIG 语义在 lexType_ 上分流并转发到底层词法引擎
// （DNFLex::GetInputStream / CompiledDNFLex::GetInputStream /
// DNFLex::getStreamName / DNFLex::CloseInputStream /
// CompiledDNFLex::CloseInputStream），引擎符号由词法引擎批次提供；
// 本 TU 仅按 ORIG ABI 声明并封装，不做固定返回值伪实现（与
// DNFLexWrapper.cpp 的 setStringTableFrom* 转发模式一致）。
//
// ORIG 汇编要点：
//   getInputStream      仅在 NORMAL 分流到 DNFLex::GetInputStream 并取
//                       返回 stream_t 偏移 +0x8 的 istream*。
//   getInputStreamBuffer NORMAL 取 DNFLex stream_t +0x10；COMPILED 取
//                       CompiledDNFLex stream_t +0x0c；其它返回 0。
//   getInputStreamSize   NORMAL 取 DNFLex stream_t +0x0c；COMPILED 取
//                       CompiledDNFLex stream_t +0x08；其它返回 -1。
//   getStreamName        tstring 返回（按 DNFLexWrapper.cpp
//                       getLastTokenDataAsString 的 by-value std::string
//                       惯例）：lexType_ != 0 时返回 "NONE"；NORMAL 时委托
//                       DNFLex::getStreamName。
//   closeInputStream     依次调用 DNFLex::CloseInputStream 与
//                       CompiledDNFLex::CloseInputStream。
// ============================================================================

#include <cstdio>
#include <strstream>
#include <string>

namespace
{
struct DNFLexStorageView { char opaque[0x40a4]; };
struct CompiledDNFLexStorageView { char opaque[0x604c]; };
struct LexWrapperState
{
    const void* vptr;
    int lastToken_;
    int lastIntValue_;
    float lastFloatValue_;
    char* lastStringValue_;
    int lexType_;
    DNFLexStorageView dnfLex_;
    CompiledDNFLexStorageView compiledDnfLex_;
    void (*lexerError_)(const char*);
    int (*loadStream_)(const char*, const char*, void*);
};
struct StreamDataView
{
    void* stream;
    int size;
    char* buffer;
};
enum { LEX_NORMAL = 0, LEX_COMPILED = 1 };


// ---- 底层词法引擎符号（由引擎批次提供；本 TU 仅按 ORIG ABI 声明并转发）----
// DNFLex::GetInputStream()                         _ZN6DNFLex14GetInputStreamEv
// CompiledDNFLex::GetInputStream()                 _ZN14CompiledDNFLex14GetInputStreamEv
// DNFLex::getStreamName()                          _ZN6DNFLex13getStreamNameEv
// DNFLex::CloseInputStream()                       _ZN6DNFLex16CloseInputStreamEv
// CompiledDNFLex::CloseInputStream()               _ZN14CompiledDNFLex16CloseInputStreamEv
extern "C" void* sub_DNFLex_GetInputStream(void* self)
    asm("_ZN6DNFLex14GetInputStreamEv");
extern "C" void* sub_CompiledDNFLex_GetInputStream(void* self)
    asm("_ZN14CompiledDNFLex14GetInputStreamEv");
extern "C" std::string sub_DNFLex_getStreamName(void* self)
    asm("_ZN6DNFLex13getStreamNameEv");
}  // namespace

// getInputStream（ORIG 0x8ace900）：仅 NORMAL 分流到 DNFLex::GetInputStream，
// 取其返回流对象偏移 +0x8 的 istream*；其它 lexType 返回 0。
extern "C" void* sub_lex_getInputStream(void* self)
    asm("_ZN13DNFLexWrapper14getInputStreamEv");
extern "C" void* sub_lex_getInputStream(void* self)
{
    LexWrapperState* w = reinterpret_cast<LexWrapperState*>(self);
    if (w->lexType_ != LEX_NORMAL)
        return 0;
    void* st = sub_DNFLex_GetInputStream(&w->dnfLex_);
    if (!st)
        return 0;
    return static_cast<StreamDataView*>(st)->stream;
}

// getInputStreamBuffer（ORIG 0x8ace970）：NORMAL 取 DNFLex stream_t +0x10，
// COMPILED 取 CompiledDNFLex stream_t +0x0c；其它返回 0。
extern "C" char* sub_lex_getInputStreamBuffer(void* self)
    asm("_ZN13DNFLexWrapper20getInputStreamBufferEv");
extern "C" char* sub_lex_getInputStreamBuffer(void* self)
{
    LexWrapperState* w = reinterpret_cast<LexWrapperState*>(self);
    if (w->lexType_ == LEX_NORMAL)
    {
        void* st = sub_DNFLex_GetInputStream(&w->dnfLex_);
        return st ? static_cast<StreamDataView*>(st)->buffer : 0;
    }
    if (w->lexType_ == LEX_COMPILED)
    {
        void* st = sub_CompiledDNFLex_GetInputStream(&w->compiledDnfLex_);
        return st ? static_cast<StreamDataView*>(st)->buffer : 0;
    }
    return 0;
}

extern "C" std::string sub_lex_getStreamName(void* self)
{
    LexWrapperState* w = reinterpret_cast<LexWrapperState*>(self);
    if (w->lexType_ != LEX_NORMAL)
        return std::string("NONE");
    return sub_DNFLex_getStreamName(&w->dnfLex_);
}

// toMbcs（_Z6toMbcsPKc，sync_script.cpp 提供）：TCHAR 转多字节，失败时
// 返回原指针。C++ 链接符号，不做 extern "C"。
const char* toMbcs(const char* src);

// stream_data_t 布局（与 DNFLexCore.cpp 一致）：+0 stream / +4 size / +8 buffer。
struct StreamDataState
{
    void*       stream;   // +0x00 istream*
    int         size;     // +0x04
    char*       buffer;   // +0x08
};

// setStringStreamData（ORIG 0x8ace9b0）：用 buffer/bufferSize 构造 istrstream
// 填充 stream_data_t；bufferSize==0 时先释放 buffer 再以空串+长度 1 构造
// （size=1，buffer 槽不写）。恒返回 true。
extern "C" int sub_lex_switchInputStream(void* self, const char* dir,
                                         const char* filename, void* s)
{
    LexWrapperState* w = reinterpret_cast<LexWrapperState*>(self);
    StreamDataState local;
    if (s == 0)
    {
        s = &local;
        sub_DNFLex_stream_data_t_clear(s);
        if (!sub_lex_LoadStream(self, dir, filename, s))
        {
            void (*err)(const char*) = w->lexerError_;
            if (err == 0)
                return 0;
            char buf[1024];
            snprintf(buf, sizeof(buf), "%s/%s Read failed!",
                     toMbcs(dir), toMbcs(filename));
            err(buf);
            return 0;
        }
    }
    StreamDataState* sd = reinterpret_cast<StreamDataState*>(s);
    if (sd->size > 1)
    {
        std::istream* is = reinterpret_cast<std::istream*>(sd->stream);
        short magic = 0;
        is->read(reinterpret_cast<char*>(&magic), 2);
        is->seekg(-2, std::ios::cur);
        if (magic == static_cast<short>(0xd0b0))
        {
            w->lexType_ = LEX_COMPILED;
            if (sd->buffer == 0)
            {
                char* buf = new char[sd->size];
                is->read(buf, sd->size);
                return sub_CompiledDNFLex_SwitchInputStream(
                    &w->compiledDnfLex_, dir, filename, buf,
                    static_cast<unsigned int>(sd->size));
            }
            int ret = sub_CompiledDNFLex_SwitchInputStream(
                &w->compiledDnfLex_, dir, filename, sd->buffer,
                static_cast<unsigned int>(sd->size));
            if (sd->stream != 0)
            {
                void (**vtable)(void*) =
                    *reinterpret_cast<void (***)(void*)>(sd->stream);
                vtable[1](sd->stream);
            }
            return ret;
        }
    }
    w->lexType_ = LEX_NORMAL;
    return sub_DNFLex_SwitchInputStream(&w->dnfLex_, dir, filename, s);
}