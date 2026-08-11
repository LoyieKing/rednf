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
    // ORIG：基类构造函数内联定义于头文件（weak 符号 C1/C2 同址）。
    // 内联定义使派生类隐式构造函数的基类子对象调用带 EH region，
    // 栈帧预留 0x18（与 ORIG TextOutputDevice_stdout C1 逐字节一致）。
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
        memset(&m_FormatBuf[0], 0x20, sizeof(m_FormatBuf));
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
        // ORIG 模板体为两次独立检查（if (m_p != 0) return; if (m_p == 0) {...}），
        // 与 `m_p == 0 && m_p == 0` 语义等价，但生成 ORIG 的 nop 落地形态：
        // 第一次检查假分支落在尾声前 nop，第二次检查与正常出口直落尾声。
        if (m_p != 0)
        {
            return;
        }
        if (m_p == 0)
        {
            try
            {
                // ORIG 模板体为 value-initialization：new T()
                // （stdout 无用户构造函数 -> GCC 生成 operator new + memset + ctor，
                //   FILE 有用户构造函数 -> operator new + ctor，与 ORIG 二进制一致）
                m_p = new T();
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
