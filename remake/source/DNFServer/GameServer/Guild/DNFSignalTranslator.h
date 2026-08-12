#ifndef DNFSIGNALTRANSLATOR_H_
#define DNFSIGNALTRANSLATOR_H_


#include "PacketHeader.h"

class CApplication;
class CSignal;
class CSignalTranslator;

// from GuildSignal.h
class CSignalTranslator
{
public:
    CSignalTranslator();
    ~CSignalTranslator();
    void clear();
    void init(CApplication* app);
    void init_signal();
    void init_handler(CApplication* app);
    bool regist_signal(int sig, void (*handler)(int));
    int getSignal(int sig) const;
    CSignal* m_handlers[0x20];  // +0
};

CSignalTranslator* CSignalTranslatorInstance();
void signal_handler(int sig);

#endif
