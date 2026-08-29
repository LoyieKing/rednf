// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// 纯 ctor/dtor/assign 数据类。
#ifndef DNF_GAME_STWORLDMAPPATTERN_H_
#define DNF_GAME_STWORLDMAPPATTERN_H_

#include <string>

// op=：string op= + 聚合整体拷贝（+0x08..+0x1c 六 int 经基址缓存展开，
// 0x088f2e04 实证——仅嵌套聚合赋值可复现该形态）+ +0x20/+0x24，返回 this。
// coord[3][2] 聚合（+0x08，0x18 字节）。
// ---- STWorldmapPattern（ORIG C1 W 0x088f2886 / D1 W 0x088f2e70 /
//      op= W 0x088f2e04，全 W → 头内联）----
// 布局（C1/op= 逐字段实证）：string@+0x00 + int@+0x04 + coord[3][2]@+0x08
// + int@+0x20 + int@+0x24，sizeof 0x28。
// C1：string 默认构造 → operator=("")（0x8e0d500 处空串）→ +0x04=-1 →
// +0x20/+0x24=0（+0x08..+0x1c 未初始化）；D1 仅 string 析构。
struct STWorldmapCoord
{
    int m_coord[3][2];
};

class STWorldmapPattern
{
public:
    STWorldmapPattern()
    {
        // ORIG 0x088f2886：string 隐式默认构造 → m_str0=""（op=）→
        // +0x04=-1 → +0x20/+0x24=0（+0x08..+0x1c 未初始化）。
        m_str0 = "";
        m_field4 = -1;
        m_field20 = 0;
        m_field24 = 0;
    }

    ~STWorldmapPattern() {}

    STWorldmapPattern& operator=(STWorldmapPattern const& o)
    {
        m_str0 = o.m_str0;
        m_field4 = o.m_field4;
        m_coord = o.m_coord;
        m_field20 = o.m_field20;
        m_field24 = o.m_field24;
        return *this;
    }

    std::string m_str0;       // +0x00
    int m_field4;             // +0x04
    STWorldmapCoord m_coord;  // +0x08（0x18 字节）
    int m_field20;            // +0x20
    int m_field24;            // +0x24
};   // sizeof 0x28

#endif  // DNF_GAME_STWORLDMAPPATTERN_H_
