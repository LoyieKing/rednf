#ifndef DNF_GAME_BASICS_SCRIPTS_H_
#define DNF_GAME_BASICS_SCRIPTS_H_

class DB_UpdateAuraAvatarOption
{
public:
    static void makeRequest(int uid, unsigned int characNo, int idx, int value);
};

class CGM_Manager
{
public:
    bool IsGm(unsigned int accId);
};

#endif
