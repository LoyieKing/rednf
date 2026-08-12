#ifndef DEBUGLOG_H_
#define DEBUGLOG_H_

#include "stdafx.h"
#include "ThreadLock.h"
#include <sys/stat.h>
#include <time.h>
#include <new>

class ITextOutputDevice
{
public:
    ITextOutputDevice()
    {
    }
    virtual void serialize(char* s) = 0;
    virtual void flush() = 0;
    void get_time(char* todaystr, char* timestr);

protected:
    char cur_date[20];
    char cur_time[20];
    char out_buf[0x19000];
};

class TextOutputDevice_FILE : public ITextOutputDevice
{
public:
    TextOutputDevice_FILE();
    ~TextOutputDevice_FILE();
    bool open(const TCHAR* s);
    void close();
    bool log_file_check();
    virtual void serialize(char* s);
    virtual void flush();

private:
    bool _reopen();

    FILE* fp_;
    TCHAR szFileName_[260];
    ThreadLock LockFile;
    TCHAR logdname[260];
    TCHAR refname[260];
    struct stat st;
    time_t now;
    struct tm* tm;
    struct tm buf;
};

class TextOutputDevice_stdout : public ITextOutputDevice
{
public:
    virtual void serialize(char* s);
    virtual void flush();
};

template <class T>
class TDebugTrace
{
public:
    TDebugTrace()
    {
        memset(&m_FormatBuf[0], 0, sizeof(m_FormatBuf));
        hexadecimal_ = false;
        pDevice_ = NULL;
        mPos = 0;
    }
    ~TDebugTrace()
    {
    }
    void setOutputDevice(ITextOutputDevice* pDevice);
    TDebugTrace& operator<<(const char* s);
    TDebugTrace& operator<<(char* s);
    TDebugTrace& operator<<(int n);
    TDebugTrace& operator<<(unsigned int n);
    TDebugTrace& operator<<(bool b);
    TDebugTrace& operator<<(TDebugTrace& (*pFunc)(TDebugTrace&));
    TDebugTrace* putText(char* s);
    TDebugTrace* putText(const char* s);
    TDebugTrace* putValue(int n);
    TDebugTrace* putValue(unsigned int n);
    TDebugTrace* flush();
    void Lock();
    void Unlock();

    template <class U>
    friend TDebugTrace<U>& endl(TDebugTrace<U>& trace);

protected:
    T m_FormatBuf[0x19000];
    bool hexadecimal_;
    ITextOutputDevice* pDevice_;
    int mPos;
    ThreadLock m_Lock;
};

template <class T>
TDebugTrace<T>& endl(TDebugTrace<T>& trace);

template <class T>
class TGlobalInstance
{
public:
    TGlobalInstance() : m_p(NULL)
    {
    }
    ~TGlobalInstance()
    {
        destroy();
    }
    T* get()
    {
        if (m_p == 0)
        {
            create();
        }
        return m_p;
    }
    void create()
    {
        if (m_p == 0)
        {
            if (m_p == 0)
            {
                try
                {
                    // ORIG 模板体为 value-initialization：new T()。
                    // stdout 无用户构造函数 -> operator new + memset（清零）+
                    // 隐式 C1（置 vptr），与 ORIG 二进制逐条一致；FILE 有用户
                    // 构造函数 -> operator new + C1（不零初始化），与 ORIG 一致。
                    m_p = new T();
                }
                catch (...)
                {
                    printf("cannot allocate memory in TGlobalInstance.! cannot continue");
                    exit(-1);
                }
            }
        }
        else
        {
            // ORIG: the first `m_p == 0` false-path jumps to a lone nop before
            // the epilogue (GCC 4.4 empty-block layout artifact). This compiler
            // merges both false exits, so force the same layout explicitly.
            __asm__ __volatile__("nop");
        }
    }
    void destroy()
    {
        if (m_p != NULL)
        {
            delete m_p;
            m_p = NULL;
        }
    }

private:
    T* m_p;
};

#endif // DEBUGLOG_H_
