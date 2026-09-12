// [b28d-69] P167 CLEAR（ORIG calls=134 = OUR 134）：补 m_mapInfo.m_mapIndex =
// map->get_index()（ORIG 0x0846063f）；去 aiList 缓存（P183：G_CDataManager 每使用点重调）；
// push_back 右值化（P270 EOj）；find 第三参 cast unsigned；getAppearancePoint /
// getStandardLevel / getItemGenerator 重调（P48 无 CSE）；mons/items 去 const
// （非 const begin/end + _List_const_iterator 转换 C1×2）；specialItems 改 const 指针
// （const at）；第四段循环条件改 m_party->m_battleField.m_bloodState!=1
//（消 get_party_type EXTRA，ORIG 0x0846136d 实证）+ _mapItem src 具名局部（C1 符号）。
// [J10 2026-09-11 复核] orig 920 / our 916（gap +4）；p167 134/134 CLEAR（EXTRA/MISSING
// 逐一配对为 C1↔C2、D1↔D2 同址别名 P93；注：game_func_report 对本题曾切错符号
// 0x085b5a4c（CParty::OnKilledMonster），本读数以 objdump 切片 0x08460634..0x084614eb 为权威）。
// 差异 = 全帧槽号 −4 平移（ORIG 0x0846068b `lea -0xf50(%ebp),%eax` ↔ 我方 `lea -0xf54`，
// N115/N126）+ m_field11 写形（ORIG 两条 movb 写 +0x11/+0x12 vs 我方 movzbl/shl/or/
// mov %ax 五条；字节视图试形 922 劣化已回滚，历史）+ 三元/临时构造块序。
bool CDeathTower::CStage::consistMap(const CDungeonMgr& dungeonMgr, const CMap* map,
                                     CDeathTower* tower)
{
    // [b28d-solo-CDeathTower] verdict=DIFF（orig 918 / our 920，P167 CLEAR 134/134）：
    // 语义（AI 布点/AP 随机怪/脚本怪/特殊被动物品四段）与调用点全等；残差 =
    // 4.4 帧 0x… vs ORIG 的槽号整体平移 + 实参跨 call 落槽（N82/N95/N7 族）
    // ⇒ byte 层 N 封顶。
    // [b28d-solo-CDeathTower2 复核] fresh 读数 orig 920 / our 920；p167 134/134
    // CLEAR（MISSING/EXTRA 全部为 C1↔C2、D1↔D2 同址别名）。试形：m_field11
    // 改 WarField.h 的 m_field11b 字节视图逐字节写（复现 ORIG 两条 movb）实测
    // 922 劣化 ⇒ 已回退（P248 S1 一次性旋钮）。残差 = 帧平移 0xc + 三元/临时
    // 构造块序 + m_amp 结构赋值写序 ⇒ N82/N93/N7 封顶。
    // ORIG 0x8460634：mobIdVec + apMap（拷贝自 aiList->getAPCount()）+
    // map_monster mob 局部，随后按三段填充：AI 布点 → AP 随机怪 → 脚本怪列表
    // → 特殊被动物品生成。tower 参数供第四段判 partyType（ORIG 0x8461371）。
    // [b28d-69] ORIG 0x0846063f：*(this+0x10)=m_mapInfo.m_mapIndex = map->get_index()；
    // G_CDataManager 每次使用点重调（p167 差集 -4 实证），禁缓存 aiList（P183）。
    m_mapInfo.m_mapIndex = map->get_index();
    std::vector<unsigned int> mobIdVec;
    std::map<int, int> apMap(G_CDataManager()->m_aiCharacterList->getAPCount());

    int mobIdx = 0;
    map_monster mob;
    const std::vector<STAICharacterMapArrangeData>* aiChars = map->getAICharacters();

    // ---- 第一段：图上 AI 布点 ----
    if (!aiChars->empty())
    {
        // [b28d-SCT] P13：ORIG call 序 begin(8)→end(9) 相邻 ⇒ end 循环外缓存；
        // 旧写法条件位每次求值 end（我方 end 落到 operator++ 之后）。
        std::vector<STAICharacterMapArrangeData>::const_iterator it = aiChars->begin();
        std::vector<STAICharacterMapArrangeData>::const_iterator itEnd = aiChars->end();
        for (; it != itEnd; ++it)
        {
            const STAICharacterMapArrangeData& data = *it;
            CAICharacter* aiChar =
                G_CDataManager()->m_aiCharacterList->get(data.m_type);
            if (!aiChar)
                return false;

            mobIdVec.push_back(aiChar->getIdx());

            if (aiChar->getAppearancePoint() != 0)
            {
                std::map<int, int>::iterator it2 =
                    apMap.find(aiChar->getAppearancePoint());
                if (it2 != apMap.end())
                    it2->second = std::max(it2->second - 1, 0);
            }

            memset(&mob, 0, sizeof(mob));
            mob.m_mobId = mobIdx;
            mob.m_mobIndex = data.m_type;
            mob.m_instId = m_mapInfo.m_assignItemIdx;
            m_mapInfo.m_assignItemIdx++;
            mob.m_roleType = 5;
            mob.m_level = aiChar->getLevel();
            mob.m_dropCnt = data.m_camp;
            // [R16a13 · §16.2] ORIG 0x08460899..0x084608ac 顺序 = +0x2c(m_dropCnt)
            //   → +0x11=0 → +0x12=0；两条独立 `movb $0x0,-0xf3f/-0xf3e(%ebp)` 即
            //   m_field11 的**字节视图逐字节清零**（short 赋值只会产单条 movw，
            //   探针 /tmp/verify357/R16a13/probe_union.s f/g 对照实证）。旧译漏写 ⇒ 补。
            mob.m_field11b.m_field11b0 = 0;
            mob.m_field11b.m_field11b1 = 0;
            m_mapInfo.Add_Mob(mob);
            mobIdx++;
        }
    }

    // ---- 第二段：AP 随机怪（剩余点数预算扣减；ORIG 0x8460940..0x8460d28）----
    // ORIG 结构：do { while(1){ [a]剩余<minAP→结束; [b]随机取怪;
    // [c]find→已在 mobIdVec(本段或首段已布)→break; [d]新怪:入 vec/扣点/建怪 }
    // [e]统计 ≤剩余点数的可用 AP 总量; } while(total != 0);
    int apRandomPoint = map->getAPCRandomPoint();
    if (apRandomPoint != 0)
    {
        mobIdx = 0x40;
        // [b28d-SCT] P25/循环布局：ORIG 0x08460966 入口 `jmp 0x08460d02`（条件块
        // 物理在尾部，0x08460d02 处才 call getMinAP），循环体 0x0846096b 在前 ⇒
        // 原码为 while(cond){BODY}；旧 for(;;){if(cond)break;BODY} 令 4.4 把条件
        // 排到体前（我方 getMinAP 在 call 序 32-33，ORIG 在 65-66）。
        while (G_CDataManager()->m_aiCharacterList->getMinAP() <= apRandomPoint)
        {
            CAICharacter* aiChar =
                G_CDataManager()->m_aiCharacterList->getByAP(
                    static_cast<unsigned int>(get_rand_int(apRandomPoint) + 1));
            if (!aiChar)
                return false;

            std::vector<unsigned int>::iterator fi = std::find(
                mobIdVec.begin(), mobIdVec.end(),
                (unsigned int)aiChar->getIdx());
            if (fi != mobIdVec.end())
            {
                // [e] 抽中已布过的怪：统计可用 AP 总量，为 0 才结束
                int total = 0;
                for (std::map<int, int>::const_iterator ci = apMap.begin();
                     ci != apMap.upper_bound(apRandomPoint); ++ci)
                    total += ci->second;
                if (total == 0)
                    break;
                continue;
            }

            // [d] 新怪：入已用表、扣减出现点数、建怪（m_dropCnt=100 ORIG 立即数）
            mobIdVec.push_back(aiChar->getIdx());
            std::map<int, int>::iterator it2 =
                apMap.find(aiChar->getAppearancePoint());
            if (it2 != apMap.end())
                it2->second = std::max(it2->second - 1, 0);
            apRandomPoint -= aiChar->getAppearancePoint();

            memset(&mob, 0, sizeof(mob));
            mob.m_mobId = mobIdx;
            mob.m_mobIndex = aiChar->getIdx();
            mob.m_instId = m_mapInfo.m_assignItemIdx;
            m_mapInfo.m_assignItemIdx++;
            mob.m_roleType = 5;
            mob.m_level = aiChar->getLevel();
            mob.m_dropCnt = 100;
            // [R16a13 · §16.2] ORIG 0x08460c88..0x08460c99：+0x2c=100 后同样两条
            //   `movb $0x0,-0xf3f/-0xf3e(%ebp)`（m_field11 字节视图清零）⇒ 补。
            mob.m_field11b.m_field11b0 = 0;
            mob.m_field11b.m_field11b1 = 0;
            m_mapInfo.Add_Mob(mob);
            mobIdx++;
        }
    }

    // ---- 第三段：脚本怪列表（ORIG 0x8460d28..0x8460f74）----
    mobIdx = 0;
    std::vector<int> namedVec;
    // ORIG 非 const 局部（按值拷贝）+ const_iterator 隐式转换（p167 差集
    // list<_mapMonster>::begin/end 非 const + _List_const_iterator C1×2 实证）。
    std::list<_mapMonster> mons = map->get_monster_list();
    for (std::list<_mapMonster>::const_iterator it = mons.begin();
         it != mons.end(); ++it)
    {
        const _mapMonster& src = *it;
        memset(&mob, 0, sizeof(mob));
        mob.m_mobId = mobIdx;

        // ORIG 0x8460dc5：flag04==0 → 直用 m_field08 低字节；
        // 否则全宽 int 加法后截断 uchar：0→1，>200→200，1..200 原样。
        // [试过无效 · 勿再试] 删 lv 中间变量直写 mob.m_level（想复现 ORIG 的
        // `mov %al,-0xf40` 字节 clamp）：4.4 生 movzbl+int 槽形，ndiff 800→854 劣化
        // ⇒ 已回滚；残余归 N82/N115 帧槽族。
        if (src.m_flag04 != 0)
        {
            unsigned int lv = (unsigned char)(dungeonMgr.getStandardLevel() +
                                              src.m_field08);
            if (lv == 0)
                lv = 1;
            else if (lv > 200)
                lv = 200;
            mob.m_level = static_cast<unsigned char>(lv);
        }
        else
        {
            mob.m_level = static_cast<unsigned char>(src.m_field08);
        }

        if (src.m_mobId == -1)
            mob.m_mobIndex =
                G_CDataManager()->m_monsterManager->get_levelrandom_mob(
                    mob.m_level);
        else
            mob.m_mobIndex = src.m_mobId;

        // ORIG 分别写 +0x11/+0x12 两字节 = m_field11 半字（小端拼装）
        // [b28d-solo-CDeathTower2 试形] 字节视图 m_field11b 两 movb 形实测 922
        // （劣于本形 920）⇒ 保留 short 拼装（P248 S1 一次性旋钮，回退记 N）。
        mob.m_field11 = static_cast<short>(
            (static_cast<unsigned short>(
                static_cast<unsigned char>(src.m_levelAdd)) |
             (static_cast<unsigned short>(
                static_cast<unsigned char>(src.m_field14)) << 8)));
        mob.m_roleType = static_cast<char>(
            static_cast<unsigned char>(src.m_roleType));
        mob.m_instId = m_mapInfo.m_assignItemIdx;
        m_mapInfo.m_assignItemIdx++;

        if (mob.m_roleType == 0 && !mob.isNamedMonster())
            namedVec.push_back(mob.m_instId);

        mob.m_field13 = static_cast<unsigned char>(src.m_champion);
        mob.m_field14 = static_cast<unsigned char>(src.m_posX);
        mob.m_dropCnt = src.m_hpPct;
        m_mapInfo.Add_Mob(mob);
        mobIdx++;
    }

    // ---- 第四段：特殊被动物品生成（ORIG 0x8460fdd..0x846133f）----
    _mapItem src;
    const std::vector<stDungeonAssignItem_t>* specialItems =
        dungeonMgr.getSpecialPassiveObjectItems();
    stGenerateRefData_t refData1;
    stGenerateRefData_t refData2;
    dungeonMgr.getObjectDropItems();   // ORIG 取出未用（忠实保留）
    std::list<_mapItem> items = map->get_item_list();

    for (std::list<_mapItem>::iterator it = items.begin();
         it != items.end(); ++it)
    {
        // ORIG 0x0846136d：循环条件 tower->m_party->m_battleField.m_bloodState!=1
        //（单人则停止本图特殊被动物品生成）；ORIG 无 get_party_type 调用（P167 EXTRA）。
        if (tower->m_party->m_battleField.m_bloodState == 1)
        {
            break;
        }

        src = *it;
        const stDungeonAssignItem_t& assign =
            specialItems->at(static_cast<unsigned int>(src.m_objectIdx));

        if (assign.m_assignItemIdx != -1)
        {
            // [Main 视图 cast 反模式] 旧 `*reinterpret_cast<char*>(&refData1.m_rankBonusIndex)`
            // 取址 cast 使 4.4 产 `lea -0xb6c; lea 0x38; mov %al,(%ebx)`（3 条）；权威头
            // union 已有 +0x38/+0x39 字节视角 ⇒ 具名直写复现 ORIG 0x084610ad
            // `mov %al,-0xb30(%ebp)` 单条 disp 写（零布局/sizeof/mangled 影响）。
            refData1.m_field38 = static_cast<unsigned char>(assign.m_assignItemIdx);
        }
        else
        {
            // ORIG 0x084610c0..0x084610f5：getStandardLevel 三次重调（P48 无 CSE）
            if (dungeonMgr.getStandardLevel() < 1 ||
                dungeonMgr.getStandardLevel() > 0xc8)
                continue;
            refData1.m_field38 = static_cast<unsigned char>(dungeonMgr.getStandardLevel());
        }
        // ORIG 0x84610fb..0x846111b：refData1+0x39 = src.m_field04 低字节
        //（+0x38/+0x39 即 m_rankBonusIndex 的低两字节，ORIG 均为 movb 单字节写：
        //  084610ad/084610f5 写 +0x38、0846110f 写 +0x39）
        refData1.m_field39 = static_cast<unsigned char>(src.m_field04);

        stGenerateResult_t result;
        // [SweepSTEventScript 2026-09-09 真 bug 修正] ORIG 0x0846114x 虚槽 `add $0xc`
        // = CItemGenerator::generateSpecificItem（vtable word[5]；word[4]=+0x08 才是
        // generateRandomItem，见 CItemGenerator.h:46-50 与 CBossPlay.cpp:452-457 同款判据）
        // ⇒ 第一次调用原写 generateRandomItem 为虚槽取错。
        G_CDataManager()->getItemGenerator()
            ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_1)
            ->generateSpecificItem(refData1, result);
        G_CDataManager()->getItemGenerator()
            ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_4)
            ->generateSpecificItem(refData2, result);

        // [b28d-SCT] P13：ORIG call 序 begin(105)→end(106) 相邻 ⇒ end 缓存。
        std::vector<Inven_Item>::iterator d = result.m_dropItems.begin();
        std::vector<Inven_Item>::iterator dEnd = result.m_dropItems.end();
        for (; d != dEnd; ++d)
        {
            map_item drop;
            drop.m_count = 1;
            drop.m_itemIndex = m_mapInfo.m_dropItemCnt;
            m_mapInfo.m_dropItemCnt++;
            drop.m_standardLevel = static_cast<char>(src.m_field0c);
            drop.m_item = *d;
            m_mapInfo.Add_Item(drop);
        }
    }

    return true;
}