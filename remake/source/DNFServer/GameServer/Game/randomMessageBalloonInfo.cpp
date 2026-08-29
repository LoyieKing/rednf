// ============================================================================
// randomMessageBalloonInfo.cpp — randomMessageBalloonInfo 强符号（T）实现
//
// 来源（df_game_r，elf32-i386）：
//   randomMessageBalloonInfo::randomMessageBalloonInfo()  T 0x08a00c08
//   randomMessageBalloonInfo::clear()                     T 0x08a00bd2
// 析构为 ORIG 弱符号（W 0x08581d50，vector D1 + string D1），编译器隐式生成
// （ORIG 无 D2，P12），本文件末尾强制实例化以发出 D1/D0。
// 布局：std::string@+0x00 + int@+0x04 + std::vector<std::string>@+0x08。
// ============================================================================
#include "randomMessageBalloonInfo.h"

// ORIG 0x08a00c08：string 默认构造 → vector 默认构造 → clear()（带 unwind 清理路径）
randomMessageBalloonInfo::randomMessageBalloonInfo()
    : m_randomMessageAnimation(),
      m_randomMessageList()
{
    clear();
}

// ORIG 0x08a00bd2：+0x04 置 0 + vector::clear + vector::reserve(0xa)
void randomMessageBalloonInfo::clear()
{
    m_randomMessageProbability = 0;
    m_randomMessageList.clear();
    m_randomMessageList.reserve(10);
}

// 强制实例化隐式析构（D1/D0），使 ORIG 弱符号 _ZN24randomMessageBalloonInfoD1Ev
// 在链接期可得；函数本身为内部链接，不产生对外符号。
namespace
{
void force_randomMessageBalloonInfo_dtor(randomMessageBalloonInfo* p)
{
    delete p;
}
}
