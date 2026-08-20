// df_game_r Game/ ARAD 命名空间类实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "CUser.h"
#include <string>

class AradServerStateMessage
{
public:
    AradServerStateMessage();
};

class Arad_EventPeriodDataManager
{
public:
    bool isApplied(int a, int b) const;
};

namespace ARAD
{

class Arad_DataManager
{
public:
    void* findGameScript(const std::string& name);
};

void* Arad_DataManager::findGameScript(const std::string&)
{
    return 0;
}

template <class T>
class Singleton
{
public:
    static T* Get();
};

template <class T>
T* Singleton<T>::Get()
{
    static T instance;
    return &instance;
}

template class Singleton<Arad_DataManager>;

void notifyOpenMessageDialog(CUser*, int, int, AradServerStateMessage&)
{
}

}  // namespace ARAD

AradServerStateMessage::AradServerStateMessage()
{
}
