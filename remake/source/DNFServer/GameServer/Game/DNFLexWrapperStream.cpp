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

#include <cstdarg>
#include <cstdio>
#include <new>
#include <strstream>
#include <string>

#include "DNFLexWrapper.h"

const char* toMbcs(const char* src);

// ORIG 变参格式化助手：弱模板 _Z10ss_sprintfILj1024EEiRAT__cPKcz @0x8acf0e0
// （紧接本 TU switchChannelInputStream 之后，自然宿主即本文件）。ORIG 体为
// ss_vsprintf(buffer, N, format, ap)；ss_vsprintf @0x8ad457c 的语义：
// n=vsnprintf(buf,size,fmt,ap)；size>n 返回 n，否则清 buf[0] 返回 0（截断
// 报错形）。声明不定义，宿主属字符串工具批次。
int ss_vsprintf(char* buffer, unsigned int size, const char* format,
                std::va_list ap);

template <unsigned int N>
int ss_sprintf(char (&buffer)[N], const char* format, ...)
{
    std::va_list ap;
    va_start(ap, format);
    int n = ss_vsprintf(buffer, N, format, ap);
    va_end(ap);
    return n;
}

// getInputStream（ORIG 0x8ace900）：仅 NORMAL 分流到 DNFLex::GetInputStream，
// 取其返回流对象偏移 +0x8 的 istream*；其它 lexType 返回 0。
void* DNFLexWrapper::getInputStream()
{
    void* r = 0;
    if (lexType_ != LEX_TYPE_NORMAL)
        return r;
    return dnfLex_.GetInputStream()->s.stream;
}

// getInputStreamBuffer（ORIG 0x8ace970）：NORMAL 取 DNFLex stream_t +0x10，
// COMPILED 取 CompiledDNFLex stream_t +0x0c；其它返回 0。无空指针护栏。
char* DNFLexWrapper::getInputStreamBuffer()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.GetInputStream()->s.buffer;
    char* r = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return compiledDnfLex_.GetInputStream()->buffer;
    return r;
}

// getInputStreamSize（ORIG 0x8ace930）：NORMAL 取 DNFLex stream_t +0x0c；
// COMPILED 取 CompiledDNFLex stream_t +0x08；其它返回 -1。无空指针护栏。
int DNFLexWrapper::getInputStreamSize()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.GetInputStream()->s.size;
    int r = -1;
    if (lexType_ == LEX_TYPE_COMPILED)
        return (int)compiledDnfLex_.GetInputStream()->size;
    return r;
}

// getStreamName（ORIG 0x8ace8a0）：tstring 返回（按 DNFLexWrapper.cpp
// getLastTokenDataAsString 的 by-value std::string 惯例）：lexType_ != 0 时
// 返回 "NONE"；NORMAL 时委托 DNFLex::getStreamName。
// b28d-21: NEAR 封顶 — 29=29 助记符序列全同，唯一残差为 lexType_ 载入寄存器
// （ORIG `mov 0x14(%eax),%edx; test %edx,%edx` vs 4.4.6 `%ecx`）——N7 跨版本
// 寄存器角色差（本 TU 路由 c6446r -O2，P230 补：N7 判词限定为「4.4.7 恒 %eax」，
// 本件为 -O2 分配器产物，无源码旋钮）。ORIG 0x8ace8a0。
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
    static const char* kEmptyBuf = "";
    if (bufferSize)
    {
        std::istrstream* is = new std::istrstream(buffer, (int)bufferSize);
        s->stream = is;
        s->size = (int)bufferSize;
        s->buffer = buffer;
    }
    else
    {
        const char* e = kEmptyBuf;
        ::operator delete(buffer);
        std::istrstream* is = new std::istrstream(e, 1);
        s->stream = is;
        s->size = 1;
    }
    return true;
}

// switchInputStream（ORIG 0x8aceaa0）：按流内容魔数判断编译态/文本态：
//   - s==0 时先 clear 局部 stream_data_t 并 LoadStream；失败经 lexerError_
//     回调报错返回 0；
//   - size>1 时读 2 字节魔数 0xd0b0 → COMPILED：转发 CompiledDNFLex::
//     SwitchInputStream（buffer 为读取区或新分配拷贝；若用 sd->stream 读则
//     完成后释放 istream）；否则 NORMAL：转发 DNFLex::SwitchInputStream。
// b28d-21: NEAR 封顶 — 120=120 助记符序列全同，唯一残差为 lexerError_
// （0x6108(%esi)）载入寄存器（ORIG `%ebx` vs 4.4.6 `%edx`）——N7 跨版本寄存器
// 角色差（-O2 分配器产物）。ORIG 0x8aceaa0。
bool DNFLexWrapper::switchInputStream(const char* dir, const char* filename,
                                      DNFLex::stream_data_t* s)
{
    char errbuf[0x400];
    DNFLex::stream_data_t local;
    short magic;

    if (s == 0)
    {
        s = &local;
        s->clear();
        if (!LoadStream(dir, filename, s))
        {
            if (lexerError_ == 0)
                return false;
            ss_sprintf(errbuf, "%s/%s Read failed!",
                       toMbcs(dir), toMbcs(filename));
            reinterpret_cast<void (*)(const char*)>(lexerError_)(errbuf);
            return false;
        }
    }
    if (1 < (unsigned)s->size)
    {
        reinterpret_cast<std::istream*>(s->stream)->read(
            reinterpret_cast<char*>(&magic), 2);
        reinterpret_cast<std::istream*>(s->stream)->seekg(-2, std::ios::cur);
        if (magic == static_cast<short>(0xd0b0))
        {
            char* pc = s->buffer;
            lexType_ = LEX_TYPE_COMPILED;
            if (pc == 0)
            {
                pc = new char[s->size];
                reinterpret_cast<std::istream*>(s->stream)->read(pc, s->size);
                return compiledDnfLex_.SwitchInputStream(
                    dir, filename, pc, static_cast<unsigned int>(s->size));
            }
            bool ret = compiledDnfLex_.SwitchInputStream(
                dir, filename, pc, static_cast<unsigned int>(s->size));
            if (s->stream != 0)
            {
                void (**vtable)(void*) =
                    *reinterpret_cast<void (***)(void*)>(s->stream);
                vtable[1](s->stream);
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
