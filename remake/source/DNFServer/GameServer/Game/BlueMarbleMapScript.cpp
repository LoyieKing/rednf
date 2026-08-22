// df_game_r BlueMarbleMapScript（G2-4 大富翁地图脚本）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarbleMapScript.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 跨类依赖（loadRDARScriptFile/ScanType/ScanInt/BlueMarbleScriptManager/
// BlueMarbleTileScript 构造）经 asm-label extern / 声明调用真实符号。
#include <string>

#include "BlueMarbleMapScript.h"
#include "BlueMarbleTileScript.h"
#include "BlueMarbleScriptManager.h"   // BlueMarbleScriptManager::getInsance/getTile（§9）
#include "DNFLexWrapper.h"             // loadRDARScriptFile/ScanType/ScanInt（§9）

// ============================================================================
// BlueMarbleMapScript 实现
// ============================================================================

BlueMarbleMapScript::BlueMarbleMapScript()
{
    clear();
}

BlueMarbleMapScript::~BlueMarbleMapScript()
{
    unsigned int i = 0;
    while (i < m_tiles.size())
    {
        if (m_tiles[i] != 0)
        {
            delete m_tiles[i];
        }
        m_tiles[i] = 0;
        ++i;
    }
    m_tiles.clear();
}

void BlueMarbleMapScript::clear()
{
    m_tiles.clear();
}

BlueMarbleTileScript* BlueMarbleMapScript::getTile(unsigned int zone)
{
    if (m_tiles.size() < zone || zone == 0)
    {
        return 0;
    }
    return m_tiles[zone - 1];
}

int BlueMarbleMapScript::importScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }
    // 局部声明顺序与 ORIG 栈布局一致（line/-0x10、value/-0x14、tile/-0xc、t/-0x18）
    std::string line;
    int value = 0;
    BlueMarbleTileScript* tile;
    BlueMarbleTileScript* t;
    for (;;)
    {
        if (!ScanType(line, true))
        {
            break;
        }
        if (line == "[tile]")
        {
            m_tiles.clear();
            for (;;)
            {
                if (!ScanInt(&value))
                {
                    break;
                }
                tile = BlueMarbleScriptManager::getInsance()->getTile(value);
                if (tile == 0)
                {
                    return 0;
                }
                if (!ScanInt(&value))
                {
                    break;
                }
                t = new BlueMarbleTileScript;
                struct BM_TileRaw
                {
                    int m_a;  // +0x00
                    int m_b;  // +0x04
                    int m_c;  // +0x08
                };
                *(BM_TileRaw*)t = *(BM_TileRaw*)tile;
                t->m_value = value;
                m_tiles.push_back(t);
            }
        }
    }
    return 1;
}
