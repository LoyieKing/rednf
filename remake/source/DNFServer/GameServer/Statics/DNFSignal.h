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

#endif // DNF_SIGNAL_H_
