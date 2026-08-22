#ifndef GAME_TOWNAREASCRIPT_H
#define GAME_TOWNAREASCRIPT_H

#include <string>
#include <vector>

// df_game_r 还原 —— TownAreaScript（sizeof 0x30）
// ORIG 布局（Village::set_village 0x86c33b8 栈布局 -0x98/-0x68 与
// Area::set_area 0x86c1d14 字段访问推导）：
//   +0x00 int    m_areaIndex    （set_village 判定 / set_area 写入 m_areaIndex）
//   +0x04 string m_mapName      （COW 4 字节；set_village 取 c_str 传 ImportMapScript）
//   +0x08 int    m_areaType     （set_area 区域类型：1=村庄 2=钓鱼/竞技场 4=特殊）
//   +0x0c pad（12）
//   +0x18 std::vector<int>      （set_area 类型 2 的 LotteryInfo 权重 / 类型 4 的 worldMapIndex）
//   +0x24 unsigned char         （set_area 写 m_field6c 限制交易标记）
// 构造/拷贝/赋值/析构符号（_ZN14TownAreaScriptC1Ev / C1ERKS_ / aSERKS_ / D1Ev）
// 由 TownAreaScript.cpp 提供（0x30 POD 语义实现，与 ORIG 弱符号一致）。
// Village.cpp 与 Area.cpp 共用本头，避免两 TU 布局漂移（Area.cpp 旧局部定义
// 仅 0x28，与 ORIG 0x30 不符）。
class TownAreaScript
{
public:
    TownAreaScript();
    TownAreaScript(const TownAreaScript& other);
    TownAreaScript& operator=(const TownAreaScript& other);
    ~TownAreaScript();

    int m_areaIndex;              // +0x00
    std::string m_mapName;        // +0x04
    int m_areaType;               // +0x08
    char m_pad0c[0x18 - 0x0c];    // +0x0c
    std::vector<int> m_field18;   // +0x18
    unsigned char m_field24;      // +0x24
    char m_pad25[0x30 - 0x25];    // +0x25
};

#endif  // GAME_TOWNAREASCRIPT_H
