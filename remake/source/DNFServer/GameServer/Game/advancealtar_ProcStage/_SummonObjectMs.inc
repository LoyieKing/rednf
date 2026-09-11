// ----------------------------------------------------------------------------
// ORIG W 0x08134674：_SummonObject ctor(this+0x14) 后 memset(this,0,0x38)。
// m_summon 为字节槽（CDataManager.h 反向不可达），ctor 显式调用复现调用序列。
// ----------------------------------------------------------------------------
_SummonObjectMs::_SummonObjectMs()
{
    // b28d-52：m_summon 改真实成员后由编译器自动发 _SummonObject::C1 call
    // （ORIG 0x08134677 `add $0x14,%eax; call _SummonObject::C1Ev`），
    // 无需 placement new（消 N83 的 extern _ZnwjPv + EH cleanup 段）。
    std::memset(this, 0, sizeof(*this));
}