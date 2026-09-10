void CStackableItem::set_item(STStackableScript& script)
{
    char buf[500];
    unsigned long seed = time(0) + script.m_field0;
    m_pRand = new CMTRand(seed);
    m_index = script.m_field0;
    m_grade = script.m_field4;
    m_field0c = script.m_field14;
    m_field10 = script.m_field18;
    m_field14 = script.m_field1c;
    m_field30 = script.m_field24;
    m_rarity = script.m_field10;
    m_char60 = script.m_fieldfc;
    if (script.m_field30)
    {
        G_CDataManager()->set_AuctionRegFreeCeraItem(m_index);
    }
    __asm__ __volatile__(
        "movl $0, %%eax\n\t"
        "movl $0x7d, %%edx\n\t"
        "movl %0, %%edi\n\t"
        "movl %%edx, %%ecx\n\t"
        "rep stosl\n\t"
        : : "b"(buf) : "eax", "edx", "ecx", "edi", "memory");
    strcpy(buf, script.m_strac.c_str());
    if (!CodePage::script2Database((char*)script.m_strac.c_str(), buf))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x80, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script.m_strac.c_str(),
            "void CStackableItem::set_item(STStackableScript&)", 0x80);
    }
    m_str2c = buf;
    m_usablePeriod = script.m_fieldf4;
    m_expirationDate = script.m_fieldf8;
    m_field64 = script.m_field20;
    m_field68 = script.m_field4ec;
    m_int2cc = script.m_field18c;
    m_pair24.first = script.m_field28;
    m_pair24.second = script.m_field2c;
    m_field50 = script.m_fieldc;
    m_attachType = script.m_field8;
    m_nItemType = script.m_field1e0;
    m_int2b8 = script.m_field3cc;
    if (m_nItemType == 3)
    {
        createRecipeInfo(&m_recipe174, &script.m_vec88, 0);
        m_recipe174.m_vec28 = script.m_vec3c0;
        m_recipe174.additionalInfo(script);
    }
    if (m_nItemType == 6)
    {
        createLotteryInfo(&m_lotto, &script.m_vec88);
    }
    else if (m_nItemType == 7)
    {
        createStackableLotteryInfo(&m_lotto, &script.m_vec88);
    }
    else if (m_nItemType == 0xd)
    {
        createStackableLotteryInfo(&m_lotto, &script.m_vec88);
    }
    else if (m_nItemType == 0x1a || m_nItemType == 0x1b)
    {
        createStackableMultiBoxLotteryInfo(
            &m_multiBox, &script.m_vec244, &script.m_vec250,
            &script.m_vec25c);
    }
    else if (m_nItemType == 0xe || m_nItemType == 0x26)
    {
        m_pkg21c = script.m_vec268;
        m_pkg234 = script.m_vec274;
        if (!script.m_vec2a4.empty())
        {
            m_pkg228 = script.m_vec2a4;
            script.m_vec2a4.clear();
        }
    }
    else if (m_nItemType == 0xf || m_nItemType == 0x10)
    {
        m_booster = script.m_booster;
    }
    else if (m_nItemType == 0x11)
    {
        m_booster = script.m_booster;
        m_nItemType = 0x10;
    }
    else if (m_nItemType == 0x22)
    {
        m_booster = script.m_booster;
        m_boosterSelect = script.m_boosterSelect;
    }
    else if (m_nItemType == 0x17)
    {
        createGlobalEffectInfo(m_vec280, &script.m_vec88);
    }
    else if (m_nItemType == 0x1d)
    {
        m_limitCube.m_materials = script.m_vec3d4;
        m_limitCube.m_2ndMaterialIdx = script.m_field3e0;
        m_limitCube.m_2ndMaterialCount = script.m_field3e4;
        m_limitCube.m_results = script.m_vec3e8;
    }
    for (int i = 0; i <= 10; ++i)
    {
        m_jobUsable[i] = script.m_usable[i];
    }
    m_usableLevel = script.m_field5c;
    m_field4c = script.m_field60;
    m_stackable = 1;
    m_int28c = script.m_field364;
    m_int2bc = script.m_field1e4;
    m_usableTypes.clear();
    if (m_nItemType == 2 && m_int2bc == 1)
    {
        if (script.m_vec94.size() > 1)
        {
            // [b28d-solo-CStackableItem] ORIG 循环尾 `call size; cmp -0x28(%ebp),%eax;
            // seta %al`（dst=%eax=size，无符号 above）⇒ 源码按 size() 在左的比较方向写。
            for (unsigned int i = 1; script.m_vec94.size() > i; ++i)
            {
                int t = GetEquipmentTypeStringToEnum(script.m_vec94[i]);
                if (t != 0x16)
                {
                    m_usableTypes.push_back((int)t);
                }
            }
        }
    }
    m_int2d0 = script.m_field68;
    m_int2d4 = script.m_field1b4;
    m_option2d8.m_field0 = script.m_field1b9;
    m_option2d8.m_field1 = script.m_field1ba;
    m_option2d8.m_field2 = script.m_field1bb;
    m_stat2dc.m_flagLo = script.m_field360;
    m_stat2dc.sec = script.m_field358;
    m_stat2dc.m_flagHi = script.m_field1bf;
    m_option2e4.m_field0 = script.m_field1bc;
    m_option2e4.m_field1 = script.m_field1bd;
    m_option2e4.m_field2 = script.m_field1be;
    m_char2e7 = script.m_field1c0;
    m_int2e8 = script.m_field1c4;
    m_char2ec = script.m_field1d0;
    m_jewelTargetSocket = script.m_field380;
    m_itemGroupName = script.m_field100;
    m_char78 = script.m_field104;
    m_int58 = -1;
    m_bool79 = 0;
    m_int80 = script.m_fieldc8;
    m_int7c = script.m_fieldcc;
    m_impossibleGift = script.m_fieldd0;
    m_strb0 = script.m_str6c;
    m_drop = script.m_specialDrop;
    script.m_specialDrop.clear();
    m_char2f0 = script.m_field10e;
    m_short2ee = script.m_field394;
    m_enchantMap = script.m_map3f4;
    m_char30c = script.m_field40c;
    memcpy(m_chars30d, script.m_fields40d, 2);
    m_char30f = script.m_field396;
    m_short310 = script.m_field398;
    m_char312 = script.m_field39a;
    m_char313 = script.m_field39b;
    m_short314 = (unsigned short)script.m_field39c;
    m_char316 = script.m_field3a0;
    m_char320 = script.m_field438;
    if (m_char320)
    {
        m_int318 = script.m_field43c;
        m_int31c = script.m_field440;
    }
    m_isRoutingItem = script.m_field10f;
    m_vecC0 = script.m_vec110;
    m_questBonus = 0;
    m_charD0 = script.m_field3d0;
    m_char321 = script.m_field3d1;
    m_setd4 = script.m_setdc;
    m_intEC = script.m_field64;
    m_intF0 = script.m_field444;
    m_intF4 = script.m_field448;
    m_intFC = script.m_field150;
    m_int100 = script.m_field154;
    m_vec104.clear();
    m_vec104 = script.m_vec158;
    m_int324 = script.m_field44c;
    m_int328 = script.m_field454;
    m_int32c = script.m_field450;
    m_int330 = script.m_field458;
    m_int334 = script.m_field45c;
    m_int338 = script.m_field460;
    m_int33c = script.m_field464;
    m_set128 = script.m_set11c;
    if (script.m_field474 != 0)
    {
        G_CDataManager()->set_lottery_use_cost(script.m_field0, script.m_field474);
    }
    m_int340 = script.m_field478;
    m_char344 = script.m_field3a1;
    m_amplifyMap = script.m_map3a4;
    m_arad148.m_field0 = script.m_field178;
    m_arad148.m_field4 = script.m_field17c;
    m_arad148.m_str8 = script.m_str180;
    m_arad148.m_strC = script.m_str184;
    m_arad148.m_str10 = script.m_str188;
    if (!script.m_vec19c.empty())
    {
        m_arad148.m_vec20.reserve(script.m_vec19c.size());
        std::copy(script.m_vec19c.begin(), script.m_vec19c.end(),
                  std::back_inserter(m_arad148.m_vec20));
    }
    if (!script.m_vec190.empty())
    {
        m_arad148.m_vec14.reserve(script.m_vec190.size());
        std::copy(script.m_vec190.begin(), script.m_vec190.end(),
                  std::back_inserter(m_arad148.m_vec14));
    }
    m_int360 = script.m_field418;
    m_char364 = script.m_field32c;
    // [b28d-solo-CStackableItem] ORIG `movzwl 0x47c(%edx),%ecx; mov %cx,0x365(%eax)`
    // 为 2 字节 disp 直存；原 `reinterpret_cast<unsigned short*>` 走 address-of +
    // 间接寻址，多出 `add $0x365,%eax` 与 mov 往返（+3 条）。改用 CStackableItem.h
    // 新增的 16 位具名视角（union 同址零布局，pack 保 sizeof 3）。
    m_avatar365.m_jobType16 = script.m_field47c;
    m_avatar365.m_field2 = script.m_field47e;
    m_set368 = script.m_set480;
    m_set380 = script.m_set498;
    m_avatarOptionChange = script.m_field4b0;
    m_int39c = (StackableAction::Type)script.m_field1e8;  // [b28d-N254] enum 字段
    m_vec3a0 = script.m_vec1ec;
    m_usablePlaces = script.m_vec1f8;
    if (m_int39c == 0x2c)
    {
        for (unsigned int i = 0; i < m_vec3a0.size(); ++i)
        {
            void* emo = ((CChattingEmoticonList*)
                ((CDataManagerView*)G_CDataManager())->m_field28)
                ->find_emoticon(m_vec3a0[i]);
            // [b28d-solo-CStackableItem] ORIG 0x0850e7xx 段：函数指针读入 %edx 后
            // 直接 `call *%edx`，**没有**具名 fn 局部（无槽往返，P0）；写成
            // `EmoticonFn fn = ...; fn(...)` 会多出 fn 槽 +2 条往返。
            typedef void (*EmoticonFn)(void*, int);
            (*(EmoticonFn*)(*(char**)emo + 0x18))(emo, m_index);
        }
    }
    m_usableServers = script.m_vec214;
    m_char3c5 = script.m_field205;
    m_impossibleCharacCargo = script.m_fieldda;
    m_increaseStatusType = script.m_field220;
    m_vec90 = script.m_vec224;
    m_dyeInfo9c = script.m_dye164;
    m_randomOptionUnseal = script.m_field4b8 != 0;
    m_char144 = script.m_fieldd8;
    m_parentBoosterItemId = 0;
    m_int3d0 = script.m_field468;
    m_int3c8 = script.m_field46c;
    if (m_int3c8 > 1)
    {
        m_booster = script.m_booster;
    }
    m_int3cc = script.m_field1ac;
    m_tradeLimitCount = script.m_field508;
    m_set3d8 = script.m_set50c;
