#ifndef DBMW_DNFSIGNAL_H_
#define DBMW_DNFSIGNAL_H_

#include "DBMWCommon.h"


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

#endif  // DBMW_DNFSIGNAL_H_
