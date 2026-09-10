// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DNFLexWrapperStream/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "DNFLexWrapperStream/ss_vsprintf.inc"
                std::va_list ap);

template <unsigned int N>
#include "DNFLexWrapperStream/ss_sprintf.inc"
{
    std::va_list ap;
    va_start(ap, format);
    int n = ss_vsprintf(buffer, N, format, ap);
    va_end(ap);
    return n;
}

// getInputStream（ORIG 0x8ace900）：仅 NORMAL 分流到 DNFLex::GetInputStream，
// 取其返回流对象偏移 +0x8 的 istream*；其它 lexType 返回 0。
#include "DNFLexWrapperStream/getInputStream.inc"
{
    void* r = 0;
    if (lexType_ != LEX_TYPE_NORMAL)
        return r;
    return dnfLex_.GetInputStream()->s.stream;
}

// getInputStreamBuffer（ORIG 0x8ace970）：NORMAL 取 DNFLex stream_t +0x10，
// COMPILED 取 CompiledDNFLex stream_t +0x0c；其它返回 0。无空指针护栏。
#include "DNFLexWrapperStream/getInputStreamBuffer.inc"
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
#include "DNFLexWrapperStream/getInputStreamSize.inc"
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
// b28d-21 + E06 复测: NEAR 封顶 — 29=29 助记符序列全同，唯一残差为 lexType_
// 载入寄存器（ORIG `mov 0x14(%eax),%edx; test %edx,%edx` vs 4.4.x `%ecx`）。
// E06 实测矩阵：①本 TU 路由 c6446r -O2 → %ecx；②c6root 4.4.7 全旗标 -O2（无
// 路由改写）→ %ecx；③源码形穷举 `lexType_ != 0` / Yoda `NORMAL != lexType_` /
// 三目条件式（此处反劣为 DIFF，已回退）→ 均 %ecx。⇒ 4.4 分配器恒选 %ecx，
// 属 N7 跨版本寄存器角色差（P321-T5：寄存器角色不改类型/源码），无源码旋钮。
// ORIG 0x8ace8a0。
#include "DNFLexWrapperStream/getStreamName.inc"
{
    if (lexType_ != LEX_TYPE_NORMAL)
        return std::string("NONE");
    return dnfLex_.getStreamName();
}

// closeInputStream（ORIG 0x8acea70）：依次调用 DNFLex::CloseInputStream 与
// CompiledDNFLex::CloseInputStream。
#include "DNFLexWrapperStream/closeInputStream.inc"
{
    dnfLex_.CloseInputStream();
    compiledDnfLex_.CloseInputStream();
}

// setStringStreamData（ORIG 0x8ace9b0）：用 buffer/bufferSize 构造 istrstream
// 填充 stream_data_t；bufferSize==0 时先释放 buffer 再以空串+长度 1 构造
// （size=1，buffer 槽不写）。恒返回 true。
#include "DNFLexWrapperStream/setStringStreamData.inc"
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
// b28d-21 + E06 复测: NEAR 封顶 — 120=120 助记符序列全同，唯一实质残差为
// lexerError_（0x6108(%esi)）载入寄存器（ORIG `mov 0x6108(%esi),%ebx; test %ebx,
// %ebx` vs 4.4.x `%edx`）。E06 实测矩阵：①本 TU 路由 c6446r -O2 → %edx；
// ②c6root 4.4.7 全旗标 -O2（无路由改写）→ %edx。⇒ N7/P321-T5 跨版本寄存器
// 角色差，无源码旋钮（后续同址 `call *0x6108(%esi)` 两侧逐条一致，证明语义等价）。
// 另余 ORIG `nop; lea 0x0(%esi,%eiz,1),%esi` 与本方 `lea…; nop` 的对齐填充顺序差
// （§6.1 填充豁免）。ORIG 0x8aceaa0。
#include "DNFLexWrapperStream/switchInputStream.inc"
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
#include "DNFLexWrapperStream/switchChannelInputStream.inc"
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
