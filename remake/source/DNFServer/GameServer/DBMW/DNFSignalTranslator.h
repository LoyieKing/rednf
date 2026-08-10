#ifndef DBMW_DNFSIGNALTRANSLATOR_H_
#define DBMW_DNFSIGNALTRANSLATOR_H_

#include "DBMWCommon.h"


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

#endif  // DBMW_DNFSIGNALTRANSLATOR_H_
