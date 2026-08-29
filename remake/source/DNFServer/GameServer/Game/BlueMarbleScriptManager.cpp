// df_game_r Game/ BlueMarbleScriptManager 专属 TU（从 GameStubs.cpp 拆出）。
// 布局依据 BlueMarbleScriptManager.h（→ PvPTypes.h）。
// 布局依据 PvPTypes.h（m_tiles +0xbc / m_maps +0xd4，ORIG 偏移已断言）。
// getMap/getTile 为 O(1) map 查找（ORIG 0x088d66b6/0x088d6646）。

#include <map>
#include <string>

struct STScriptFileList;   // +0x00 即 std::map<int,std::string>（权威布局见
                           // STScriptFileList.cpp；此处仅按 ORIG ABI 传址）。
// ORIG _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIii... 0x088bbd77 T
//（定义 CDataManagerScripts.cpp/STScriptFileList.cpp；DNFLexWrapper.h 未含声明）。
int loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                           const char* listPath, std::map<int, int>* unused);

#include "BlueMarbleScriptManager.h"
// ORIG 0x88d614a：ctor 顺序构造两个 STScriptFileList（+0x00/+0x18）、
// BlueMarbleInfoScript（+0x30）、m_tiles/m_maps 两 map（成员默认构造即等价）。
BlueMarbleScriptManager::BlueMarbleScriptManager()
    : m_mapFileList(), m_tileFileList(), m_info(), m_tiles(), m_maps()
{
}

// ORIG 0x88d622c T：清两个文件列表 map 与 m_tiles/m_maps（不删指针、不触
// m_info——与 ORIG clear 逐段一致）。
void BlueMarbleScriptManager::clear()
{
    m_mapFileList.clear();
    m_tileFileList.clear();
    m_tiles.clear();
    m_maps.clear();
}

// ORIG 0x88d60f2：懒加载单例（new + 静态指针保存；异常路径 delete）。
BlueMarbleScriptManager* BlueMarbleScriptManager::getInsance()
{
    static BlueMarbleScriptManager* s_instance;
    if (s_instance == 0)
        s_instance = new BlueMarbleScriptManager();
    return s_instance;
}

// ORIG 0x088d66b6 T：m_maps（this+0xd4）find，未命中返回 0。
BlueMarbleMapScript* BlueMarbleScriptManager::getMap(int idx)
{
    std::map<int, BlueMarbleMapScript*>::const_iterator it = m_maps.find(idx);
    return it != m_maps.end() ? it->second : 0;
}

// ORIG 0x088d6726 T：m_maps（this+0xd4）std::map::size。
int BlueMarbleScriptManager::getMapTotalCount()
{
    return static_cast<int>(m_maps.size());
}

// ORIG 0x088d6646 T：m_tiles（this+0xbc）find，未命中返回 0。
BlueMarbleTileScript* BlueMarbleScriptManager::getTile(int zone)
{
    std::map<int, BlueMarbleTileScript*>::const_iterator it = m_tiles.find(zone);
    return it != m_tiles.end() ? it->second : 0;
}

// ============================================================================
// init* 三连（CDataManager::init 调用链；报告 BlueMarbleScriptManager/*.md）
// ============================================================================

// ORIG T 0x088d6622：直接以 m_info（this+0x30）调 importScript(dir, file)，
// 返回值透传。
bool BlueMarbleScriptManager::initBlueMarbleInfo(const char* dir, const char* file)
{
    return m_info.importScript(dir, file) != 0;
}

// ORIG T 0x088d626e：loadRDARScriptListFile(&m_tileFileList, dir, list, 0)
// 失败返回 false；清空 m_tiles 后遍历文件列表，new BlueMarbleTileScript 并
// importScript(dir + 文件名)；成功入表，失败 delete。
bool BlueMarbleScriptManager::initTileList(const char* dir, const char* list)
{
    if (!loadRDARScriptListFile(
            reinterpret_cast<STScriptFileList*>(&m_tileFileList), dir, list, 0))
    {
        return false;
    }

    m_tiles.clear();
    for (std::map<int, std::string>::const_iterator it = m_tileFileList.begin();
         it != m_tileFileList.end(); ++it)
    {
        BlueMarbleTileScript* tile = new BlueMarbleTileScript;
        std::string path(dir);
        path += it->second;
        if (tile->importScript(path.c_str()) == 1)
        {
            m_tiles[it->first] = tile;
        }
        else
        {
            delete tile;
            tile = 0;
        }
    }
    return true;
}

// ORIG T 0x088d6438：同 initTileList，作用于 m_mapFileList/m_maps 与
// BlueMarbleMapScript（import 失败时 ORIG 先析构再 delete）。
bool BlueMarbleScriptManager::initMapList(const char* dir, const char* list)
{
    if (!loadRDARScriptListFile(
            reinterpret_cast<STScriptFileList*>(&m_mapFileList), dir, list, 0))
    {
        return false;
    }

    m_maps.clear();
    for (std::map<int, std::string>::const_iterator it = m_mapFileList.begin();
         it != m_mapFileList.end(); ++it)
    {
        BlueMarbleMapScript* map = new BlueMarbleMapScript;
        std::string path(dir);
        path += it->second;
        if (map->importScript(path.c_str()) == 1)
        {
            m_maps[it->first] = map;
        }
        else
        {
            delete map;
            map = 0;
        }
    }
    return true;
}