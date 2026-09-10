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
