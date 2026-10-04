// [b28d-solo-CDataManager NEAR-N] 本轮复核：ORIG `mov level,%edx; mov this,
// %eax; add $0x36a2,%edx; mov 0x8(%eax,%edx,4),%eax`。探针 /tmp/p_sp_at{,2,3,4,5}.cpp
// 全形实测：显式 `m_sp8[level+0x36a2]`（视图 +8）4.4 恒产 `mov level,%eax;
// lea 0x36a2(%eax),%edx; mov this,%eax`（lea 合并 + this 后置，q1..q8/r1..r5
// 12 形全败）；改大偏移视图 `m[level]`（视图 +0xda90）可复现装载序
// `mov level,%edx; mov this,%eax; add …,%edx` 但常量被折成 add $0x36a4 + disp 0
// （ORIG 为 add $0x36a2 + disp 8）⇒ 4.4 对 scaled-index 大常量的「全折」约束
// （N94）无源码解。语义等价封顶。
unsigned int CDataManager::GetSpAtLevelUp(int level) const
{
    if (level < 1 || level > 95)
    {
        return 0;
    }
    // ORIG 0x8360cb8：SP-at-level-up 表基址 +0xda90；与 set_reward_sp 共用
    // 同一具名 ABI 视图。ORIG 0x8360cda `mov 0x8(%eax,%edx,4)`，edx=level+0x36a2
    // ⇒ 名义基址 this+8、两步下标 0x36a2+level（P205 拆形族）。
    // 残差 1 条：4.4 恒先载 this 地址再求下标（N76 load 侧地址先物化，
    // register 钉 edx/交换加数均无效），ORIG 为 edx=level+0x36a2 先行。
    return this->m_spTableTail[level];
}