// stNoviceCommand —— importNoviceCommandScript（ORIG T 0x0890402a）目标结构。
// 布局证据（objdump 0x0890402a）：ORIG 清 0xb 个 vector<vector<int>> 槽
// （每槽 0xc 字节，+0x0..+0x78），脚本按职业写入 8 个槽：
//   +0x00 swordman(8 轮) / +0x0c fighter(8) / +0x18 gunner(10) /
//   +0x24 mage(10) / +0x30 priest(10) / +0x3c at gunner(10) /
//   +0x48 thief(6) / +0x54 at fighter(8)；每轮内层 10 个 ScanInt。
#ifndef DNF_GAME_STNOVICECOMMAND_H_
#define DNF_GAME_STNOVICECOMMAND_H_

#include <vector>

struct stNoviceCommand
{
    std::vector<std::vector<int> > m_swordman;    // +0x00
    std::vector<std::vector<int> > m_fighter;     // +0x0c
    std::vector<std::vector<int> > m_gunner;      // +0x18
    std::vector<std::vector<int> > m_mage;        // +0x24
    std::vector<std::vector<int> > m_priest;      // +0x30
    std::vector<std::vector<int> > m_atGunner;    // +0x3c
    std::vector<std::vector<int> > m_thief;       // +0x48
    std::vector<std::vector<int> > m_atFighter;   // +0x54
    std::vector<std::vector<int> > m_unused1;     // +0x60（ORIG 清 0xb 槽，保留）
    std::vector<std::vector<int> > m_unused2;     // +0x6c
    std::vector<std::vector<int> > m_unused3;     // +0x78
};

#endif  // DNF_GAME_STNOVICECOMMAND_H_
