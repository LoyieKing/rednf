// ---------------------------------------------------------------------------
// 终判（b28d-solo-CUserPremium，b28d 复测）：P167 CLEAR（20/20），语义完成。
// 本波按 ORIG 直证改写（clean/AE ndiff 466→56）：
//  ①`size() != 0` 正向判 + 非空分支直落/空分支置后（ORIG 0x86af571
//    `test;setne;test;je <空分支>`；旧 `==0` 产 sete 且块序相反）；
//  ②for 初始化处直构 it/itEnd 并缓存 end()（ORIG 0x86af5a2/0x86af5ce 各一次、
//    it=-0x44/end=-0x48；旧形每轮重调 end() 且槽错位）；
//  ③两处 SetPremiumItemData 调用点改 bool 裸 `!`（ORIG 0x86af61d/0x86af679
//    `call;xor $1;test %al;je`；返回类型经 Main 代落 CUser.h int→bool，T3 直证）；
//  ④外层循环变量 i 提函数头（P212，复现 ORIG i=-0x34）。
// 残差 56 行 = ORIG `sub $0x7d50/$0x7d10 + disp 0x10` vs 4.4 `sub $0x7d40/
// $0x7d00 + disp 0` 的常量折叠基准差（N1/N4 族，等效地址）+ 键值局部取舍：
// ORIG 0x86af54c 确有 key 栈槽往返（P0），但 4.4 为它分配栈槽会触发整帧
// 0x7e3c→0x7e4c / table 基址 -0x7e28→-0x7e2c 平移（探针 /tmp/b28d_solo：
// 带槽 466、内联实参 56、纯 register 52）⇒ 取内联实参形（语义一致，仅少
// store/load 对）⇒ N82/N95/N1 封顶。
// ORIG T 0x086aeca4 WongWork::CUserPremium::LoadPremiumInfo(char const*)。
// importPremiumList 失败→0x15c；逐记录拷贝进 g_SPremiumInfo[i]（0x130→0x120
// 字段重排，+0xc8 起写穿 _Additioal_info 子对象区）；SetPremiumItemData
// 失败：非空 vector 分支→0x1bc、空分支（key=m_targetItemKey，flag 取
// m_checkTerm 低字节）→0x1c4。全部成功返回 0（CDataManager::init 以
// "LoadPremiumInfo Fail(Line: %d)" 打印该行码）。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserPremium] 修法（P167/P96/P50）：ORIG 0x086aeca4 调非 const
// 迭代器族（ptEv/ppEv/ne(N,N)）且无 `iterator::C1Ev`（P167 EXTRA×1）⇒ 非
// const iterator 且 for 初始化处直构；改后差集清零（20/20 CLEAR）。
// 终判（b28d）：同上——删 key 局部后 gap -3/758；残余帧差与槽基差（N82/N95/
// P232）封顶。
// [L3CUserPremDBInven 本波复核（ORIG 0x086aeca4）] 复算 758/760、diff 仅 5 块：
//   ① 三处 `short` 数组取址（ORIG `movzwl -0x7e20(%ebp,%eax,2)` 单条 base+idx*2
//      vs 4.4 `lea -0x7e28; imul $0x130; add $0xd0; lea 0x8` 常量链，等效地址差
//      = N1/N4 折叠基准）；② `table[i].m_targetItemKey`（ORIG 0x86af54c 有 key
//      栈槽往返、我方内联实参 = P0 取舍，带槽实测触发整帧 0x7e3c→0x7e4c 平移）。
//   ⇒ 无新旋钮，维持 N82/N95/P232 封顶。
int CUserPremium::LoadPremiumInfo(const char* path)
{
    stPremiumTable_t table[0x6a];
    // [b28d-solo-CUserPremium] P212：ORIG 0x086aed54 起外层循环变量 i 占 -0x34
    // （4.4 若留在 for-init 会被同块 key 顶到 -0x38）⇒ 提函数头声明、循环内仅赋值。
    int i;

    if (importPremiumList(path,
                          table,
                          g_vecPremiumOverEquipableInfo,
                          g_vecPremiumReturnItemInfo) != 0)
        return 0x15c;

    for (i = 0; i <= 0x69; ++i)
    {
        // ORIG 无任何引用缓存：每个字段读写都重算 table+i*0x130+off 与
        // g_SPremiumInfo+i*0x120+off（Ghidra C 每条赋值两侧都带 local_38*0x130 /
        // local_38*0x120 的完整下标式），且四抗性走 j=0..3 循环而非四条赋值。
        g_SPremiumInfo[i].m_kind = table[i].m_kind;                 // +0x00
        g_SPremiumInfo[i].m_checkTerm = table[i].m_checkTerm;       // +0x04
        g_SPremiumInfo[i].m_timeLimit = table[i].m_expireTime;      // +0x08
        for (int j = 0; j < 5; ++j)                     // +0x0c 特权物品对
        {
            g_SPremiumInfo[i].m_advantageItems[j].first = table[i].m_advantageItems[j].first;
            g_SPremiumInfo[i].m_advantageItems[j].second = table[i].m_advantageItems[j].second;
        }
        g_SPremiumInfo[i].m_fatigueRate = table[i].m_fatigueRate;         // +0x34
        g_SPremiumInfo[i].m_premiumCount = table[i].m_coin;               // +0x38
        g_SPremiumInfo[i].m_expRate = table[i].m_expRate;                 // +0x3c
        // [b28d-solo-CUserPremium] 顺序照 ORIG g 表 store 序列（0x00/0x04/0x08/
        // 0x0c/0x34/0x38/0x3c/0x58/0x5a/0x5c/0x40/0x44/0x48/0x60/0x64）。
        g_SPremiumInfo[i].m_overSkillLevel = table[i].m_overSkillLevel;   // +0x58 ← t+0x42
        g_SPremiumInfo[i].m_field5a = table[i].m_cerashopDiscountRate;    // +0x5a ← t+0x44
        g_SPremiumInfo[i].m_useTargetServer = table[i].m_useTargetServer; // +0x5c
        g_SPremiumInfo[i].m_needDailyCheck = table[i].m_unlimitFatigue;   // +0x40
        g_SPremiumInfo[i].m_field44 = table[i].m_bonusExp;                // +0x44
        g_SPremiumInfo[i].m_field48 = table[i].m_questItemDropRate;       // +0x48
        g_SPremiumInfo[i].m_independentDropRates = table[i].m_independentDropRates;  // +0x4c
        g_SPremiumInfo[i].m_overEquipFlag = table[i].m_goldBonus;         // +0x60
        g_SPremiumInfo[i].m_field64 = table[i].m_overEquipCount;          // +0x64
        for (int j = 0; j < 5; ++j)                     // +0x68 返还物品对
        {
            g_SPremiumInfo[i].m_returnPairs68[j].first = table[i].m_overEquipPairs[j].first;
            g_SPremiumInfo[i].m_returnPairs68[j].second = table[i].m_overEquipPairs[j].second;
        }
        g_SPremiumInfo[i].m_field90 = table[i].m_returnItemCount;         // +0x90
        for (int j = 0; j < 3; ++j)                     // +0x94 返还对
        {
            g_SPremiumInfo[i].m_returnPairs94[j].first = table[i].m_returnItemPairs[j].first;
            g_SPremiumInfo[i].m_returnPairs94[j].second = table[i].m_returnItemPairs[j].second;
        }
        for (int j = 0; j < 7; ++j)                     // +0xac yday 掩码
            g_SPremiumInfo[i].m_applyDayMask[j] = table[i].m_applyDayMask[j];

        g_SPremiumInfo[i].m_additionalInfo.m_field00 = table[i].m_hpMax;
        g_SPremiumInfo[i].m_additionalInfo.m_field04 = table[i].m_mpMax;
        g_SPremiumInfo[i].m_additionalInfo.m_field08 = table[i].m_physicalAttack;
        g_SPremiumInfo[i].m_additionalInfo.m_field0a = table[i].m_physicalDefense;
        g_SPremiumInfo[i].m_additionalInfo.m_field0c = table[i].m_magicalAttack;
        g_SPremiumInfo[i].m_additionalInfo.m_field0e = table[i].m_magicalDefense;
        for (int j = 0; j < 4; ++j)                     // +0xd8 四抗性（ORIG 为循环）
            g_SPremiumInfo[i].m_additionalInfo.m_field10[j] =
                (&table[i].m_fireResistance)[j];
        for (int j = 0; j < 17; ++j)
            g_SPremiumInfo[i].m_additionalInfo.m_field18[j] = table[i].m_rawE0[j];
        g_SPremiumInfo[i].m_additionalInfo.m_field3a = table[i].m_inventoryLimit;
        g_SPremiumInfo[i].m_additionalInfo.m_field3e = table[i].m_hpRegenSpeed;
        g_SPremiumInfo[i].m_additionalInfo.m_field40 = table[i].m_mpRegenSpeed;
        g_SPremiumInfo[i].m_additionalInfo.m_field42 = (unsigned int)table[i].m_moveSpeed;
        g_SPremiumInfo[i].m_additionalInfo.m_field46 = table[i].m_attackSpeed;
        g_SPremiumInfo[i].m_additionalInfo.m_field48 = table[i].m_castSpeed;
        g_SPremiumInfo[i].m_additionalInfo.m_field4a = table[i].m_hitRecovery;
        g_SPremiumInfo[i].m_additionalInfo.m_field4c = table[i].m_jumpPower;
        g_SPremiumInfo[i].m_additionalInfo.m_field4e = table[i].m_field118;

        g_SPremiumInfo[i].m_name = table[i].m_name;                    // +0x11c

        STPremiumItemData data;
        data.m_value0 = i;
        // [b28d-solo-CUserPremium] P0 取舍（b28d 实测）：ORIG 0x86af54c
        // `mov %eax,-0x38(%ebp)` + 空分支 0x86af667 回读，确为键具名局部；
        // 但 4.4 一旦为该键分配栈槽即触发帧 0x7e3c→0x7e4c、table 基址
        // -0x7e28→-0x7e2c 全帧平移（探针 /tmp/b28d_solo：带槽 ndiff 466、
        // 内联实参 56、纯 register 52；帧/槽对齐后整函数 ~700 条回正）。
        // 取内联实参形（P264/P112 帧族：语义与 ORIG 完全一致，仅少 store/load 对）。
        // [b28d-solo-CUserPremium] P22/P197④：ORIG 0x86af571 `test %eax,%eax;
        // setne %al; test %al,%al; je <空分支 0x86af651>` ⇒ 原码是 `size() != 0`
        // 正向判（非空分支直落、空分支置后），非 `==0` 反极性（后者产 sete）。
        if (table[i].m_premiumItems.size() != 0)
        {
            // [b28d-solo-CUserPremium] P13/P50：ORIG 0x86af5a2/0x86af5ce 的
            // begin/end 各只调一次（it=-0x44、end=-0x48 缓存），循环条件复用
            // 槽；原码在 for 初始化处同时直构 it/itEnd（无 iterator::C1Ev）。
            for (std::vector<premium_script_parsing::STPremiumItemData>::iterator
                     it = table[i].m_premiumItems.begin(),
                     itEnd = table[i].m_premiumItems.end();
                 it != itEnd; ++it)
            {
                data.m_c4 = it->m_termFlag;
                data.m_c5 = (unsigned char)(it->m_restrictFlag != 0);
                // [b28d-solo-CUserPremium] P292/P321-T3：ORIG 0x086af61d
                // `call; xor $0x1,%eax; test %al,%al; je` ⇒ bool 返回 + 裸 `!`
                // （`!=1` 形在 bool 下仍产 cmp/setne）。
                if (!SetPremiumItemData((unsigned long)it->m_key, data))
                    return 0x1bc;
            }
        }
        else
        {
            // 空表：flag 字节取已拷贝 g 表 m_checkTerm 低字节，key 用记录键。
            data.m_c4 = (unsigned char)g_SPremiumInfo[i].m_checkTerm;
            if (!SetPremiumItemData((unsigned long)table[i].m_targetItemKey, data))
                return 0x1c4;
        }
    }
    return 0;
}