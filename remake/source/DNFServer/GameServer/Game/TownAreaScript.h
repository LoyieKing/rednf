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
    TownAreaScript(const TownAreaScript& other);      // ORIG W 0x86d296a（=default 外联发射）
    TownAreaScript& operator=(const TownAreaScript& other);  // ORIG W 0x86d2a1a
    ~TownAreaScript();
    void clear();   // ORIG 0x86d1d46

    int m_areaIndex;               // +0x00
    std::string m_mapName;         // +0x04
    int m_areaType;                // +0x08
    std::vector<T_Rect> m_rects;   // +0x0c
    std::vector<int> m_field18;    // +0x18
    unsigned char m_field24;       // +0x24
    // [b28-L14] 删显式 pad + 用户拷贝构造/op=（P26/P74）：ORIG C1ERKS_ 0x86d296a
    // /aSERKS_ 0x86d2a1a 均为 W 隐式合成，逐字节循环拷 m_pad 是伪差来源；
    // 尾部 +0x25..+0x27 由编译器按 4 字节对齐自动填充（sizeof 仍 0x28）。
};

#endif  // GAME_TOWNAREASCRIPT_H
