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
    return ((const CDataManagerRewardSpView*)this)->m_sp8[level + 0x36a2];
