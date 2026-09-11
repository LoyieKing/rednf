// [b28d-solo-CEquipItem] ORIG 0x0850fd84（669=669 AE；p167 32/32 CLEAR）修复要点：
//   ① 真语义 bug：`(T*)0x941f750` 把全局变量**地址**当对象指针 ⇒ 改取
//      `GlobalData::s_pCompoundBindCube` 的**指针值**（ORIG `mov 0x941f750,%eax`，P237）；
//   ② bool 族源字段化（P136/P300）：STItemScript/STEquipmentScript 的 11 个 char 字段
//      （m_usable[0xb]、m_fieldd0/d9/da/10f、m_6f4/5/6、m_7ac/7bc/888）改 bool 后直存；
//   ③ cMyTrace 全表达式临时链（P227）复现 ctor 前右起求 varargs + 跨 ctor 保活；
//   ④ m_field1d4 包匿名 union float 视图复现单条位拷贝（N107 取址形恒 add+mov）；
//   ⑤ Wp/ExpBonus 类型统一（stBonusPointData_t 副本删除，§4.4）+ trivial op= 整体赋值
//      复现 P246 的 dest 缓存 %ecx 块拷贝；
//   ⑥ m_880 char→unsigned char 复现两段零扩展装载；
//   ⑦ m_experienceAmount float 视图（替旧 `(int)` 转换 7 条）、m_compound110/
//      m_additionalExp120 结构整体赋值（P246）、m_auraOptionType 直存。
void CEquipItem::set_item(STEquipmentScript& script)
{
    m_index = script.m_field0;
    m_grade = script.m_field4;
    m_field0c = script.m_field14;
    m_field30 = script.m_field24;
    m_field10 = script.m_field18;
    m_field14 = script.m_field1c;
    m_rarity = script.m_field10;
    m_char60 = script.m_fieldfc;
    m_pair24.first = script.m_pair28.first;
    m_pair24.second = script.m_pair28.second;
    if (script.m_field30)
    {
        G_CDataManager()->set_AuctionRegFreeCeraItem(m_index);
    }
    m_boolF8 = script.m_field31;
    char buf[500] = {0};
    strcpy(buf, script.m_strac.c_str());
    if (!CodePage::script2Database((char*)script.m_strac.c_str(), buf))
    {
        // [b28d-solo-CEquipItem] ORIG 在 ctor 之前右起求 varargs 并跨 ctor 保活
        // （edi/esi/ebx）⇒ P227 全表达式临时链形（具名 trace 会先构造后求值）。
        cMyTrace(__PRETTY_FUNCTION__, 0x553, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script.m_strac.c_str(),
            __PRETTY_FUNCTION__, 0x553);
    }
    m_str2c = buf;
    m_attachType = script.m_field8;
    m_field50 = script.m_fieldc;
    m_field64 = script.m_field20;
    m_field68 = script.m_88c;
    for (int i = 0; i <= 10; ++i)
    {
        m_jobUsable[i] = script.m_usable[i];
    }
    m_usableLevel = script.m_field5c;
    m_field4c = script.m_field60;
    m_itemType = script.m_field1b4;
    m_randomTable.m_endurance = script.m_1bc;
    m_stackable = 0;
    m_int58 = script.m_184;
    m_int5c = script.m_180;
    // [b28d-solo-CEquipItem] ORIG `mov 0x750(%eax),%edx` 单条位拷贝 ⇒ 走 +0x10 float 视图
    // （float=float 直搬），旧 `(int)` 转换形多 flds/fnstcw/fistpl 七条。
    m_randomTable.m_experienceAmount = script.m_artifact738.m_expRate;
    m_randomTable.m_field14 = script.m_6bc;
    m_randomTable.m_field20 = script.m_artifact738.m_minLevel;
    if (script.m_field1b4 == 0x16)
    {
        switch (script.m_1b8)
        {
        case 0:
            user_creature::GetInstanceCreatureScriptMgr()
                ->RegisterCreatureIdAndItemId(m_randomTable.m_field14, m_index);
            break;
        case 1:
            m_randomTable.m_field1c = script.m_764;
            break;
        case 2:
            m_randomTable.Set(&script.m_vec88);
            break;
        }
    }
    m_randomTable.m_field18 = script.m_1b8;
    m_usablePeriod = script.m_fieldf4;
    m_expirationDate = script.m_fieldf8;
    m_randomTable.m_field24 = script.m_7ac;
    m_avatarSelect = script.m_avatar6c0;
    m_vec180 = script.m_vec6cc;
    m_field18c = script.m_6d8;
    m_repairCost = script.m_178;
    m_itemGroupName = script.m_field100;
    m_char78 = script.m_field104;
    m_bool79 = script.m_7bc;
    m_strb0 = script.m_str6c;
    m_int80 = script.m_fieldc8;
    m_int7c = script.m_fieldcc;
    m_impossibleUpgrade = script.m_6f4;
    m_impossibleAmplify = script.m_6f5;
    m_impossibleDisjoint = script.m_6f6;
    m_impossibleGift = script.m_fieldd0;
    m_impossibleCharacCargo = script.m_fieldda;
    m_possibleKiriProtection = script.m_fieldd9;
    m_periodAfterUnsealing = script.m_6f8;
    m_drop = script.m_specialDrop;
    script.m_specialDrop.clear();
    m_char194 = script.m_field10e;
    m_vec198 = script.m_vec7d8;
    m_vec1a4 = script.m_vec7e4;
    m_enchantVec = script.m_enchant7cc;
    m_field1bc = 0;
    if (IsAvatarItem())
    {
        if (m_grade == 3 && script.m_798 == 3)
        {
            m_field1bc = 6;
        }
        setAvatarOptionSize((int)script.m_param1c0.m_avatar264.size());
    }
    else if (GetItemType() == 0x16)
    {
        if (script.m_field10 == 2)
        {
            m_field1bc = 10;
        }
    }
    else if (script.m_field10 == 2)
    {
        if (script.m_fieldc4 == 5)
        {
            m_field1bc = 2;
        }
        else if (script.m_fieldc4 == 0x1e)
        {
            m_field1bc = 1;
        }
        else
        {
            m_field1bc = 9;
        }
    }
    else if (script.m_field10 == 3)
    {
        m_field1bc = 3;
    }
    else if (script.m_field10 == 5)
    {
        m_field1bc = 4;
    }
    else if (script.m_field10 == 4)
    {
        m_field1bc = 5;
    }
    else if (script.m_field10 == 0)
    {
        m_field1bc = 7;
    }
    else if (script.m_field10 == 1)
    {
        m_field1bc = 8;
    }
    m_isRoutingItem = script.m_field10f;
    m_vecC0 = script.m_vec110;
    m_questBonus = script.m_810;
    m_field1c0 = script.m_17c;
    if (script.m_814 != 2)
    {
        // [b28d-solo-CEquipItem] ORIG `mov 0x941f750,%eax; mov %eax,(%esp)` = 取
        // GlobalData::s_pCompoundBindCube **指针值**（+0x941f750 存的是 void*）；
        // 旧硬编码 `(T*)0x941f750` 直传变量地址 = 真语义 bug（P237）。
        ((ExtreamDungeon::CCompound_ExtreamDun_Item*)GlobalData::s_pCompoundBindCube)
            ->ForceResultItemRule_Setting(m_index, script.m_814 != 0,
                                          script.m_818);
    }
    m_setd4 = script.m_setdc;
    m_intEC = script.m_field64;
    m_avatarSelectIdx = script.m_6e0;
    m_randomOption = script.m_888;
    m_randomGrade = script.m_889;
    m_floatFC = script.m_expertJob.m_field18;
    m_float100 = script.m_expertJob.m_field1c;
    m_vec104.clear();
    m_vec104 = script.m_expertJob.m_usable;
    // [b28d-solo-CEquipItem] ORIG dest 缓存 %ecx + 两读交错 = 8 字节结构整体赋值（P246）。
    m_compound110 = script.m_expertJob.m_variation;
    m_float118 = script.m_expertJob.m_field10;
    m_float11c = script.m_expertJob.m_field14;
    m_additionalExp120 = script.m_expertJob.m_additionalExp;
    m_set128 = script.m_set11c;
    // ORIG set_item @ 0x850fd84 +0x8a1..+0x8f9 reads STEquipmentScript
    // +0x604/+0x608/+0x60c, i.e. EquipmentParameterInfo
    // +0x444/+0x448/+0x44c.  The middle field is rounded by the x87
    // fistp sequence; the final field is copied as one dword by ORIG.
    m_field1cc = script.m_param1c0.m_upgradeCostDiscount;
    m_field1d0 = static_cast<int>(script.m_param1c0.m_upgradeProbIncrease);
    // [b28d-solo-CEquipItem] ORIG 单条 `mov 0x60c(%eax),%edx` 位拷贝 ⇒ float 视图直搬。
    m_assaultCostDiscount = script.m_param1c0.m_assaultCostDiscount;
    // [b28d-solo-CEquipItem] ORIG 两段装载 `movzbl 0x880(%eax),%eax; mov %eax,%edx`
    //（源字段 unsigned char 的零扩展 + 目标 char 转换中转）。
    m_auraOptionType = script.m_880;
    m_auraOptionValue = script.m_884;
    {
        // ORIG 依字节偏移读 STEquipmentScript +0x81c/+0x820/+0x824/+0x828
        //（+0x81c/+0x820=wpBonus、+0x824/+0x828=expBonus，具名成员见 STEquipmentScript.h）。
        // [b28d-solo-CEquipItem] ORIG dest 缓存 %ecx + 两读交错 = 8 字节整体赋值（P246）；
        // 类型统一后 trivial op= 直接块拷贝，不发射符号。
        m_wpBonus1e0 = script.m_wpBonus81c;
        m_expBonus1e8 = script.m_expBonus824;
    }
    m_avatarAbility = script.m_param1c0.m_avatar264;
    if (script.m_890 == 1)
    {
        m_pollutionFlag = 0;
    }
    else
    {
        m_pollutionFlag = 1;
    }
    m_upgradeLevel = script.m_upgrade6fc;
    m_avatarColor[0] = script.m_88a;
    m_avatarColor[1] = script.m_88b;
    // ORIG set_item @ 0x850fd84 +0x9fc..+0xa62 copies the
    // [level linear ability] values in this order: level bounds, grade
    // bounds, then repair bounds.  These names preserve the nested layout.
    m_growthFlag = script.m_69c;
    m_growthA = script.m_param1c0.m_growthLevelBegin;
    m_growthB = script.m_param1c0.m_growthLevelEnd;
    m_growthC = script.m_param1c0.m_growthGradeBegin;
    m_growthD = script.m_param1c0.m_growthGradeEnd;
    m_growthE = script.m_param1c0.m_growthRepairBegin;
    m_growthF = script.m_param1c0.m_growthRepairEnd;
    // [b28e 必配对律第 3 点 · 目标字段 ↔ 写入源字段同型化] ORIG 0x085107ef
    // `movzbl 0xd8(%eax),%edx` → 0x085107f9 `mov %dl,0x144(%eax)`（**无 setne**）。
    // 目标 `CItem::m_char144` 改 bool（E01）后源 `STItemScript::m_fieldd8` 仍 char
    // ⇒ 4.4 插 `movzbl; test %al,%al; setne %dl; mov %dl,0x144`（+2）致本件 AE→DIFF
    // （191/375/1/2 → 191/374/1/3）；源端改 bool（C04 落 STItemScript.h，权威头与
    // CStackableItem.cpp:792 本地视图统一）后写点复现 ORIG 直存 ⇒ **恢复 191/375/1/2**。
    // 边界（供判词模板）：源端「本地镜像视图已 bool 而权威头仍 char」时无 codegen 后果
    // （该 TU 直存），但有 §4.4 重复定义/ODR 隐患 ⇒ 仍须成对统一。
    m_char144 = script.m_fieldd8;
    m_parentBoosterItemId = 0;
}