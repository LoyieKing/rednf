#ifndef DNF_SIGNAL_H_
#define DNF_SIGNAL_H_

class CApplication;

// ---- CSignal ----
class CSignal
{
public:
    CSignal();
    virtual void handle(int sig) = 0;
    virtual ~CSignal();
    void attachApp(CApplication* app);
    void dump_core_file();
    CApplication* m_app;  // +4
};

#endif  // DNF_SIGNAL_H_
