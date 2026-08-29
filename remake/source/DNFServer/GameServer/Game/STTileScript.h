// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// STTileScript：ORIG 仅导出 Clear() W @0x089e82f6；
// 布局由 Clear 逐偏移实证：+0x00 vector<pair<string,int>>、+0x0c byte、
// +0x10 vector<vector<int>>、+0x1c int。
#ifndef DNF_GAME_STTILESCRIPT_H_
#define DNF_GAME_STTILESCRIPT_H_

#include <string>
#include <utility>
#include <vector>

struct STTileScript
{
    std::vector<std::pair<std::string, int> > m_actions;   // +0x00
    char m_flagC;                       // +0x0c
    std::vector<std::vector<int> > m_collisions;  // +0x10
    int m_field1c;                      // +0x1c

    void Clear();  // ORIG W @0x089e82f6
};

#endif  // DNF_GAME_STTILESCRIPT_H_
