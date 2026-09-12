// ORIG 0x0867794c _ZNK5CUser10GetEtcInfoEP12SIG_LOAD_ETC
// 依 SIG_LOAD_ETC 具名布局填充。expand 子区经 CExpandEquipslot vtable 槽 +8（getData）
// 直调（同 CacheCharacterMemory 的 0xc 槽位写法）；混合区内子对象用文档化偏移定位。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867794c | _ZNK5CUser10GetEtcInfoEP12SIG_LOAD_ETC
// [b28d-GrpD 判词·无实差异] ORIG 0x0867794c：norm 后残差仅 `lea 0xN(%R),%R` vs
//   `add $BIG,%R`（数据地址未解析重定位）与寄存器选择差异；字段偏移、PvpResultType::Get
//   成员直调序全一致 ⇒ 非可改项。
int CUser::GetEtcInfo(SIG_LOAD_ETC* sig) const
{
    sig->m_field8 = 0;
    // ORIG 0x08677979：PvpResultType::Get(source, dest) 成员直调，非裸 memcpy。
    getPVPResultR()->Get(sig->m_pvpResult);
    GetRecord(sig);
    GetContinuousItemInfo(sig);
    GetLimitItemUsage(sig);
    sig->m_oneDayLetheFlag = GetOneDayLetheFlag();
    // [R18arr 2026-09-13] m_backupSkill 已由 blob 改为真实成员 SIG_LOAD_PREMIUM_LETHE
    // （+0x29fd，sizeof 0x199）：取址形式 `&obj` 与旧数组名退化同为常量地址 ⇒ 零指令差。
    memcpy(&sig->m_backupSkill, GetCurCharacBackupSkill(), 0x199);
    sig->m_oneDayLetheFlag2ND = GetOneDayLetheFlag2ND();
    memcpy(&sig->m_backupSkill2ND, GetCurCharacBackupSkill2ND(), 0x199);
    sig->m_curConditionEventStep = GetCurConditionEventStep();
    sig->m_curConditionEventRewardStep = GetCurConditionEventRewardStep();
    sig->m_properDungeonClearCount = GetProperDungeonClearCount();

    // expand type 6 → +0x40ce（真实虚调用 getData @ vtable+8）。
    // 2026-09-10（P183/P0，报告 0x08677a79..0x08677a95 实证）：ORIG 对每个 expand 段
    //   都是 `movl $type,0x4(%esp); … call GetCharacExpandDataR; mov (%eax),%edx;
    //   add $0x8,%edx; mov (%edx),%ecx; … call *%ecx` **全链内联**，帧仅 sub $0x10 ⇒
    //   原码无任何 `CExpandEquipslot* eXX` 具名局部（我方 12 个局部致帧 0x40、+40 指令）⇒
    //   全部改链式单表达式。
    // [R18arr 2026-09-13] +0x40ce 区 blob → 真实成员 SIG_REDEEMINFO_LIST_TO_DB。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)6)->getData((char*)&sig->m_redeemInfo);

    sig->m_curVillageAttackCount = GetCurVillageAttackCount();
    sig->m_curRevengeDungeonCount = GetCurRevengeDungeonCount();
    sig->m_flagPresentAvengerTitle = getFlagPresentAvengerTitle();
    // [R18arr 2026-09-13] +0xc9af 区 blob+标量 → 真实成员 SIG_SAVE_PC_ROOM_PLAY_TIME。
    sig->m_pcRoomPlayTime.m_totalTime = getTotalPcRoomPlayTime();
    sig->m_pcRoomPlayTime.m_rewardCnt = getPcRoomPlayTimeRewardCnt();
    sig->m_curCharacUsedGiftFatigueQuantity = GetCurCharacUsedGiftFatigueQuantity();
    // ORIG 调 std::bitset<32>::set()（无参，全部 32 位置 1 → 0xffffffff）；
    // 直接赋值会经 bitset(unsigned long) ctor（EXTRA）而非 set()。
    sig->m_bitset.set();

    // skill 区（+0xc9c0）+ combo（+0x14aaa）
    GetSkill((SIG_LOAD_SKILL*)sig->m_mixedRegion, (SIG_COMBO_SKILL*)sig->m_comboSkill);

    // 混合区内 expand 子区（m_mixedRegion 具名成员 + 区内文档化下标）。
    // ORIG 顺序：type 7/0xd/8/0xb 无条件；tag 角色存在时只填充 tag 角色技能槽，
    // 其余 type 0xa/0x10/0xe/0xf/0x11/0x12、aura、option、attendance、
    // blueMarble、type 0x13 均无条件执行。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)7)
        ->getData(&sig->m_mixedRegion[0xcf1f - 0xc9c0]);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xd)
        ->getData(&sig->m_mixedRegion[0xd718 - 0xc9c0]);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8)
        ->getData(&sig->m_mixedRegion[0xcdd5 - 0xc9c0]);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xb)
        ->getData(&sig->m_mixedRegion[0xd5a7 - 0xc9c0]);

    // tag 角色技能槽（+0x10/+0x1a8）
    if (getTagCharacR() != 0)
    {
        // SIG_LOAD_ETC 新结构：tag 区为 TAG_CHARAC_ETC_DATA（2 行 × 0x198，
        // Lane6 已将平铺对拆成真实二维数组 m_skills[2][204]，见 SigTypes_C.h:300）。
        ((SkillSlot*)getTagCharacSkillR())->get_skill_slot(
            reinterpret_cast<char*>(sig->m_tagEtc.m_skills[0]),
            reinterpret_cast<char*>(sig->m_tagEtc.m_skills[1]), 0x198);
    }

    // ORIG 0x0867794c：expand type 0xa → mix+0xd6fc（0xd6fc - 0xc9c0）。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xa)
        ->getData(&sig->m_mixedRegion[0xd6fc - 0xc9c0]);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x10)
        ->getData(&sig->m_mixedRegion[0xd73c - 0xc9c0]);
    // [R18arr 2026-09-13] +0x466a/+0xb1a4 区 blob → 真实成员 stTitleBook / stAchievement_DBData。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xe)
        ->getData((char*)&sig->m_titlebook);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xf)
        ->getData((char*)&sig->m_achievement);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x11)
        ->getData(&sig->m_mixedRegion[0xecc8 - 0xc9c0]);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x12)
        ->getData(&sig->m_mixedRegion[0xecd0 - 0xc9c0]);

    // aura（+0xd730）/ character option（+0xeac8）在混合区内
    GetAuraAvatarOption(
        *(stAuraAvatarOption*)&sig->m_mixedRegion[0xd730 - 0xc9c0]);
    GetCurCharacOption(
        *(CharacterOption*)&sig->m_mixedRegion[0xeac8 - 0xc9c0]);

    // attendance（+0x14a99）+ blueMarble（+0x14a98）+ expand 0x13（+0x14a9a）
    GetCharacCashAttendanceInfo(*(SIG_ATTENDANCE_EVENT*)&sig->m_attendanceEvent);
    sig->m_blueMarbleEnterCount = getBlueMarbleEnterCount();
    // type 0x13 区 = attendanceEvent 内 +1 字节起（0x14a99 起 0x11 字节，0x14a9a=+1）。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x13)
        ->getData((char*)&sig->m_attendanceEvent + 1);

    return 1;
}