#ifndef DNFSIGNAL_H_
#define DNFSIGNAL_H_


#include "PacketHeader.h"

class CApplication;
class CSignal;

// from GuildSignal.h
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

#endif
