// ---------------------------------------------------------------------------
// _Charac_info::Reset() T 0x086822b8 sz0x7d9
// 报告：docs/class_func_reports/_Charac_info/Reset.md（## 汇编+## 反编译 C 全文）。
// 全字段清零 + CommonTime::clear + _Additioal_info::clear + CInventory::reset +
// SkillSlot::clear_all_skills_both + CCargo::reset + 两 map clear +
// tagCharacVisibleFlag::reset + stAuraAvatarOption::reset。
// ---------------------------------------------------------------------------
void _Charac_info::Reset()
{
    // 2026-09-04 §4.3 转换：删除 char* p 基址视图（ORIG 0x086822b8 全程
    // mov 0x8(%ebp),%eax; movx $imm,off(%eax) 直读 this，无局部 spill）。
    // 具名字段按 CUser.h 布局；pad 区写保留「布局未建模」裸视图（待 Main 代落）。
    m_characNo = 0;                                   // +0x00 movl
    memset(m_name, 0, 0x1e);                          // +0x04..0x21
    m_field22 = 0;                                    // +0x22 movb
    m_field23 = 0;                                    // +0x23 movb
    m_field26 = 0;                                    // +0x26 movb
    m_level = 0;                                      // +0x27 movw
    m_field29.m_growthType = 0;                       // +0x29 movb（具名 STGrowthType）
    m_pad2a.m_growthType = 0;                         // +0x2a movb（具名 STGrowthType）
    m_field2b = 0;                                    // +0x2b movl
    m_field2f = 0;             // +0x2f movl（pad 未建模）
    m_field33 = 0;                                    // +0x33 movw
    m_field35 = 0;                                    // +0x35 movw
    m_field37 = 0;                                    // +0x37 movw
    reinterpret_cast<CommonTime*>(m_delReqTime)->clear();   // +0x39 call
    m_field77 = 0;                                    // +0x77 movw
    m_field79 = 0;                                    // +0x79 movw
    m_field7b = 0;                                    // +0x7b movl
    m_field7f = 0;                                    // +0x7f movl
    m_additioalInfo.clear();                                  // +0x88 call（具名子对象；判据②消视图 cast）
    m_curCharacInven.reset();
    m_skillSlot.clear_all_skills_both();
    m_cargo.reset();
    m_guildId = 0;                                    // +0xdbe movl
    m_fielddc2 = 0;                                   // +0xdc2 movb
    m_fieldE02 = 0;  // +0xe02 movl（pad 未建模）
    m_resetDwordDf4 = 0;                              // +0xdf4 movl
    m_resetDwordDf8 = 0;                              // +0xdf8 movl
    m_fieldDFE = 0;  // +0xdfe movl（pad）
    m_fieldE06 = 0;  // +0xe06 movl（pad）
    m_fieldE0A = 0;  // +0xe0a movl（pad）
    memset(m_warAreaKill, 0, sizeof(m_warAreaKill));      // +0xe0e memset(0x14)（pad）
    m_fielde22 = 0;                                   // +0xe22 movl
    m_fieldE26 = 0;  // +0xe26 movl（pad）
    m_fieldE2A = 0;  // +0xe2a movl（pad）
    m_fieldE33 = 0;  // +0xe33 movl（pad）
    m_fieldE37 = 0;  // +0xe37 movl（pad）
    m_fieldE2E = 0;  // +0xe2e movl（pad）
    m_fieldE32 = 0; // movb（pad）（Reset.md 证据）
    m_needCheckOverEquipItem = 0;                     // +0xe3b movb
    m_pad83 = 0;                                   // +0x83 movb
    m_field84 = 0;           // +0x84 movl（pad83 后继，未建模）
    m_fieldE3C = 0;            // +0xe3c movl（pad）
    m_fieldDE9 = 0;  // +0xde9 movl（pad）
    m_fieldDED = 0; // +0xded movw（pad）
    m_fieldDEF = 0;  // +0xdef movl（pad）
    m_fieldDF3 = 0; // movb（pad）（Reset.md 证据）
    m_fieldDFC = 0;          // +0xdfc movw（pad）
    m_fieldE41 = 0;  // +0xe41 movl（pad）
    m_fielde49 = 0;                                   // +0xe49 movl
    m_fielde4d = 0;                                   // +0xe4d movl
    m_fielde45 = 0;                                   // +0xe45 movl
    m_fielde51 = 0;                                   // +0xe51 movl
    m_fielde55 = 0;                                   // +0xe55 movl
    m_fielde59 = 0;                                   // +0xe59 movl
    m_fieldE61 = 0;            // +0xe61 movl（pad）
    memset(&m_powerWarData, 0, 0xc);  // +0xe8d memset(0xc)（具名 STPowerWarCharacData）
    m_powerWarData.m_data3 = 1;      // +0xe90 movb $1（具名；ctor 亦置 1，Reset 复查同形）
    m_field24 = 0;                                    // +0x24 movb
    m_fieldE65 = 0; // movb（pad）（Reset.md 证据）
    m_fielde5d = 0;                                   // +0xe5d movl
    m_fieldE66 = 0; // movb（pad）（Reset.md 证据）
    m_fieldE67 = 0; // movb（pad）（Reset.md 证据）
    m_fielde68 = 0;                                   // +0xe68 movl
    m_fieldE6C = 0;            // +0xe6c movl（pad）
    m_fielde71 = 0;                                   // +0xe71 movl
    m_fieldE75 = 0;            // +0xe75 movl（pad）
    m_fielde79 = 0;                                   // +0xe79 movl
    m_fielde7d = 0;                                   // +0xe7d movl
    m_fielde81 = 0;                                   // +0xe81 movl
    m_fieldE85 = 0;            // +0xe85 movl（pad）
    m_fieldE89 = 0;  // +0xe89 movl（pad）
    m_fieldE99 = 0; // movb（pad）（Reset.md 证据）
    m_schoolPoint = 0;  // +0x71 movl（CReliablePerson 16B 视图内）
    m_fieldEB2 = 0; // movb（pad）（Reset.md 证据）
    m_fieldEB4 = 0; // movb（pad）（Reset.md 证据）
    m_fieldEB8 = 0; // movb（pad）（Reset.md 证据）
    m_fieldEB7 = 0; // movb（pad）（Reset.md 证据）
    memset(m_demensionInout, 0, 6);  // +0xeb9 memset(6)
    m_fieldEBF = 0; // movb（pad）（Reset.md 证据）
    memset(m_ultimateInout, 0, 3);   // +0xec0 memset(3)
    m_fieldEC3 = 0; // movb（pad）（Reset.md 证据）
    m_fieldEC4 = -1;   // +0xec4 movl $-1（pad）
    m_fieldEC8 = 400000000;  // +0xec8 movl（pad）
    m_bloodBestRecords.clear();                       // +0xecc map::clear（内联）
    m_fieldEE4 = 0;                                  // +0xee4 movb（pad）
    // ORIG 0x086822b8+0x443 `mov $0x0,%edx; mov %edx,0xee5(%eax)`：float 成员 0.0f
    // 物化两段形（probe p14.w1；Main 已落 CUser.h:637 int→float，同宽）。
    m_fieldEE5 = 0.0f;     // +0xee5（Reset.md 证据）
    m_field4f = 0;                                    // +0x4f movw
    m_fieldEE9 = 0; // movb（pad）（Reset.md 证据）
    m_fieldEEA = 0; // movb（pad）（Reset.md 证据）
    m_fieldEEB = 0;  // +0xeeb movw（pad）
    memset(&m_premiumLethe, 0, 0x199);   // +0xeed memset(0x199)（具名 SIG_LOAD_PREMIUM_LETHE）
    memset(m_tradeRestrictData, 0, 0x10);             // +0x55 memset(0x10)
    m_field121f = 0;                                  // +0x121f movw
    m_field1221 = 0;                                  // +0x1221 movl
    m_field1225 = 0;                                  // +0x1225 movl
    m_field1229 = 0;         // +0x1229 movw（pad）
    m_field122B = 0;                                 // +0x122b movb（pad）
    m_field122C = 0;     // +0x122c movw（pad）
    m_field122E = 0;       // +0x122e movl（pad）
    m_field1232 = 0;     // +0x1232 movw（pad）
    m_field1234 = 0;   // +0x1234 movw（pad）
    m_field1236 = 0;   // +0x1236 movw（pad）
    m_field1238 = 0;                               // +0x1238 movb（pad）
    m_field1239 = 0xff;                               // +0x1239 movb $0xff
    m_field123A = 0;         // +0x123a movw（pad）
    m_field123C = 0;     // +0x123c movw（pad）
    m_field123e = 0;                                  // +0x123e movb
    m_field1241 = 1;                                  // +0x1241 movb $1
    m_field1242 = 0;                                 // +0x1242 movb（pad）
    m_field1243 = 0;                                 // +0x1243 movb（pad）
    m_field1244 = 0;                                 // +0x1244 movb（pad）
    m_field1245 = 0xff;                              // +0x1245 movb $0xff（pad）
    m_characLinkType = 0;                             // +0x1246 movb
    m_field1247 = 0;           // +0x1247 movl（pad）
    m_field124B = 0;       // +0x124b movl（pad）
    m_linkBonusSetting = 0;                           // +0x124f movb
    m_field1250 = 0;           // +0x1250 movl（pad）
    m_skillCommandsMap.clear();                       // +0x1254 map::clear（内联）
    m_field126c = 0;                                  // +0x126c movw
    m_field126e = 0;                                  // +0x126e movb
    m_field126f = 0;                                  // +0x126f movl
    m_field1273 = 0;                                 // +0x1273 movb（pad）
    m_field1274 = 0;                                 // +0x1274 movb（pad）
    m_field1275 = 0;                                  // +0x1275 movb
    m_field1276 = 0;                                  // +0x1276 movb
    m_field1277 = 0;                                  // +0x1277 movb
    m_field1278 = 0;                                  // +0x1278 movb
    m_pad1279 = 0;                                    // +0x1279 movb
    m_field127a = 0;                                  // +0x127a movb
    m_tagVisibleFlags.reset();
    m_field127B = 0;                                 // +0x127b movb（pad）
    m_field127C = 0;       // +0x127c movl（pad）
    m_field1280 = 0;                                  // +0x1280 movb
    m_auraAvatarOption.reset();
    m_field1291 = 0;  // +0x1291 movl（pad）
    m_field128D = 0;  // +0x128d movl（pad）
    m_field1295 = 0;  // +0x1295 movl（pad）
    m_field1299 = 0;  // +0x1299 movl（pad）
    memset(&m_wpBonusPointResult, 0, 8);    // +0x129d memset(8)（具名）
    memset(&m_expBonusPointResult, 0, 8);   // +0x12a5 memset(8)（具名）
    m_field12ad = 0;                                  // +0x12ad movl
    m_field12B1 = 0;           // +0x12b1 movl（pad）
    m_field12B5 = 0;     // +0x12b5 movw（pad）
    memset(m_characterOption.m_pad, 0, 0x200);     // +0x12b7 memset(0x200)（具名 CharacterOption）
    m_field14b7 = 0;                                  // +0x14b7 movb
    m_field14b8 = 0;                                  // +0x14b8 movl
    m_field14BC = 0;                                 // +0x14bc movb（pad）
    m_field14BD = 0;                                 // +0x14bd movb（pad）
    m_field14BE = 0;       // +0x14be movl（pad）
    m_field14C2 = 0;       // +0x14c2 movl（pad）
    m_viewState = 0;                                  // +0x14c6 movl
    m_aniDeco.m_field0 = 0;  // +0x14ca movw（具名 seriaRoom_AniDeco）
    m_aniDeco.m_field2 = 0;  // +0x14cc movw（具名）
    m_field14CE = 0;                                 // +0x14ce movb（pad）
    m_field14CF = 0;                                 // +0x14cf movb（pad）
    m_field14D0 = 0;                                 // +0x14d0 movb（pad）
}