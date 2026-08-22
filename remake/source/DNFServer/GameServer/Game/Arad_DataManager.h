// df_game_r Game/ ARAD::Arad_DataManager（批3：从 GameStubs.cpp 迁移）。
#ifndef DNF_ARAD_DATAMANAGER_H
#define DNF_ARAD_DATAMANAGER_H

#include <string>

class CUser;
class AradServerStateMessage;

namespace ARAD
{
class Arad_DataManager
{
public:
    void* findGameScript(const std::string& name);
};

template <class T>
class Singleton
{
public:
    static T* Get();
};

void notifyOpenMessageDialog(CUser* user, int a, int b,
                             AradServerStateMessage& msg);
}

#endif  // DNF_ARAD_DATAMANAGER_H