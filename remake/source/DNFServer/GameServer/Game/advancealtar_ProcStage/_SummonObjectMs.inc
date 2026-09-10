_SummonObjectMs::_SummonObjectMs()
{
    // b28d-52：m_summon 改真实成员后由编译器自动发 _SummonObject::C1 call
    // （ORIG 0x08134677 `add $0x14,%eax; call _SummonObject::C1Ev`），
    // 无需 placement new（消 N83 的 extern _ZnwjPv + EH cleanup 段）。
    std::memset(this, 0, sizeof(*this));
