void CMonster::generateIndependentItem(
    const WongWork::stGenerateRefData_t& ref,
    std::vector<Inven_Item>& out) const
{
    // ORIG 0x0834f876：`call getMonsterScript; lea 0x3ac(%eax)` 无局部中转。
    // static_cast 结果直接取容器内联传参（P0/技巧36），声明 script 局部多一
    // -0xc 槽令帧 0x28→0x18 失配。
    ::generateIndependentItem(
        static_cast<STMonsterScript*>(getMonsterScript())->m_independentDrops,
        ref, out);
