#ifndef DNFPROJECT_SOURCE_DNFSERVER_GAMESERVER_GAME_CSYNCSLANGFILTER_H
#define DNFPROJECT_SOURCE_DNFSERVER_GAMESERVER_GAME_CSYNCSLANGFILTER_H

#include <string>

#include "GameTypes.h"

class SlangNode;

// ORIG 布局：+0 IMPL*（脏话树）、+4 IMPL*（名字树）、+8 rwLock(0x20)，sizeof 0x28。
// IMPL 仅 4 字节，持有 SlangNode* 根。静态成员 instance 对应 ORIG .bss。
class CSyncSlangFilter
{
public:
    struct IMPL
    {
        SlangNode* root;
        IMPL();
        ~IMPL();
    };

    static CSyncSlangFilter* instance;
    static CSyncSlangFilter* GetInstance();
    static void FreeInstance();

    bool AddSlangList();
    bool AddSlangListName();
    void AddSlang(const std::string& str);
    void AddSlangName(const std::string& str);
    bool HasSlang(const std::string& name);
    bool HasSlangName(const std::string& name);
    int match(const std::string& name) const;
    int MatchName(const std::string& name) const;
    std::string Filter(const std::string& str);

private:
    CSyncSlangFilter();
    ~CSyncSlangFilter();

    IMPL* impl_;
    IMPL* implName_;
    rwLock lock_;
};

#endif
