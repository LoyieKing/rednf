// df_game_r BlueMarbleMapScript（G2-4 大富翁地图脚本）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarbleMapScript.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 跨类依赖（loadRDARScriptFile/ScanType/ScanInt/BlueMarbleScriptManager/
// BlueMarbleTileScript 构造）经 asm-label extern / 声明调用真实符号。
#include <string>

#include "BlueMarbleMapScript.h"
#include "BlueMarbleTileScript.h"

class BlueMarbleScriptManager;

// ============================================================================
// 跨类 / 脚本解析方法（asm-label extern）
// ============================================================================
extern "C" bool sub_loadRDARScriptFile(const char* dir, const char* path)
    asm("_Z18loadRDARScriptFilePKcS0_");
extern "C" bool sub_ScanType(std::string& line, bool value)
    asm("_Z8ScanTypeRSsb");
extern "C" bool sub_ScanInt(int* out) asm("_Z7ScanIntPi");

extern "C" BlueMarbleScriptManager* sub_BSM_getInsance()
    asm("_ZN23BlueMarbleScriptManager10getInsanceEv");
extern "C" BlueMarbleTileScript* sub_BSM_getTile(BlueMarbleScriptManager* self, int idx)
    asm("_ZN23BlueMarbleScriptManager7getTileEi");

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
    if (!sub_loadRDARScriptFile("", path))
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
        if (!sub_ScanType(line, true))
        {
            break;
        }
        if (line == "[tile]")
        {
            m_tiles.clear();
            for (;;)
            {
                if (!sub_ScanInt(&value))
                {
                    break;
                }
                tile = sub_BSM_getTile(sub_BSM_getInsance(), value);
                if (tile == 0)
                {
                    return 0;
                }
                if (!sub_ScanInt(&value))
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
