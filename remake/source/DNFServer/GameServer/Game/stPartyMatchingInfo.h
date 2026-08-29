// stPartyMatchingInfo —— 组队匹配脚本结构（ORIG 顶层类型）。
// ORIG（mangled _ZN19stPartyMatchingInfo...）：
//   C1 W 0x0891b78e、C1ERKS_ W 0x0892ead6、D1 W 0x0891ca12、operator= W 0x0892ebe0、
//   clear W 0x0891b81c（两 string clear + vector<int> clear）。
// importPartyMatchingInfo（T 0x0890f311）实证：
//   +0x00 string（[channel name] ScanStr）、+0x04 string（[region name] ScanStr）、
//   +0x08 vector<int>（[dungeon index] ScanInt 循环）；sizeof 0x14。
#ifndef DNF_GAME_STPARTYMATCHINGINFO_H_
#define DNF_GAME_STPARTYMATCHINGINFO_H_

#include <string>
#include <vector>

struct stPartyMatchingInfo
{
    stPartyMatchingInfo();                            // W 0x0891b78e
    stPartyMatchingInfo(stPartyMatchingInfo const&);  // W 0x0892ead6
    ~stPartyMatchingInfo();                           // W 0x0891ca12
    stPartyMatchingInfo& operator=(stPartyMatchingInfo const&);  // W 0x0892ebe0
    void clear();                                     // W 0x0891b81c

    std::string m_channelName;   // +0x00
    std::string m_regionName;    // +0x04
    std::vector<int> m_dungeonIdx;  // +0x08
};  // sizeof 0x14

#endif  // DNF_GAME_STPARTYMATCHINGINFO_H_
