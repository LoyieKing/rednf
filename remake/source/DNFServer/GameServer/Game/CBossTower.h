// df_game_r Game/ WongWork::CBossTower（批3：从 GameStubs.cpp 迁移）。
// 唯一声明点：CGameManager.h include 本头（原 CGameManager.h 内本地占位
// class CBossTower 已删除，避免重复定义）。ctor/dtor 实现在 CDeathTower.cpp，
// getIdx/setIdx 实现在 CBossTower.cpp。
// 布局：总尺寸 0xb18（ORIG StaticPool<CBossTower,600> 节点数据区）。
#ifndef DNF_CBOSSTOWER_H
#define DNF_CBOSSTOWER_H

namespace WongWork
{
class CBossTower
{
public:
    CBossTower();
    ~CBossTower();

    int getIdx();
    void setIdx(int idx);

private:
    char m_pad[0xb18];
};
}

#endif  // DNF_CBOSSTOWER_H
