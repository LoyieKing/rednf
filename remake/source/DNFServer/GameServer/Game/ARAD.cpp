// df_game_r Game/ ARAD 命名空间类实现（批3：从 GameStubs.cpp 迁移，收拢到独立头文件）。
#include "AradServerStateMessage.h"
#include "Arad_EventPeriodDataManager.h"
#include "Arad_DataManager.h"
#include "CUser.h"

namespace ARAD
{
void* Arad_DataManager::findGameScript(const std::string&)
{
    return 0;
}

template <class T>
T* Singleton<T>::Get()
{
    static T instance;
    return &instance;
}

template class Singleton<Arad_DataManager>;

void notifyOpenMessageDialog(CUser*, int, int, AradServerStateMessage&)
{
    // TODO(G2)
}
}  // namespace ARAD