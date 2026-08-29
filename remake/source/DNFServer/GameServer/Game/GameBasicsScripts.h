#ifndef DNF_GAME_BASICS_SCRIPTS_H_
#define DNF_GAME_BASICS_SCRIPTS_H_

#include "DBDispatcher.h"

class Stream;

class DB_UpdateAuraAvatarOption : public DBDispatcher
{
public:
    DB_UpdateAuraAvatarOption() {}   // ORIG C1 0x844fa26 (sz 0x4: base ctor+vptr only)
    static void makeRequest(int uid, unsigned int characNo, int idx, int value); // ORIG 0x0844f26
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844b000
};

class CGM_Manager
{
public:
    bool IsGm(unsigned int accId);
};

#endif
