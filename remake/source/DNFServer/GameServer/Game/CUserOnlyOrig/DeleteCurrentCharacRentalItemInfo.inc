void CUser::DeleteCurrentCharacRentalItemInfo(int key)
{
    // [b28d-22 修法+真语义 bug] ORIG 0x08677081/0x08677093：**外层** find key = 形参 key
    // （`mov 0xc(%ebp),%eax; mov %eax,-0x18; lea -0x18(%ebp),%edx`）；0x0867706e/0x086770ef：
    // **内层** find key = cur->m_characNo（`mov -0xc(%ebp),%eax; mov (%eax),%eax; mov %eax,-0x1c`）。
    // 旧版两级 key 写反（行为错误）。ORIG 0x086770d8 `jne <尾>` 早退形 + 每处 `lea 0x6ef78(%eax)`
    // 重算（P183 禁引用缓存）。
    const _Charac_info* cur = getCurCharacR();
    if (cur == NULL) return;
    unsigned int accId = cur->m_characNo;
    if (accId == 0) return;
    typedef std::map<unsigned int,
            std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> > >
        RentalOuterMap;                                     // m_rentalInfo (+0x6ef78)
    RentalOuterMap::iterator it = m_rentalInfo.find(key);
    if (it == m_rentalInfo.end()) return;
    std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> >::iterator it2 =
        it->second.find(accId);
    // ORIG 0x086770d8 `test %al,%al; jne <跳过>` ⇒ ==end 早退形（P222 正向包裹）。
    if (it2 == it->second.end()) return;
    it->second.erase(it2);
