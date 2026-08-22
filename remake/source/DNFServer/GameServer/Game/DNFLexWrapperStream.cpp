// ============================================================================
// DNFLexWrapper stream 访问方法（G4 脚本批次；ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLexWrapper.md）
// ----------------------------------------------------------------------------
// 本 TU 提供以下包装器方法（其余 DNFLexWrapper 方法分别由 DNFLexWrapper.cpp、
// DNFLexWrapperLoadStream.cpp 提供）：
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
//
// 这些方法按 ORIG 语义在 lexType_ 上分流并转发到底层词法引擎
// （DNFLex::GetInputStream / CompiledDNFLex::GetInputStream /
// DNFLex::getStreamName / DNFLex::CloseInputStream /
// CompiledDNFLex::CloseInputStream），引擎符号由词法引擎批次提供；
// 本 TU 仅按 ORIG ABI 声明并封装，不做固定返回值伪实现。
// ============================================================================

#include <cstdio>
#include <strstream>
#include <string>

#include "DNFLexWrapper.h"

// toMbcs（_Z6toMbcsPKc，sync_script.cpp 提供）：TCHAR 转多字节，失败时
// 返回原指针。
const char* toMbcs(const char* src);

// getInputStream（ORIG 0x8ace900）：仅 NORMAL 分流到 DNFLex::GetInputStream，
// 取其返回流对象偏移 +0x8 的 istream*；其它 lexType 返回 0。
void* DNFLexWrapper::getInputStream()
{
    if (lexType_ != LEX_TYPE_NORMAL)
        return 0;
    DNFLex::stream_t* st = dnfLex_.GetInputStream();
    if (!st)
        return 0;
    return st->s.stream;
}

// getInputStreamBuffer（ORIG 0x8ace970）：NORMAL 取 DNFLex stream_t +0x10，
// COMPILED 取 CompiledDNFLex stream_t +0x0c；其它返回 0。
char* DNFLexWrapper::getInputStreamBuffer()
{
    if (lexType_ == LEX_TYPE_NORMAL)
    {
        DNFLex::stream_t* st = dnfLex_.GetInputStream();
        return st ? st->s.buffer : 0;
    }
    if (lexType_ == LEX_TYPE_COMPILED)
    {
        CompiledDNFLex::stream_t* st = compiledDnfLex_.GetInputStream();
        return st ? st->buffer : 0;
    }
    return 0;
}

// getInputStreamSize（ORIG 0x8ace930）：NORMAL 取 DNFLex stream_t +0x0c；
// COMPILED 取 CompiledDNFLex stream_t +0x08；其它返回 -1。
int DNFLexWrapper::getInputStreamSize()
{
    if (lexType_ == LEX_TYPE_NORMAL)
    {
        DNFLex::stream_t* st = dnfLex_.GetInputStream();
        return st ? st->s.size : 0;
    }
    if (lexType_ == LEX_TYPE_COMPILED)
    {
        CompiledDNFLex::stream_t* st = compiledDnfLex_.GetInputStream();
        return st ? static_cast<int>(st->size) : 0;
    }
    return -1;
}

// getStreamName（ORIG 0x8ace8a0）：tstring 返回（按 DNFLexWrapper.cpp
// getLastTokenDataAsString 的 by-value std::string 惯例）：lexType_ != 0 时
// 返回 "NONE"；NORMAL 时委托 DNFLex::getStreamName。
std::string DNFLexWrapper::getStreamName()
{
    if (lexType_ != LEX_TYPE_NORMAL)
        return std::string("NONE");
    return dnfLex_.getStreamName();
}

// closeInputStream（ORIG 0x8acea70）：依次调用 DNFLex::CloseInputStream 与
// CompiledDNFLex::CloseInputStream。
void DNFLexWrapper::closeInputStream()
{
    dnfLex_.CloseInputStream();
    compiledDnfLex_.CloseInputStream();
}

// setStringStreamData（ORIG 0x8ace9b0）：用 buffer/bufferSize 构造 istrstream
// 填充 stream_data_t；bufferSize==0 时先释放 buffer 再以空串+长度 1 构造
// （size=1，buffer 槽不写）。恒返回 true。
bool DNFLexWrapper::setStringStreamData(DNFLex::stream_data_t* s, char* buffer,
                                        unsigned int bufferSize)
{
    if (bufferSize == 0)
    {
        ::operator delete(buffer);
        std::istrstream* is = new std::istrstream("", 1);
        s->stream = is;
        s->size = 1;
    }
    else
    {
        std::istrstream* is = new std::istrstream(buffer, bufferSize);
        s->stream = is;
        s->size = static_cast<int>(bufferSize);
        s->buffer = buffer;
    }
    return true;
}

// switchInputStream（ORIG 0x8aceaa0）：按流内容魔数判断编译态/文本态：
//   - s==0 时先 clear 局部 stream_data_t 并 LoadStream；失败经 lexerError_
//     回调报错返回 0；
//   - size>1 时读 2 字节魔数 0xd0b0 → COMPILED：转发 CompiledDNFLex::
//     SwitchInputStream（buffer 为读取区或新分配拷贝；若用 sd->stream 读则
//     完成后释放 istream）；否则 NORMAL：转发 DNFLex::SwitchInputStream。
bool DNFLexWrapper::switchInputStream(const char* dir, const char* filename,
                                      DNFLex::stream_data_t* s)
{
    DNFLex::stream_data_t local;
    if (s == 0)
    {
        s = &local;
        s->clear();
        if (!LoadStream(dir, filename, s))
        {
            void (*err)(const char*) = reinterpret_cast<void (*)(const char*)>(lexerError_);
            if (err == 0)
                return false;
            char buf[1024];
            std::snprintf(buf, sizeof(buf), "%s/%s Read failed!",
                          toMbcs(dir), toMbcs(filename));
            err(buf);
            return false;
        }
    }
    DNFLex::stream_data_t* sd = s;
    if (sd->size > 1)
    {
        std::istream* is = reinterpret_cast<std::istream*>(sd->stream);
        short magic = 0;
        is->read(reinterpret_cast<char*>(&magic), 2);
        is->seekg(-2, std::ios::cur);
        if (magic == static_cast<short>(0xd0b0))
        {
            lexType_ = LEX_TYPE_COMPILED;
            if (sd->buffer == 0)
            {
                char* buf = new char[sd->size];
                is->read(buf, sd->size);
                return compiledDnfLex_.SwitchInputStream(
                    dir, filename, buf, static_cast<unsigned int>(sd->size));
            }
            bool ret = compiledDnfLex_.SwitchInputStream(
                dir, filename, sd->buffer, static_cast<unsigned int>(sd->size));
            if (sd->stream != 0)
            {
                void (**vtable)(void*) =
                    *reinterpret_cast<void (***)(void*)>(sd->stream);
                vtable[1](sd->stream);
            }
            return ret;
        }
    }
    lexType_ = LEX_TYPE_NORMAL;
    return dnfLex_.SwitchInputStream(dir, filename, s);
}

// switchChannelInputStream（ORIG 0x8acf060）：s==0 时先 clear 局部
// stream_data_t 并 setFileStreamData；置 lexType_=NORMAL 后转发
// DNFLex::SwitchInputStream("", filename, s)。
bool DNFLexWrapper::switchChannelInputStream(const char* filename,
                                             DNFLex::stream_data_t* s)
{
    DNFLex::stream_data_t local;
    if (s == 0)
    {
        s = &local;
        s->clear();
        if (!setFileStreamData(s, filename))
            return false;
    }
    lexType_ = LEX_TYPE_NORMAL;
    return dnfLex_.SwitchInputStream("", filename, s);
}
