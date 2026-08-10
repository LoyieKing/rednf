#ifndef DNF_SIGNAL_TRANSLATOR_H_
#define DNF_SIGNAL_TRANSLATOR_H_

class CApplication;
class CSignal;

// ---- CSignalTranslator ----
class CSignalTranslator
{
public:
    CSignalTranslator();
    ~CSignalTranslator();
    char regist_signal(int sig, void (*handler)(int));
    void clear();
    void init_signal();
    void init_handler(CApplication* app);
    void init(CApplication* app);
    CSignal* getSignal(int idx) const;
    CSignal* m_signals[0x1a];  // +0（0x68 字节）
};

CSignalTranslator* CSignalTranslatorInstance();
void signal_handler(int sig);

#endif  // DNF_SIGNAL_TRANSLATOR_H_
