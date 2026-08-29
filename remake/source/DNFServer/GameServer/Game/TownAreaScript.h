#ifndef GAME_TOWNAREASCRIPT_H
#define GAME_TOWNAREASCRIPT_H

#include <string>
#include <vector>
#include "T_Rect.h"

// df_game_r 还原 —— T_Rect / TownAreaScript
// ORIG 符号（nm -S df_game_r 核实）：
//   TownAreaScript C1Ev 0x86d1d9a / D1Ev 0x86d28f6 / C1ERKS_ 0x86d296a /
//   aSERKS_ 0x86d2a1a / clear 0x86d1d46。
// 布局（ctor/dtor/clear 反汇编）：
//   +0x00 int                   m_areaIndex（clear 置 -1）
//   +0x04 std::string           m_mapName（clear 置 ""）
//   +0x08 int                   m_areaType（clear 置 0）
//   +0x0c std::vector<T_Rect>   m_rects（clear 清空）
//   +0x18 std::vector<int>      m_field18（clear 清空）
//   +0x24 unsigned char         m_field24（clear 置 1）
//   +0x25 pad 至 0x30（map 节点按值存储实证 sizeof==0x30）
class TownAreaScript;

class TownAreaScript
{
public:
    TownAreaScript();
    TownAreaScript(const TownAreaScript& other);
    TownAreaScript& operator=(const TownAreaScript& other);
    ~TownAreaScript();
    void clear();   // ORIG 0x86d1d46

    int m_areaIndex;               // +0x00
    std::string m_mapName;         // +0x04
    int m_areaType;                // +0x08
    std::vector<T_Rect> m_rects;   // +0x0c
    std::vector<int> m_field18;    // +0x18
    unsigned char m_field24;       // +0x24
    char m_pad25[0x30 - 0x25];     // +0x25
};

#endif  // GAME_TOWNAREASCRIPT_H
