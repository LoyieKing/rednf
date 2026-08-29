#ifndef GAME_T_RECT_H
#define GAME_T_RECT_H

// df_game_r 还原 —— T_Rect（TownAreaScript 的矩形坐标组）
// ORIG 符号（nm -S df_game_r 核实）：
//   T_Rect::T_Rect 0x8aae094（调 clear）/ clear 0x8aae0a8（4 个 int 清零，0x10 字节）
//   D1/D2 0x8aae84a（用户空析构，5 字节）。
// 布局（clear 反汇编）：+0x00/+0x04/+0x08/+0x0c 四个 int，sizeof==0x10。
// 实现见 T_Rect.cpp（外联定义，保证 C1 产生 call clear 形态）。

struct T_Rect
{
    int m_00;   // +0x00（字段名未还原：矩形坐标组）
    int m_04;   // +0x04
    int m_08;   // +0x08
    int m_0c;   // +0x0c

    T_Rect();               // ORIG 0x8aae094
    ~T_Rect();              // ORIG 0x8aae84a（平凡）
    void clear();           // ORIG 0x8aae0a8
};

#endif  // GAME_T_RECT_H
