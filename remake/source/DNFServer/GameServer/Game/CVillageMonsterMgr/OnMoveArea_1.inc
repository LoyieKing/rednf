void CVillageMonsterMgr::OnMoveArea(CUser* user)
{
    // ORIG 0x86b45bc：Zone(curVill, curArea) 查找，命中则转交区域处理。
    // P36/P52：ORIG 0x86b45bc get_area 值经 %ebx 跨 getCurCharacVill 调用保活
    // （无栈槽 ⇒ 无具名局部），vill 为内联右值直进 eax。
    // [b28-L11 清违例] 原 register asm("ebx") 绑定违反 P0/N96 全域禁令；改
    // P52 单语句内联实参形（右起求值，get_area 结果由编译器升格 callee-saved）。
    Zone zone(user->getCurCharacVill(), (unsigned char)user->get_area(false));
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(zone);
    // ORIG 0x86b45bc 实为 eq + jne 早退形，且 pt 后 +0x4 经具名槽 -0xc（P0 指针局部）。
    if (it == m_areas.end())
        return;
    CVillageMonsterArea* areaObj = &it->second;
    areaObj->OnMoveArea(user);
