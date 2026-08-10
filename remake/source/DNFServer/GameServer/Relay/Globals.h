#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdio.h>

// TGlobalInstance<T>：仅持指针（4B），create/destroy 管理堆实例
template <class T>
class TGlobalInstance
{
public:
    TGlobalInstance()
    {
        m_p = 0;
    }
    ~TGlobalInstance()
    {
        destroy();
    }
    void create()
    {
        if (m_p != 0)
        {
            return;
        }
        if (m_p == 0)
        {
            try
            {
                m_p = new T;
            }
            catch (...)
            {
                printf("cannot allocate memory in TGlobalInstance.! cannot continue");
                exit(-1);
            }
        }
    }
    void destroy()
    {
        if (m_p != 0)
        {
            delete m_p;
            m_p = 0;
        }
    }
    T* inst_ptr()
    {
        return m_p;
    }
    const T* inst_ptr() const
    {
        return m_p;
    }

private:
    T* m_p;
};

// ITextOutputDevice（抽象基类，vptr）
class ITextOutputDevice
{
public:
    ITextOutputDevice()
    {
    }
    virtual void serialize(char c) = 0;
    virtual void serialize(char* str) = 0;
    virtual void serialize(char const* str) = 0;
    virtual void serialize(wchar_t c) = 0;
    virtual void serialize(wchar_t* str) = 0;
    virtual void serialize(wchar_t const* str) = 0;
};

// TextOutputDevice_FILE：vptr@0 / m_fp@4 / m_filename[0x104]@8，总 0x10c
class TextOutputDevice_FILE : public ITextOutputDevice
{
public:
    TextOutputDevice_FILE();
    ~TextOutputDevice_FILE();
    virtual void serialize(char c);
    virtual void serialize(char* str);
    virtual void serialize(char const* str);
    virtual void serialize(wchar_t c);
    virtual void serialize(wchar_t* str);
    virtual void serialize(wchar_t const* str);
    void _reopen();
    void close();
    void flush();

private:
    FILE* m_fp;
    char m_filename[0x104];
};

// TDebugTrace<char>：0x408（ORIG ctor 只清 byte@0、byte@0x400、dword@0x404）
template <class T>
class TDebugTrace
{
public:
    TDebugTrace()
    {
        m_buf[0] = 0;
        m_flag = 0;
        m_len = 0;
    }
    ~TDebugTrace()
    {
    }

private:
    char m_buf[0x400];   // @0
    char m_flag;         // @0x400
    char m_pad[3];       // @0x401..0x403
    int m_len;           // @0x404
};


// ---- 日志全局对象与 create/destroy（RelayServiceApp 命名空间）----
namespace RelayServiceApp
{
void createFileLogInfo();
void createFileLogWarn();
void createFileLogError();
void createFileLogCri();
void createLogInfo();
void createLogCri();
void createLogWarn();
void createLogError();
void destroyFileLogInfo();
void destroyFileLogWarn();
void destroyFileLogError();
void destroyFileLogCri();
void destroyLogInfo();
void destroyLogCri();
void destroyLogWarn();
void destroyLogError();

extern TGlobalInstance<TextOutputDevice_FILE> g_FileLogInfo;
extern TGlobalInstance<TextOutputDevice_FILE> g_FileLogWarn;
extern TGlobalInstance<TextOutputDevice_FILE> g_FileLogError;
extern TGlobalInstance<TextOutputDevice_FILE> g_FileLogCri;
extern TGlobalInstance<TDebugTrace<char> > g_LogInfo;
extern TGlobalInstance<TDebugTrace<char> > g_LogCri;
extern TGlobalInstance<TDebugTrace<char> > g_LogWarn;
extern TGlobalInstance<TDebugTrace<char> > g_LogError;
} // namespace RelayServiceApp

#endif // GLOBALS_H_
