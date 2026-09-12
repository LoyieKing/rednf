// ORIG execute 0x084b0b4e。
// 【b28d-2026-09-10 重判】jcc 逐位交叉验证揪出 1 处条件反向真 bug：尾段
// `count == 0 ? memcpy(kNoDropList) : printClient+sprintf` 的块序写反（ORIG
// 0x084b140c `cmpl $0; je <memcpy>` = then=printClient 直落）⇒ 改 `if (count != 0)`
// 后 jcc 序列 23=23 全等（P272/P194）。另两处 P0 右值内联：getDungIndex 结果
// `movzwl %ax,%ebx` 直传（0x084b0b95）、field->GetCurrentMapInfo() 内联
// （0x084b0c84 无 field 具名槽）⇒ 助记符 619→617（ORIG 616，净 +1 为
// `mov $0x17318,%edx` 常量中转缺失等寄存器伪影）。
// 文件名分支反写为 `if (!GMD_IND_ALL(this))` 复现 ORIG `xor $0x1` 取反物化 +
// then=非 all 直落块序（P168a/P272，0x084b13b5）。
// 验收三件套（§6.2，orig 616>500）：① 助记符 617 vs 616（+1 寄存器伪影）；② p167 98=98（MISSING/EXTRA 仅
// C1/C2、D1/D2 同址别名，P93）；③ 全 TU call* 虚槽 + jcc 序列交叉验证 0 差。
// 残余为 4.4 槽复用 vs 4.1 独立槽的全帧槽号错位（N82）+ `mov $imm,%edx` 常量中转
// （N76/P238）等 20 个寄存器/块序伪影块，byte 层封顶（语义闭合：缓冲清零、
// hash_map/set/list/vector 迭代器 cv+后置增量、P183 去缓存、P49 宏化访问器均归位）。
// [b28c-R26 复验] p167 98=98 CLEAR（MISSING/EXTRA 仅 C1/C2、D1/D2 同址别名）；
//   gap 616/617 复核不变。
// [round5-GmCmds_D 复核] 617 vs 616（gap +1）、p167 98=98（同上别名）⇒ 无缺口。
//   残余 = N82 槽序 + memset 第 3 实参 `mov $0x17318,%edx; mov %edx,0x8(%esp)` 的
//   4.1 常量中转（ORIG 0x084b0bfc 段；与我方少 1 条，与 CUpgradeCardDebug 同现象，
//   P68 `memset((char*)p,0,N)` 两形均实测无效）。P339/P331 复核：两侧槽直方图同形、
//   整体平移（非槽映射互换）⇒ 声明序不可翻，byte 层封顶（§6.2 三件套验收）。
void CIndependentDrop::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return;
    }
    // ORIG 0x084b0b90/95：getDungIndex 结果 `movzwl %ax,%ebx` 直传（无具名槽往返，
    // P0 右值内联）⇒ 实参内联进 find_dungeon 调用。
    CDungeon* dungeon = G_CDataManager()->find_dungeon(party->getDungIndex());
    if (dungeon == 0)
    {
        return;
    }

    // ORIG 逐 buffer 形不同（0x084b0bb6/0x084b0be6/0x084b0bf1/0x084b0c40）：
    // lineBuf/chatMsg 对齐 ⇒ 简单 rep stos（`={0}`）；content 95000B ⇒ call
    // memset；fileName[0x1e] 非对齐 ⇒ 内联慢路（P312）。
    // [BigStore/临时授权] REAL-BUG 修正：ORIG 两处 rep stos 均为 `mov $0x100,%edx`
    // = 0x100 **dwords** = 1024 字节（0x084b0bc1 / 0x084b0c4b），旧注释把 dword 数
    // 误当字节数写成 [0x100] ⇒ 少清 768 字节，且 sprintf 最长可达 1024 时我方会
    // 栈溢出。ORIG 缓冲跨度 -0x508..-0x108 / -0x908..-0x508 各 0x400 字节。
    char lineBuf[0x400] = {0};                               // -0x508 sprintf 行缓冲（ORIG 0x400B）
    char content[95000];                                     // auStack_17c24 输出累计缓冲
    memset(content, 0, sizeof(content));
    char fileName[0x1e] = {0};                               // -0xc6
    char chatMsg[0x400] = {0};                               // -0x908（ORIG 0x400B）

    GMD_IND_MONS(this).clear();
    GMD_IND_AIS(this).clear();

    if (!GMD_IND_ALL(this))
    {
        // 当前副本当前地图：roleType<4 → 普通怪集，4..8 → AI 集合。
        // ORIG 0x084b0c84：`mov party; add $0xb24; mov (%esp)` 直传（无 field 具名槽，
        // P0 右值内联）⇒ 成员访问内联进调用。
        MapInfo* curMap =
            reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField
                .GetCurrentMapInfo();                        // ORIG 0x822cbcc
        for (std::map<int, map_monster>::iterator it = curMap->m_monsterMap.begin();
             it != curMap->m_monsterMap.end(); it++)
        {
            if (it->second.m_roleType < 4)
            {
                GMD_IND_MONS(this).insert(it->second.m_mobIndex);
            }
            else if (it->second.m_roleType < 9)
            {
                GMD_IND_AIS(this).insert(it->second.m_mobIndex);
            }
        }
    }
    else
    {
        // 全地图收集属于本副本的所有普通怪/AI idx。
        // ORIG 调 hash_map::begin() const / end() const ⇒ 原码经 const 指针访问。
        const CMapList* mapList =
            G_CDataManager()->m_mapList->GetMapList();       // ORIG this=datamgr+4
        const int di = dungeon->get_index();                 // ORIG CDungeon::get_index
        for (__gnu_cxx::hash_map<int, CMap*>::const_iterator it =
                 mapList->m_mainMap.begin();
             it != mapList->m_mainMap.end(); it++)
        {
            // ORIG 循环体 3 次独立 `it->second`（ptEv @0x084b0deb/0x084b0ea3/
            // 0x084b0f63，P183 禁指针缓存）⇒ 逐处表达式直读，无 map 局部。
            // ORIG list<int>::begin()/end() const + find<_List_const_iterator> ⇒
            // dungs 经 const 指针访问；守卫用正极性 `!=`（P220，消 operator==）。
            const std::list<int>* dungs = it->second->getDungeonLists();
            if (std::find(dungs->begin(), dungs->end(), di) != dungs->end())
            {
                std::list<_mapMonster> monsters =
                    it->second->get_monster_list();          // 按值返回（ORIG sret）
                for (std::list<_mapMonster>::iterator mit = monsters.begin();
                     mit != monsters.end(); mit++)
                {
                    GMD_IND_MONS(this).insert(mit->m_mobId); // insert 取首 int
                }
                const std::vector<STAICharacterMapArrangeData>* ais =
                    it->second->getAICharacters();
                for (std::vector<STAICharacterMapArrangeData>::const_iterator ait =
                         ais->begin();
                     ait != ais->end(); ait++)
                {
                    GMD_IND_AIS(this).insert(ait->m_type);   // insert 取首 int
                }
            }
        }
    }

    // ---- 普通怪独立掉落输出（script+0x3ac 表拷贝逐条 ResultToStr）----
    int count = 0;                                           // -0x34
    for (std::set<int>::const_iterator sit = GMD_IND_MONS(this).begin();
         sit != GMD_IND_MONS(this).end(); sit++)
    {
        CMonster* monster =
            static_cast<CMonster*>(G_CDataManager()->find_monster(*sit));
        if (monster == 0)
        {
            continue;
        }
        // ORIG 两次独立 `monster->getMonsterScript()`（0x084b10eb/0x084b1126，
        // P0/P183 无 script 局部缓存）。
        std::vector<stMonsterIndependentDrop_t> drops(
            static_cast<STMonsterScript*>(monster->getMonsterScript())
                ->m_independentDrops);
        if (!drops.empty())
        {
            // [R28fmt 2026-09-13 §34.2.1 实参来源轴] ORIG 0x084b1146/0x084b1148/
            // 0x084b114c/0x084b1150：arg3(0xc)=*sit、arg2(0x8)=0（`mov (%eax),%eax;
            // mov %ebx,0x10(%esp); mov %eax,0xc(%esp); movl $0x0,0x8(%esp)`）⇒ 源码为
            // (0, *sit, name)——原 (sit, 0) 使输出列序相反（0/1 标志与怪物 ID 互换）。
            sprintf(lineBuf, "%d\t%d\t%s\n", 0, *sit,
                    static_cast<STMonsterScript*>(monster->getMonsterScript())
                        ->m_name.c_str());
            strcat(content, lineBuf);
            ResultToStr(drops, content);
            ++count;
        }
    }

    // ---- AI 角色独立掉落输出 ----
    for (std::set<int>::const_iterator sit = GMD_IND_AIS(this).begin();
         sit != GMD_IND_AIS(this).end(); sit++)
    {
        CAICharacter* ai = G_CDataManager()->m_aiCharacterList->get(// ORIG datamgr+0x869c
            static_cast<unsigned int>(*sit));
        if (ai == 0)
        {
            continue;
        }
        // ORIG W 0x084b410e：sret 直构进 drops 槽（源形=拷贝初始化）。
        std::vector<stMonsterIndependentDrop_t> drops = ai->getIndepentDropItem();
        if (!drops.empty())
        {
            // [R28fmt 2026-09-13 §34.2.1 实参来源轴] ORIG 0x084b12d1/0x084b12d5：
            // arg3(0xc)=*sit、arg2(0x8)=1 ⇒ 源码为 (1, *sit, name)。
            sprintf(lineBuf, "%d\t%d\t%s\n", 1, *sit, ai->GetName());
            strcat(content, lineBuf);
            ResultToStr(drops, content);
            ++count;
        }
    }

    // ORIG 0x084b13b5 `movzbl all; xor $0x1; test; je <all 分支>`：then=非 all 直落、
    // all 分支后置 ⇒ 源码写 `if (!GMD_IND_ALL(this))`（P168a X 族取反物化 + 块序）。
    if (!GMD_IND_ALL(this))
    {
        sprintf(fileName, "item_indepedent_drop_%s.txt", dungeon->GetDungeonName());
    }
    else
    {
        sprintf(fileName, "item_indepedent_drop_all_%s.txt",
                dungeon->GetDungeonName());
    }

    // ORIG 0x084b140c `cmpl $0; je <memcpy 分支>`：then=printClient 直落、else=memcpy
    // 后置 ⇒ 源码必须写 `if (count != 0)`（P272/P194 块序；`count == 0` 反产 jne）。
    if (count != 0)
    {
        printClient(GetUser(), fileName, content);
        sprintf(chatMsg, "%s\xc6\xc4\xc0\xcf\xc0\xcc \xbb\xfd\xbc\xba\xb5\xc7\xbe\xfa\xbd\xc0"
                         "\xb4\xcf\xb4\xd9.\n", fileName);
    }
    else
    {
        memcpy(chatMsg, "\xc3\xe2\xb7\xc2\xc7\xd2 \xc1\xf6\xc1\xa4\xb5\xe5\xb6\xf8 \xb8\xae\xbd"
                        "\xba\xc6\xae\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.\n", 0x24);
    }
    SendChatMsg(GetUser(), chatMsg);
}