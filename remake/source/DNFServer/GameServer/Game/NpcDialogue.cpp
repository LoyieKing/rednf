// df_game_r NpcDialogue 实现（OrigDataClears 批，2026-08-28）。
// ORIG：ctor T 0x89fb688 / clear T 0x89fb71c（objdump 逐指令转写）。
#include "NpcDialogue.h"

#include <string>
#include <vector>

// ORIG ctor 0x89fb688：+0x0c/+0x10 string 与 +0x14 vector 由编译器默认构造，
// 函数体仅调 clear()（089fb6ba mov this,(%esp); call clear）。
NpcDialogue::NpcDialogue()
{
    clear();
}

// ORIG clear 0x89fb71c：
//   movl $0,(this)；for (i=0; i<=1; ++i) movl $0,0x4(this+i*4)（cmpl $0x1,setle）；
//   string@+0xc/0x10 operator=("")；vector<int>@+0x14 clear()。
void NpcDialogue::clear()
{
    m_field0 = 0;                 // movl $0,(%eax)
    for (int i = 0; i <= 1; ++i)  // ORIG 循环头：jmp 到尾部 cmpl $0x1,setle
        m_field4[i] = 0;          // movl $0,0x4(%eax,%edx,4)
    m_str0c = "";                 // string::operator=(char const*)
    m_str10 = "";                 // string::operator=(char const*)
    m_vec14.clear();              // vector<int>::clear()
}
