// [b28d-solo-WarField] 语义完成、byte 层 N 封顶（605/606，p167 CLEAR 94/94）：
// ①P235 具名字节视图（refData.m_field38/39/3c/3c_byte/3d/3e_byte/40_byte/41/43/44_byte）
// 消 29 条 lea+add（裸 *(char*)& 取址）；②P68/P0 删具名 buf 引用，内联
// ((InterfacePacketBuf*)&packet)->（ORIG 每调用点重载 0x10(%ebp)，无 buf 槽）。
// 残差：整体槽差 4（ORIG userUid=-0x40/refData=-0x110/map_item=-0x164 vs 我们 -0x3c/-0x10c/-0x160，
// 帧同为 0x1bc ⇒ N82 槽分配）；list 迭代器 end() 布局（ORIG 循环外预求值+jmp cond，4.4 旋转到
// 体尾 ⇒ N86/P164）；m_field11 逐字节取址 add $0x11（需 map_monster 匿名 union 字节视图，跨泳道 .h）；
// 2 处寄存器名（%edx vs %eax ⇒ N7 族）+ 尾部对齐 nop（§6.1 豁免）。@0x086b9672
// [M3S1 2026-09-11 落地·P183/P0] `refData.m_field39/m_field3d` 由 `*((char*)&m_field11±)`
//   改为 map_monster 匿名 union 字节视图 `m_field11b.m_field11b1/b0` ⇒ 消掉
//   `add $0x11; add $0x1; movzbl` 三条链，复现 ORIG 单条 `movzbl 0x12(%eax)`。
//   读数 606→**603**（gap −1 → +2）、OURS-only 清空（原 {'add':3}），四数 66/88/1/3 不劣。
//   残余：ORIG-only={'mov':2}（086b98xx 区 ORIG 每使用点重装 `mov -0x34(%ebp),%eax`，
//   我方因字节视图后 %eax 复用少 1-2 条）⇒ 方向为「我方少」，消指令旋钮不适用，判 N86/N7 族。
bool WarField::HandleMonsterKill(int uid, PacketGuard& packet, map_monster& mob,
                                 CUser* user, const CDungeon* dungeon)
{
    int userUid = 0xffff;
    int moneyRatio = 0;
    if (user != 0)
    {
        userUid = (unsigned short)user->get_unique_id();
        int moneyLimit = (int)((const CDataManager*)G_CDataManager())
                             ->GetMoneyLimitPerLevel(user->get_charac_level(),
                                                     user->get_acc_name());
        if (moneyLimit < 1)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x178, 5);
            trace("WarField::HandleMonsterKill limitMoney(%d) is under 0", moneyLimit);
        }
        else
        {
            moneyRatio = (user->getCurCharacMoney() * 100) / moneyLimit;
        }
    }
    MapInfo* mi = &m_mapInfo;
    std::map<int, map_monster>::iterator it = mi->m_monsterMap.find(uid);
    if (it == mi->m_monsterMap.end())
    {
        return 0;
    }
    map_monster* fieldMob = &it->second;
    CMonster* monster = (CMonster*)((const CDataManager*)G_CDataManager())
                            ->find_monster(fieldMob->m_mobIndex);
    if (monster == 0)
    {
        LogManager::logFormat(
            1, "WarField.cpp",
            __PRETTY_FUNCTION__,
            0x18c, "G_CDataManager()->find_monster(%u) fail", fieldMob->m_mobIndex);
        return 0;
    }
    mob = *fieldMob;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x26);
    ((InterfacePacketBuf*)&packet)->put_short(uid);
    std::list<map_item> itemList;
    int curSec = GlobalData::s_systemTime_.getCurSec();
    stGenerateRefData_t refData;
    if (user != 0)
    {
        refData.m_dropRate = 100;
        if (ARAD::Singleton<ServiceRestrictManager>::Get()
                ->isRestricted(RestrictCategory::Enum_3, 1))
        {
            refData.m_dropRate = user->getStdDropRate();
        }
    }
    // 掉落上下文区（宽度逐字段 = HandleMonsterKill 086b98b1-0x298 汇编：movb/movl）。
    refData.m_field34 = fieldMob->m_level;               // movb +0x34
    refData.m_field35 = fieldMob->m_roleType;            // movb +0x35
    refData.m_rateSlot = fieldMob->m_level;              // movb +0x36
    refData.m_pad37 = 0;                                 // movb +0x37
    refData.m_field38 = 1;                               // movb +0x38（int 槽低字节）
    // [M3S1 本波实测·P183/P0] ORIG 086b98xx 为单条 `movzbl 0x12(%eax)`（disp 折叠）
    //   ⇒ 用 map_monster 匿名 union 的 `m_field11b1` 字节视图（unsigned char），
    //   原 `*((char*)&m_field11 + 1)` 产 `add $0x11; add $0x1; movzbl`（我方多 1 条 add）。
    refData.m_field39 = fieldMob->m_field11b.m_field11b1;    // movb +0x39（mob +0x12）
    refData.m_field3c = moneyRatio;                      // movl +0x3c..0x3f
    refData.m_field40_byte = 1;                          // movb +0x40（指针槽低字节）
    refData.m_field41 = 0;                               // movb +0x41
    stGenerateResult_t result;
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_0)
        ->generateRandomItem(refData, result);
    // 第二段（specific）：+0x38 movl、+0x3c/0x3d/0x35/0x36/0x3e/0x40/0x41/0x43/0x44 movb
    refData.m_rankBonusIndex = fieldMob->m_mobIndex;     // movl +0x38
    refData.m_field3c_byte = fieldMob->m_roleType;       // movb +0x3c
    refData.m_field3d = fieldMob->m_field11b.m_field11b0;  // movb +0x3d（mob +0x11）
    refData.m_rateSlot = 0;                              // movb +0x36
    refData.m_field3e_byte = 1;                          // movb +0x3e
    refData.m_field35 = fieldMob->m_level;               // movb +0x35
    refData.m_field40_byte = 0;                          // movb +0x40
    refData.m_field41 = 0;                               // movb +0x41
    refData.m_field43 = 0;                               // movb +0x43
    refData.m_field44_byte = 0;                          // movb +0x44
    if (user != 0 && user->isGMUser())
    {
        refData.m_field24 = user->getForceDropFlag();
    }
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_0)
        ->generateSpecificItem(refData, result);
    dungeon->getDropItems();
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_4)
        ->generateSpecificItem(refData, result);
    refData.m_field34 = 1;                               // movb +0x34
    refData.m_field35 = fieldMob->m_level;               // movb +0x35
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_3)
        ->generateRandomItem(refData, result);
    for (std::vector<Inven_Item>::iterator itemIt = result.m_dropItems.begin();
         itemIt != result.m_dropItems.end(); ++itemIt)
    {
        map_item item;
        item.m_count = 1;
        item.m_itemIndex = mi->m_dropItemCnt;
        mi->m_dropItemCnt = mi->m_dropItemCnt + 1;
        item.m_dropIndex = userUid;
        item.m_createTick = curSec;
        item.m_item = *itemIt;
        itemList.push_back(item);
        mi->Add_Item(item);
    }
    ((InterfacePacketBuf*)&packet)->put_byte((int)itemList.size());
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    for (std::list<map_item>::iterator itemIt = itemList.begin();
         itemIt != itemList.end(); itemIt++)
    {
        map_item& mi = *itemIt;
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_itemIndex);
        ((InterfacePacketBuf*)&packet)->put_int(mi.m_item.m_addInfo);
        ((InterfacePacketBuf*)&packet)->put_byte((int)mi.m_item.GetItemAttr());
        if (mi.m_item.isAvatarItemType())
        {
            CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(mi.m_item.m_addInfo);
            if (equip == 0)
            {
                ((InterfacePacketBuf*)&packet)->put_int(mi.m_item.get_add_info());
            }
            else
            {
                ((InterfacePacketBuf*)&packet)->put_int(
                    equip->getAvatarPeriod((unsigned char)mi.m_item.m_endurance) * 0x15180);
                emblem.init();
                equip->getAvatarSocket(mi.m_item.m_endurance, emblem);
            }
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_int(mi.m_item.get_add_info());
        }
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_item.m_endurance);
        ((InterfacePacketBuf*)&packet)->put_byte((int)mi.m_item.m_amp.getAbilityType());
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_item.m_amp.getAbilityValue());
        ((InterfacePacketBuf*)&packet)->put_packet(mi.m_item);
        if (mi.m_item.isAvatarItemType())
        {
            ((InterfacePacketBuf*)&packet)->put_byte(1);
            ((InterfacePacketBuf*)&packet)->put_int(0x1e);
            ((InterfacePacketBuf*)&packet)->put_binary((const char*)&emblem, 0x1e);
            stAvatarExpansionInfo_t expansion;
            expansion.init();
            ((InterfacePacketBuf*)&packet)->put_int(4);
            ((InterfacePacketBuf*)&packet)->put_binary((const char*)&expansion, 4);
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_byte(0);
        }
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_dropIndex);
    }
    mi->m_monsterMap.erase(it);
    return 1;
}