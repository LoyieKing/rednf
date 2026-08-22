#ifndef DNFPROJECT_SOURCE_DNFSERVER_GAMESERVER_GAME_CSYNCSLANGFILTER_H
#define DNFPROJECT_SOURCE_DNFSERVER_GAMESERVER_GAME_CSYNCSLANGFILTER_H

#include <string>

// CSyncSlangFilter —— 聊天/名字敏感词过滤桥接（项目自造符号 _ZN16CSyncSlangFilter6FilterERKSs）
// GetInstance/HasSlang/HasSlangName/AddSlangList/AddSlangListName/FreeInstance 已在
// GlobalData.cpp 定义；此处仅声明，勿重复定义。Filter 在 CSyncSlangFilter.cpp 定义。
class CSyncSlangFilter
{
public:
    static CSyncSlangFilter* GetInstance();
    static void FreeInstance();
    void Filter(const std::string& str);
};

#endif