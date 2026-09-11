// ORIG T 0x086773ea：返回 m_rentalInfo[param][charac_no] 是否存在；存在则拷贝到 out。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086773ea | _ZN5CUser26getCurrentCharacRentalInfoERSt6vectorIN20InstanceRentalSystem10RentalInfoESaIS2_EEi
bool CUser::getCurrentCharacRentalInfo(
    std::vector<InstanceRentalSystem::RentalInfo>& out, int param)
{
    // ORIG 0x086773fd..0x08677415：cur 落 -0xc 判空后**直读 cur->m_characNo**
    //   （`mov (%eax),%eax`），非 getCurCharacNo 调用（我们旧形多一次 call，p167 EXTRA）。
    const _Charac_info* cur = getCurCharacR();
    if (cur == 0)
        return false;
    unsigned int characNo = (unsigned int)cur->m_characNo;
    if (characNo == 0)
        return false;

    std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> > inner;
    GetInstanceRentalInfo(inner, param);
    // b28d-solo-CUser p167：ORIG 0x086773ea 用 const_iterator（find 与 end()
    //   各一次转换构造 + const operator==/operator*）⇒ 改 const_iterator + `(*it).second`。
    std::map<unsigned int,
             std::vector<InstanceRentalSystem::RentalInfo> >::const_iterator it =
        inner.find(characNo);
    if (it == inner.end())
        return false;
    out = (*it).second;
    return true;
}