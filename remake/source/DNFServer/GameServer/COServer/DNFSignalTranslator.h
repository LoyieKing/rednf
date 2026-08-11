#ifndef DNF_SIGNAL_TRANSLATOR_H_
#define DNF_SIGNAL_TRANSLATOR_H_

#include "DNFSignal.h"

class CApplication;

// CSignalTranslator：handlers[0x20]@0（0x80）；sysfail@0x10 / segv@0x18 /
// user1@0x28 / user2@0x30 / terminate@0x3c
class CSignalTranslator
{
public:
    CSignalTranslator();
    ~CSignalTranslator();
    void clear();
    void init(CApplication* app);
    void init_signal();
    int init_handler(CApplication* app);
    bool regist_signal(int sig, void (*handler)(int));
    CSignal* getSignal(int sig) const;
    CSignal* m_handlers[0x20];  // +0
};

CSignalTranslator* CSignalTranslatorInstance();
void signal_handler(int sig);

#endif // DNF_SIGNAL_TRANSLATOR_H_
