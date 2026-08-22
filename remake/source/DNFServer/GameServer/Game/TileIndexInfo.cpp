// ============================================================================
// TileIndexInfo（G2 蓝海币/地图批次）
// ----------------------------------------------------------------------------
// ORIG：
//   ctor  _ZN13TileIndexInfoC1Ev  0x80d9f04（+0/+4 两 int 置 0）
//   reset _ZN13TileIndexInfo5resetEv  0x80d9f1c（+0/+4 两 int 置 0）
// 布局：{ int m_dungeon; unsigned int m_item; }（8 字节），与 BlueMarblePOD.h 一致。
// 经 BlueMarbleUserInfo.cpp 的 asm-label extern（_ZN13TileIndexInfoC1Ev / reset）引用。
// ============================================================================

#include "BlueMarblePOD.h"

TileIndexInfo::TileIndexInfo()
{
    m_dungeon = 0;
    m_item = 0;
}

void TileIndexInfo::reset()
{
    m_dungeon = 0;
    m_item = 0;
}