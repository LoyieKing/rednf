// df_game_r Game/ BlueMarbleScriptManager 专属 TU（从 GameStubs.cpp 拆出）。
// 布局依据 BlueMarbleScriptManager.h（→ PvPTypes.h）。
// 布局依据 PvPTypes.h（m_tiles +0xbc / m_maps +0xd4，ORIG 偏移已断言）。
// getMap/getTile 为 O(1) map 查找（ORIG 0x088d66b6/0x088d6646）。

#include <map>
#include <string>

#include "BlueMarbleScriptManager.h"

// ORIG _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIii... 0x088bbd77 T
//（定义 CDataManagerScripts.cpp/STScriptFileList.cpp；DNFLexWrapper.h 未含声明）。
// b28d-22/168a①：ORIG initTileList/initMapList 调用点 `call; xor $1,%eax; test %al,%al`
// ⇒ 本 TU 按 bool 视图声明（返回类型不入 mangled；定义处恒 0/1，ABI 兼容）。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* unused);

// BlueMarbleInfoScript（+0x30）、m_tiles/m_maps 两 map（成员默认构造即等价）；
// 全部成员构造后调用 clear()（0x88d622c，清两文件列表与两 map）。
BlueMarbleScriptManager::BlueMarbleScriptManager()
    : m_mapFileList(), m_tileFileList(), m_info(), m_tiles(), m_maps()
{
    clear();
}

// ORIG 0x88d622c T：清两个文件列表 map 与 m_tiles/m_maps（不删指针、不触
// m_info——与 ORIG clear 逐段一致）。
void BlueMarbleScriptManager::clear()
{
    m_mapFileList.Clear();     // ORIG STScriptFileList::Clear
    m_tileFileList.Clear();
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
    std::map<int, BlueMarbleMapScript*>::iterator it = m_maps.find(idx);
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
    std::map<int, BlueMarbleTileScript*>::iterator it = m_tiles.find(zone);
    return it != m_tiles.end() ? it->second : 0;
}

// ============================================================================
// init* 三连（CDataManager::init 调用链；报告 BlueMarbleScriptManager/*.md）
// ============================================================================

// ORIG T 0x088d6622：直接以 m_info（this+0x30）调 importScript(dir, file)，
// 返回值透传。
// [b24F] DONE：返回类型 bool→int（ORIG call 后直接 leave;ret 透传，无 test;setne）。
int BlueMarbleScriptManager::initBlueMarbleInfo(const char* dir, const char* file)
{
    return m_info.importScript(dir, file);
}

// ORIG T 0x088d626e：loadRDARScriptListFile(&m_tileFileList, dir, list, 0)
// 失败返回 false；清空 m_tiles 后遍历文件列表，new BlueMarbleTileScript 并
// importScript(dir + 文件名)；成功入表，失败 delete。
bool BlueMarbleScriptManager::initTileList(const char* dir, const char* list)
{
    if (!loadRDARScriptListFile(&m_tileFileList, dir, list, 0))
    {
        return false;
    }

    m_tiles.clear();
    // b28d-96/270：ORIG 0x088d626e 用非 const _Rb_tree_iterator（ptEv/ppEv/ne），
    // const_iterator 会多出转换构造 + const ptEv ⇒ 改非 const。
    for (std::map<int, std::string>::iterator it = m_tileFileList.m_fileMap.begin();
         it != m_tileFileList.m_fileMap.end(); ++it)
    {
        BlueMarbleTileScript* tile = new BlueMarbleTileScript;
        // b28d-183 反例（ORIG 有槽）：ORIG 0x088d62f3 缓存 it->first 到 -0x1c 后再用。
        const int key = it->first;
        std::string path(dir);
        path += it->second;
        // b28d-59/168a①：ORIG 失败分支直落（`xor $1; test; je <成功>`）⇒ 否定早返形。
        if (!tile->importScript(path.c_str()))
        {
            delete tile;
            tile = 0;
        }
        else
        {
            m_tiles[key] = tile;
        }
    }
    return true;
}

// ORIG T 0x088d6438：同 initTileList，作用于 m_mapFileList/m_maps 与
// BlueMarbleMapScript（import 失败时 ORIG 先析构再 delete）。
bool BlueMarbleScriptManager::initMapList(const char* dir, const char* list)
{
    if (!loadRDARScriptListFile(&m_mapFileList, dir, list, 0))
    {
        return false;
    }

    m_maps.clear();
    // b28d-96/270：同 initTileList——ORIG 非 const 迭代器。
    for (std::map<int, std::string>::iterator it = m_mapFileList.m_fileMap.begin();
         it != m_mapFileList.m_fileMap.end(); ++it)
    {
        BlueMarbleMapScript* map = new BlueMarbleMapScript;
        const int key = it->first;
        std::string path(dir);
        path += it->second;
        // b28d-59/168a①：同 initTileList（否定早返形）。
        if (!map->importScript(path.c_str()))
        {
            delete map;
            map = 0;
        }
        else
        {
            m_maps[key] = map;
        }
    }
    return true;
}
