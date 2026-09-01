#ifndef GAME_CWORLDMAP_H_
#define GAME_CWORLDMAP_H_
#define GAME_CWORLDMAP_DEFINED

// CWorldMap 布局依据 ORIG（load_list: new CWorldMap(0x40)；set_world_map 直访各偏移）：
//   +0x00 int   m_index              （get_index 读）
//   +0x04 byte  m_isHellDungeon      （IsInHellDungeon 读，set_world_map 写 = script+0x20）
//   +0x08 int   m_dungeonIndex       （set_world_map 置 0x7fffffff；hasDungeon 读）
//   +0x0c byte  m_hasDeathTower      （hasDeathTower 读，set_world_map 置 0）
//   +0x10 vector<int> m_dungeonList  （GetDungeonList 返回 &m_dungeonList；hasDungeon 遍历）
//   +0x1c vector<int> m_field1c
//   +0x28 vector<pair<int,int>> m_hellPassItemList （CheckHellDungeonFreepassItemHaveAndDel 遍历）
//   +0x34 vector<int> m_field34
// 总大小 0x40（64）字节。
#include <utility>
#include <vector>

struct WorldMapScript;   // 完整定义见 WorldMapScript.h（set_world_map 形参仅指针）
class CWorldMap
{
public:
    // ORIG W 0x83746ac _ZN9CWorldMap9get_indexEv（非 const）：mov (%eax),%eax 直返 m_index。
    // insert_world_map（0x08364694）call 消费；定义于 CWorldMap.cpp。
    int get_index();
    // ORIG W 0x85beda4 _ZN9CWorldMap14GetDungeonListEv（非 const）：lea +0x10 直返
    // &m_dungeonList。CParty::make_blood_dungeon_info（0x085b3e14 区）对其 begin()/end()
    // 遍历；定义于 CWorldMap.cpp。
    std::vector<int>& GetDungeonList();
    bool IsInHellDungeon() const;      // ORIG W 0x830e6ba
    bool hasDeathTower() const;        // ORIG W 0x822b520（const）
    // ORIG W 0x0822b514 _ZNK9CWorldMap18getDungeonMinLevelEv：返回 m_dungeonIndex（this+0x8）。
    // StartGame::check_error（0x081ca2xx 区）消费（DisPatcherWave1，定义待落地→U 引用合规）。
    int getDungeonMinLevel() const;
    // ORIG W 0x822b52c _ZNK9CWorldMap10hasDungeonEj：std::find(m_dungeonList, dungeonIdx)
    // != end()。SelectDungeon::process（0x81c86ab）以 (int)(short)MSG+0xd 作实参消费。
    bool hasDungeon(unsigned int dungeonIdx) const;
    CWorldMap();                                       // ORIG W 0x0837f6ea
    void set_world_map(WorldMapScript* script);        // ORIG T 0x0836421a（形参完整类型见 WorldMapScript.h）


    int m_index;                                          // +0x00
    bool m_isHellDungeon;                               // +0x04（ORIG IsInHellDungeon movzbl 按 bool 读取）
    int m_dungeonIndex;                                   // +0x08
    bool m_hasDeathTower;                               // +0x0c（ORIG hasDeathTower movzbl 按 bool 读取）
    std::vector<int> m_dungeonList;                       // +0x10（GetDungeonList 返回；
                                                          //   hasDungeon 遍历；
                                                          //   ORIG 缺失致 m_field1c 等偏移漂移，本批补回）
    bool IsInHellDungeon();      // ORIG W 0x830e6ba（非 const，_ZN9CWorldMap15IsInHellDungeonEv）
    std::vector<int> m_field1c;                           // +0x1c
    std::vector<std::pair<int, int> > m_hellPassItemList; // +0x28
    std::vector<int> m_field34;                           // +0x34
};

#endif
