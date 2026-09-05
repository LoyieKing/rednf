// stNoviceCommand —— importNoviceCommandScript（ORIG T 0x0890402a）目标结构。
// 布局证据（objdump 0x0890402a）：ORIG 清 0xb 个 vector<vector<int>> 槽
// （每槽 0xc 字节，+0x0..+0x78，i*3*4+base 寻址 ⇒ 源内即数组成员），
// 脚本按职业写入 8 个槽：
//   +0x00 swordman(8 轮) / +0x0c fighter(8) / +0x18 gunner(10) /
//   +0x24 mage(10) / +0x30 priest(10) / +0x3c at gunner(10) /
//   +0x48 thief(6) / +0x54 at fighter(8)，+0x60..+0x78 三槽仅被清；
//   每轮内层 10 个 ScanInt。
#ifndef DNF_GAME_STNOVICECOMMAND_H_
#define DNF_GAME_STNOVICECOMMAND_H_

#include <vector>

struct stNoviceCommand
{
    // ORIG 职业槽索引（push_back 目标 = base + 槽*0xc，objdump 复核）。
    enum Slot
    {
        SLOT_SWORDMAN  = 0,   // +0x00
        SLOT_FIGHTER   = 1,   // +0x0c
        SLOT_GUNNER    = 2,   // +0x18
        SLOT_MAGE      = 3,   // +0x24
        SLOT_PRIEST    = 4,   // +0x30
        SLOT_ATGUNNER  = 5,   // +0x3c
        SLOT_THIEF     = 6,   // +0x48
        SLOT_ATFIGHTER = 7    // +0x54
    };

    std::vector<std::vector<int> > m_slots[11];   // +0x00..+0x78
};

static_assert(sizeof(stNoviceCommand) == 0x84, "stNoviceCommand size mismatch");

#endif  // DNF_GAME_STNOVICECOMMAND_H_
