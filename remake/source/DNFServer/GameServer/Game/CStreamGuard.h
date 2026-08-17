#ifndef DNF_GAME_CSTREAMGUARD_H_
#define DNF_GAME_CSTREAMGUARD_H_

// ============================================================================
// df_game_r 还原 —— CStreamGuard（RAII，S1 组）
// 参照 docs/class_func_reports/CStreamGuard.md + df_game_r 反汇编。
// 布局：+0x00 Stream* m_stream | +0x04 bool m_bOwn（sizeof = 8）
// ORIG 语义要点：
//   - CStreamGuard(bool) 只写 m_bOwn（m_stream 不初始化）；
//   - operator->()/operator*() 返回 this（调用方随后在 CStreamGuard 上继续
//     operator<< / GetInBuffer 等成员调用，见 ORIG 0x80c8c4e/0x80c8c46）；
//   - operator<< 系列转发给 Stream::operator<< 并直接返回其结果。
// ============================================================================

#include "Stream.h"
#include "StreamPool.h"

class CStreamGuard
{
public:
    CStreamGuard(bool bOwn);             // 0x080de62c
    CStreamGuard(Stream* stream, bool bOwn);  // 0x080c8c26
    ~CStreamGuard();                     // 0x0861c8d2

    void Set(Stream* stream);            // 0x080de644
    Stream* Get();                       // 0x08100ffa
    CStreamGuard* operator->();          // 0x080c8c4e（返回 this）
    CStreamGuard* operator*();           // 0x080c8c46（返回 this）

    Stream& operator<<(bool b);          // 0x0844d430
    Stream& operator<<(char c);          // 0x08101004
    Stream& operator<<(short s);         // 0x0822ad66
    Stream& operator<<(int v);           // 0x080c8c56
    Stream& operator<<(long v);          // 0x080e2276
    Stream& operator<<(unsigned char c); // 0x0844d454
    Stream& operator<<(unsigned short s);// 0x0847dcfa
    Stream& operator<<(unsigned int v);  // 0x080e2292
    Stream& operator<<(unsigned long v); // 0x0822ada6
    Stream& operator<<(char* s);         // 0x0822ad8a

    int put_binary(void* src, int len);  // 0x080de652

    template <class T> T* GetInBuffer(); // 模板，按使用点实例化

    Stream* m_stream;   // +0x00
    bool m_bOwn;        // +0x04
};

template <class T>
T* CStreamGuard::GetInBuffer()
{
    return m_stream->GetInBuffer<T>();
}

#endif  // DNF_GAME_CSTREAMGUARD_H_
