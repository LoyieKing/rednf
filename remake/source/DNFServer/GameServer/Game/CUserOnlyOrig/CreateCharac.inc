// ---------------------------------------------------------------------------
// CUser::CreateCharac(SIG_CREATE_CHARAC* sig) T 0x0864a186 sz0x2ab
// 报告：docs/class_func_reports/CUser/CreateCharac.md
// 栈上 _Charac_info：Reset→字段化填充（characNo/name/job/技能表 0x52 拷贝/
// stamina=100/疲劳上限/0xff 禁玩标记等）→局部 CInventory/CCargo 缓冲绑
// InventoryMemory→push_back→CharacAdd(size-1)→enableSaveCharacView→
// _onCreateCharacter→返回 0。局部对象无显式构造的缓冲区按 ORIG 原样
// reinterpret_cast（ORIG 无 CInventory/CCargo C1 调用）。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0864a186：p167=M0/E1:memcpy，gap=-38；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864a186：修法（本轮）——删 invenBuf[0xcd0]/
// cargoBuf[0x4a] 缓冲，改在 info 内嵌子对象偏移（CInventory@0xda/CCargo@0xdaa）上直调
// ⇒ 帧 0x2200→0x14f0（ORIG 0x14fc）；memcpy(0x52) 改结构整体赋值复现 ORIG 内联 rep movsl
// （P83/N78）⇒ p167 CLEAR（15=15），130→172 条（ORIG 168）。残余槽基/初始化序差 4 条
// ⇒ N82/N78 封顶。
int CUser::CreateCharac(SIG_CREATE_CHARAC* sig)
{
    _Charac_info info;
    info.Reset();
    info.m_characNo = sig->m_characNo;
    strncpy(info.m_name, sig->m_name, 0x1e);
    info.m_vill = 1;
    info.m_villFixed = 2;
    info.m_villSpecial = 0;
    info.m_villPrev = 0;
    info.m_job = static_cast<char>(sig->m_slot);           // job
    // [b28d-solo-CUserOnlyOrig P0] ORIG 无 job 槽（`movzbl 0x22(%eax),%eax; movsbl %al,%edx`
    // 每使用点重读 sig）⇒ 内联表达式，旧具名 job 生 -0x9 槽。
    // [ORIG 0864a209..0864a294] recipe 表整体拷贝：dest = &info + 0x88，长 0x52
    // （0x88+0x52 = 0xda = CInventory 起始；旧稿写 +0x84 ⇒ 覆盖 m_field84 且
    //  末尾 4 字节 +0xd6..+0xd9 未初始化 = 语义 bug）；
    // [b28d-solo-CUserOnlyOrig P83/N78] ORIG 内联 rep movsl，结构整体赋值触发同形
    // （显式 memcpy 恒 call）。ORIG 位置在 level/stamina 段之前。
    info.m_additioalInfo =
        G_CDataManager()->m_characList->m_characs[static_cast<char>(sig->m_slot)].m_baseInfo0;
    info.m_level = 1;
    info.m_exp = 0;
    info.m_growthType.m_growthType = 0;
    info.m_stamina = 100;
    reinterpret_cast<CommonTime*>(info.m_delReqTime)->SetCurTime();  // +0x39
    info.m_fatigue = 0;
    info.m_maxFatigue = static_cast<unsigned short>(DEFAULT_MAX_FATIGUE);
    info.m_fatigueGrownUpBuff = 0;
    info.m_fatigueBattery = 0;
    info.m_pad83 = 0;                                       // +0x83
    info.m_field84 = 0;             // +0x84（recipe 区自 +0x88 起，不覆盖本字段）
    info.m_resetDwordDf4 = 0;
    info.m_resetDwordDf8 = 0;
    info.m_powerWarData.m_data2 = 0;                          // +0xe8f（具名 STPowerWarCharacData）
    info.m_skillTreeIndex = 0xff;
    info.m_pad1240 = static_cast<char>(sig->m_eventCharacFlag != 0);
    // [b28d-solo-CUserOnlyOrig P0/P235] ORIG 帧 0x14fc（仅 info 0x14e9 + 小局部）
    // ⇒ 原码无 invenBuf[0xcd0]/cargoBuf[0x4a] 缓冲，直接在 info 内嵌子对象偏移上
    // 调用（_Charac_info 的 CInventory@0xda / CCargo@0xdaa 为 reinterpret 视图区）。
    info.m_curCharacInven.SetInventoryMemory(&m_inventoryMemory);
    info.m_cargo.SetInventoryMemory(&m_inventoryMemory);
    m_characList.push_back(info);
    m_historyLog.CharacAdd(static_cast<int>(m_characList.size()) - 1);
    m_characterView.enableSaveCharacView();
    _onCreateCharacter(sig->m_characNo, sig->m_name, static_cast<char>(sig->m_slot));
    return 0;
}