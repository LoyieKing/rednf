// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BlueMarbleMapScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "BlueMarbleMapScript/BlueMarbleMapScript.inc"
{
    clear();
}

#include "BlueMarbleMapScript/dtor_BlueMarbleMapScript.inc"
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

#include "BlueMarbleMapScript/clear.inc"
{
    m_tiles.clear();
}

#include "BlueMarbleMapScript/getTile.inc"
{
    if (m_tiles.size() < zone || zone == 0)
    {
        return 0;
    }
    return m_tiles[zone - 1];
}

#include "BlueMarbleMapScript/importScript.inc"
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
