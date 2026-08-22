#ifndef DNF_GAME_NeedMaterialDataManager_H_
#define DNF_GAME_NeedMaterialDataManager_H_

// 迁移自 GameStubs.cpp NeedMaterialDataManager 桩（batch-6）。
// 布局按 ORIG ctor (0x081b5a3c) / getNeedMaterial(CItem*) (0x081b5b70) 推导：
//   +0x00 std::map<int,std::pair<int,int>> m_map    （ctor 先 map C1Ev；0x18 字节）
//   +0x18 std::pair<int,int> m_default              （ctor 后 pair{-1,0}）
// ORIG 符号（T）：
//   _ZN23NeedMaterialDataManagerC1Ev                (0x081b5a3c)
//   _ZN23NeedMaterialDataManager11getInstanceEv     (0x081b5ab2)
//   _ZNK23NeedMaterialDataManager15getNeedMaterialEPK5CItem (0x081b5b70)

#include <map>
#include <utility>

class CItem;

class NeedMaterialDataManager
{
public:
    static NeedMaterialDataManager* getInstance();
    CItem* getNeedMaterial(const CItem* item) const;

private:
    NeedMaterialDataManager();
    ~NeedMaterialDataManager();

    std::map<int, std::pair<int, int> > m_map; // +0x00（0x18 字节）
    std::pair<int, int> m_default;             // +0x18
};

#endif // DNF_GAME_NeedMaterialDataManager_H_