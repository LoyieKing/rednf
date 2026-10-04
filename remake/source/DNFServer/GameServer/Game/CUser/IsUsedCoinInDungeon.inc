// [R6a18 类型旋钮对齐·Main 授权] 与 CUser.h 声明同步：unsigned char→bool。
// 依据：ORIG 唯一消费点（CParty::ClearDungeon 0x085a96d4）为 bool 形 `xor $0x1`
// （raw `83 f0 01 84 c0 74 3a`）；字段 m_usedCoinInDungeon 已为 bool（值域 {0,1}）。
bool CUser::IsUsedCoinInDungeon() const
{
    return m_usedCoinInDungeon;
}