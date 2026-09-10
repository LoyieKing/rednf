int CUser::CreateCharac(SIG_CREATE_CHARAC* sig)
{
    _Charac_info info;
    info.Reset();
    info.m_characNo = sig->m_characNo;
    strncpy(info.m_name, sig->m_name, 0x1e);
    info.m_field22 = 1;
    info.m_field23 = 2;
    info.m_field24 = 0;
    info.m_field25 = 0;
    info.m_field26 = static_cast<char>(sig->m_slot);           // job
    info.m_level = 1;
    info.m_field2b = 0;
    info.m_field29.m_growthType = 0;
    info.m_stamina = 100;
    reinterpret_cast<CommonTime*>(info.m_delReqTime)->SetCurTime();  // +0x39
    info.m_field33 = 0;
    info.m_field35 = static_cast<unsigned short>(DEFAULT_MAX_FATIGUE);
    info.m_field37 = 0;
    info.m_field4f = 0;
    info.m_pad83 = 0;                                       // +0x83
    info.m_field84 = 0;             // +0x84（随后被 0x52 覆盖）
    // [b28d-solo-CUserOnlyOrig P0] ORIG 无 job 槽（`movzbl 0x22(%eax),%eax; movsbl %al,%edx`
    // 每使用点重读 sig）⇒ 内联表达式，旧具名 job 生 -0x9 槽。
    // [b28d-solo-CUserOnlyOrig P83/N78 试形] ORIG 内联 rep movsl（0x52 字节），
    // 结构整体赋值触发同形（显式 memcpy 恒 call）。
    struct RecipeTable52 { char m_data[0x52]; };
    *(RecipeTable52*)(reinterpret_cast<char*>(&info) + 0x84) =
        *(RecipeTable52*)(*reinterpret_cast<char**>(reinterpret_cast<char*>(G_CDataManager()) + 0x14)
            + static_cast<char>(sig->m_slot) * 0x7dc);
    info.m_resetDwordDf4 = 0;
    info.m_resetDwordDf8 = 0;
    info.m_powerWarData.m_data2 = 0;                          // +0xe8f（具名 STPowerWarCharacData）
    info.m_field1239 = 0xff;
    info.m_pad1240 = static_cast<char>(sig->m_eventCharacFlag != 0);
    // [b28d-solo-CUserOnlyOrig P0/P235] ORIG 帧 0x14fc（仅 info 0x14e9 + 小局部）
    // ⇒ 原码无 invenBuf[0xcd0]/cargoBuf[0x4a] 缓冲，直接在 info 内嵌子对象偏移上
    // 调用（_Charac_info 的 CInventory@0xda / CCargo@0xdaa 为 reinterpret 视图区）。
    reinterpret_cast<CInventory*>(reinterpret_cast<char*>(&info) + 0xda)
        ->SetInventoryMemory(&m_inventoryMemory);
    reinterpret_cast<CCargo*>(reinterpret_cast<char*>(&info) + 0xdaa)
        ->SetInventoryMemory(&m_inventoryMemory);
    m_characList.push_back(info);
    m_historyLog.CharacAdd(static_cast<int>(m_characList.size()) - 1);
    m_characterView.enableSaveCharacView();
    _onCreateCharacter(sig->m_characNo, sig->m_name, static_cast<char>(sig->m_slot));
    return 0;
