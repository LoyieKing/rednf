#ifndef DNF_SIGNAL_H_
#define DNF_SIGNAL_H_

class CApplication;

// CSignal：vptr@0 / app@4（8B）；vtable：handle / dtor / dtor
class CSignal
{
public:
    // ORIG：构造/析构均为头文件内联（weak COMDAT）。内联体使 new 表达式的
    // 清理分析可见“不抛异常”→ 不生成 delete 清理 landing pad（init_handler 对齐）。
    CSignal() { m_app = 0; }
    virtual void handle(int sig) = 0;
    virtual ~CSignal() {}
    void dump_core_file();
    void attachApp(CApplication* app);
    CApplication* m_app;  // +4
};

class CTerminateSig : public CSignal
{
public:
    // 空构造内联于头（复现 ORIG：构造器为 weak 符号且帧为 sub $0x18,%esp；
    // 若在 .cpp 定义，同 TU 可见 CSignal::CSignal 定义，帧会变成 sub $0x4,%esp）
    CTerminateSig() throw() {}
    virtual ~CTerminateSig() throw() {}
    virtual void handle(int sig);
};

class CSegmentationFaultSig : public CSignal
{
public:
    CSegmentationFaultSig() throw() {}
    virtual ~CSegmentationFaultSig() throw() {}
    virtual void handle(int sig);
};

class CSystemFailSig : public CSignal
{
public:
    CSystemFailSig() throw() {}
    virtual ~CSystemFailSig() throw() {}
    virtual void handle(int sig);
};

class CFloatingPointExceptSig : public CSignal
{
public:
    CFloatingPointExceptSig() throw() {}
    virtual ~CFloatingPointExceptSig() throw() {}
    virtual void handle(int sig);
};

#endif // DNF_SIGNAL_H_
