#ifndef DNF_GAME_STREAM_H_
#define DNF_GAME_STREAM_H_

// ============================================================================
// df_game_r 还原 —— Stream / SmallStream / BigStream（S1 组）
// 参照 docs/class_func_reports/Stream.md；布局与 ORIG 一致：
//   +0x00 vptr | +0x04 m_size | +0x08 m_buffer | +0x0c m_inPtr |
//   +0x10 m_outPtr | +0x14 m_bigStream | +0x18 m_file | +0x1c m_line |
//   +0x20 m_poolType | +0x24 m_inUse（sizeof = 0x28）
// ============================================================================

class Stream
{
public:
    Stream();                 // 0x0861bc08
    Stream(int size);         // 0x0861bc72
    Stream(char* str);        // 0x0861bcda
    Stream(Stream& other);    // 0x0861bd14
    virtual ~Stream();        // 0x0861bd6c / 0x0861bdaa

    Stream& operator<<(bool b);          // 0x0861c01a
    Stream& operator<<(char c);          // 0x0861c0b2
    Stream& operator<<(short s);         // 0x0861c12e
    Stream& operator<<(int v);           // 0x0861c1bc
    Stream& operator<<(long v);          // 0x0861c242
    Stream& operator<<(char* s);         // 0x0861c2c8
    Stream& operator<<(unsigned char c);     // 0x0861c36a
    Stream& operator<<(unsigned short s);    // 0x0861c392
    Stream& operator<<(unsigned int v);      // 0x0861c3b8
    Stream& operator<<(unsigned long v);     // 0x0861c3d6

    bool operator>>(bool& b);            // 0x0861c47c
    bool operator>>(char& c);            // 0x0861c4f0
    bool operator>>(short& s);           // 0x0861c552
    bool operator>>(int& v);             // 0x0861c5c4
    bool operator>>(long& v);            // 0x0861c636
    bool operator>>(char* s);            // 0x0861c6a8
    bool operator>>(unsigned char& c);       // 0x0861c732
    bool operator>>(unsigned short& s);      // 0x0861c762
    bool operator>>(unsigned int& v);        // 0x0861c796
    bool operator>>(unsigned long& v);       // 0x0861c7c8

    int put_binary(void* src, int len);  // 0x0861c3f4
    bool get_binary(void* dst, int len); // 0x0861c7fa

    int size();                 // 0x0861bdc8
    int length();               // 0x0861bdd4
    int in_ptr();               // 0x0861be00
    int in_ptr(int n);          // 0x0861be0c
    int out_ptr();              // 0x0861be28
    int out_ptr(int n);         // 0x0861be34
    void reset();               // 0x0861be8a
    void set_big_stream(bool b);// 0x0861bec4
    bool is_big_stream();       // 0x0861bedc
    int resize(int n);          // 0x0861bee8
    bool enable_in(int n);      // 0x0861bf9e
    bool enable_out(int n);     // 0x0861bfdc
    char* get_buffer();         // 0x0861c8b0
    int str_len(char* s);       // 0x0861c86a

    // 头文件模板（ORIG 按使用点实例化，本批不产出符号）
    template <class T> T* GetInBuffer();
    template <class T> T* GetOutBuffer();

    // ---- 对象布局（+0x00 vptr）----
    int m_size;          // +0x04
    char* m_buffer;      // +0x08
    int m_inPtr;         // +0x0c
    int m_outPtr;        // +0x10
    bool m_bigStream;    // +0x14
    const char* m_file;  // +0x18（对象池 Acquire 记录文件名）
    int m_line;          // +0x1c（对象池 Acquire 记录行号）
    int m_poolType;      // +0x20（0=StreamPool 1=SmallStreamPool 2=BigStreamPool）
    bool m_inUse;        // +0x24（对象池占用标记）

private:
    char m_pad[3];       // +0x25，sizeof(Stream)=0x28
};

template <class T>
T* Stream::GetInBuffer()
{
    while (!enable_in((int)sizeof(T)))
    {
        resize(size() * 2);
    }
    char* p = (char*)((unsigned long)m_buffer + (unsigned long)m_inPtr);
    in_ptr((int)sizeof(T));
    return (T*)p;
}

template <class T>
T* Stream::GetOutBuffer()
{
    if (!enable_out((int)sizeof(T)))
    {
        return 0;
    }
    char* p = m_buffer + out_ptr();
    out_ptr((int)sizeof(T));
    return (T*)p;
}

class SmallStream : public Stream
{
public:
    SmallStream();            // 0x082a3d38（Stream(0x1000)）
    virtual ~SmallStream();   // 0x082ad248 / 0x082ad278
};

class BigStream : public Stream
{
public:
    BigStream();              // 0x082a3d5c（Stream(0x40000)）
    virtual ~BigStream();     // 0x082ad97e / 0x082ad9ae
};

#endif  // DNF_GAME_STREAM_H_
