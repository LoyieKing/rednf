CUser* CGameManager::GetUserByAccId(unsigned int accId)
{
    // b28d-solo-CGameManager: ORIG 0x082948c6 早返链（P16/P222 正极性）：
    //   it==end → `mov $0,%eax; jmp epilogue`；get_acc_id()==accId → 直返 user；
    //   不等 → cMyTrace 临时链后 `mov $0,%eax` 落 epilogue。原 `user=0` 槽形
    //   会产 `movl $0,-0xc` 中转（P167 已 CLEAR，本修为控制流/槽形）。
    std::map<unsigned int, CUser*>::iterator it;
    it = m_userByAccId.find(accId);
    if (it == m_userByAccId.end())
        return 0;
    CUser* user = it->second;
    if (user->get_acc_id() == accId)
        return user;
    // P227 全表达式临时链（N80 突破口）——ORIG 0x0829499e 在 cMyTrace ctor 前
    // 右起求 varargs（mapId 先入 %esi、inId 后入 %ebx）并跨 ctor 保活。
    cMyTrace(__PRETTY_FUNCTION__, 0x9ab, 5)(
        "[%s][%d][IN m_id: %s][Map m_id: %s]",
        __PRETTY_FUNCTION__, 0x9ab,
        NumberToString(accId, 0), NumberToString(user->get_acc_id(), 1));
    return 0;
