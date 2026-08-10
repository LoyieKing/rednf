#ifndef DNF_SIGNAL_H_
#define DNF_SIGNAL_H_

class CApplication;

// CSignal：vptr@0 / app@4（8B）；vtable：handle / dtor / dtor
class CSignal
{
public:
    CSignal();
    virtual void handle(int sig) = 0;
    virtual ~CSignal();
    void dump_core_file();
    void attachApp(CApplication* app);
    CApplication* m_app;  // +4
};

class CTerminateSig : public CSignal
{
public:
    // throw() 声明：TU 拆分后让编译器在 DNFSignalTranslator.cpp 里仍能看到
    // 空构造/析构不抛异常，从而不发射 EH cleanup（复现 ORIG 137 insns 机器码）
    CTerminateSig() throw();
    virtual ~CTerminateSig() throw();
    virtual void handle(int sig);
};

class CSegmentationFaultSig : public CSignal
{
public:
    CSegmentationFaultSig() throw();
    virtual ~CSegmentationFaultSig() throw();
    virtual void handle(int sig);
};

class CSystemFailSig : public CSignal
{
public:
    CSystemFailSig() throw();
    virtual ~CSystemFailSig() throw();
    virtual void handle(int sig);
};

class CFloatingPointExceptSig : public CSignal
{
public:
    CFloatingPointExceptSig() throw() {}
    virtual ~CFloatingPointExceptSig() throw();
    virtual void handle(int sig);
};

#endif // DNF_SIGNAL_H_
