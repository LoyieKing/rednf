#ifndef GAME_NPCDIALOGUE_H_
#define GAME_NPCDIALOGUE_H_

// 布局依据 ORIG 反汇编（ctor 0x89fb688 / clear 0x89fb71c，df_game_r T 符号）：
//   +0x00 int；+0x04 int[2]（clear 以 for(i=0;i<=1;++i) 循环单 disp 寻址清零）
//   +0x0c std::string（gcc4.4 COW，4 字节）；+0x10 std::string
//   +0x14 std::vector<int>；sizeof 0x20
#include <string>
#include <vector>

class NpcDialogue
{
public:
    // ORIG 0x89fb688 _ZN11NpcDialogueC1Ev：成员默认构造后调 clear()
    NpcDialogue();
    // ORIG 0x89fb71c _ZN11NpcDialogue5clearEv
    void clear();

    int m_field0;           // +0x00
    int m_field4[2];        // +0x04, +0x08
    std::string m_str0c;    // +0x0c
    std::string m_str10;    // +0x10
    std::vector<int> m_vec14;  // +0x14
};

#endif  // GAME_NPCDIALOGUE_H_
