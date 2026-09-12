// ---- consistMap（ORIG 0x0814bef4）----
// 按源图布怪：AI 布点 → AP 随机怪（预算扣减）→ 脚本怪列表 → 特殊被动物品。
// 任一 AI 查询失败返回 false。map_monster 具名字段映射：
//   m_field11 半字 = ORIG +0x11/+0x12 两字节写、m_field13/m_field14 同名、
//   m_dropCnt(+0x2c) 收布点/脚本怪掉落数。
// [b28d-62] N89+N82：const/非 const 迭代器族 + map 拷贝 ctor + getParty。
//   ORIG 0x0814bef4
// [b28d-solo-CBossStage] 待办：p167 剩 8 missing（getIdx/getAppearancePoint/
//   get_standard_level 各 -2、G_CDataManager -1、_mapItem C1Ev -1、at -1、
//   getItemGenerator -1）。已修：G_CDataManager 直调 ×5、apStat 默认构造+赋值、
//   (*it).m_type（P167）、mobs/items 去 const。927 条超大函数，槽/帧分区为主。
// [b28d-solo-CBossStage] 封顶 N82：p167 修 getIdx×2/getAppearancePoint×2/
//   get_standard_level×2/G_CDataManager×1/getItemGenerator×1（全部改内联重读，
//   P157/P0）+ find 极性 `!=`（P220）+ upper_bound/begin 缓存（P13）+ at const 版
//   + memcpy→结构赋值（N84）；残差=_mapItem C1Ev 发射点（4.4 list move 赋值 vs
//   ORIG 元素默认构造，N86 族）+map_monster C1/D1 计数+槽/帧分区（0xfbc vs 0xf9c）。
//   927 条超大函数，语义完成（调用点差集仅剩 1 项结构性）。
// [b28d-62 本轮] 修 3 处真缺口：map_monster 提升到三循环外（ORIG 仅一枚
//   -0xf50 对象，P0 槽证据；帧 0x161c→0xfbc 对齐、-29 条）、第二段 AP 循环
//   改底部条件 while（ORIG 0x0814c240 `jmp cond`，P283）、等级夹取拆嵌套
//   if（ORIG 0x0814c704 双判）、refGen1/refGen4 构造提前到 objectDropItems/
//   item_list 之前 ⇒ 929/920（gap +9，其中 6 条为 list 拷贝构造的 4.4 move
//   折叠，N 类）⇒ 本件无我方缺失的语义段；p167 CLEAR。
bool CBossStage::consistMap(const CMap* map)
{
    m_stageData.m_mapIndex = map->get_index();

    std::vector<unsigned int> usedAIs;
    // ORIG：std::map::operator= 从 CAICharacterList::getAPCount() 结果拷贝
    // P0：ORIG 每处直调 G_CDataManager()（p167 MISSING ×5）⇒ 禁 dm 缓存。
    // p167：ORIG 为 C1Ev + operator=（默认构造 + 拷贝赋值），非拷贝构造。
    std::map<int, int> apStat;
    apStat = G_CDataManager()->m_aiCharacterList->getAPCount();
    int counter = 0;
    // ORIG 0x0814bf55 后构造 map_monster（-0xf50）一次，三个布怪循环
    // 各自 memset 后复用（P0 槽证据：ORIG 仅一枚 0x34c 对象）。
    map_monster mob;

    // ---- 第一段：图上 AI 布点（STAICharacterMapArrangeData 向量）----
    const std::vector<STAICharacterMapArrangeData>* arranges =
        map->getAICharacters();
    if (!arranges->empty())
    {
        counter = 0;
        // [R15a13 · ④ 修复（P13）] ORIG 0x0814bf95/bfbd：begin→-0xad4、end→-0xad8
        //   （**具名** end 迭代器）后 `jmp cond`，条件块只 `ne(it,itEnd)`
        //   ⇒ 原码为**循环外缓存 end()一次**；我方原形 `it != arranges->end()`
        //   在条件位每轮重算（4.4.7 探针实证：同源形恒把 end() 落条件块）。
        //   语义等价（容器 const），本条按 ORIG 源形复刻。
        std::vector<STAICharacterMapArrangeData>::const_iterator it =
            arranges->begin();
        const std::vector<STAICharacterMapArrangeData>::const_iterator itEnd =
            arranges->end();
        for (; it != itEnd; ++it)
        {
            // P167：ORIG 用 operator*（p167 MISSING de / EXTRA ptEv）⇒ (*it)
            // [R13a12 2026-09-12 ② 调用点差集] ORIG 0x0814bfd8 `mov %eax,-0x30(%ebp)` =
            //   `*it` 的**具名解引用局部**（-0x30 存指针，其后 0x0814bfdb/0x0814c132/
            //   0x0814c170 三处经 -0x30 复用，de 仅 1 次/迭代）；我方原写两处 `(*it)`
            //   使 `deEv` 静态 2 次（p167 EXTRA ×1）。P0 槽律：ORIG 有 -0x30 槽往返 ⇒
            //   原码确为具名局部。
            const STAICharacterMapArrangeData& arr = *it;
            unsigned int aiType = (unsigned int)arr.m_type;
            CAICharacter* ai =
                G_CDataManager()->m_aiCharacterList->get(aiType);
            if (ai == 0)
                return false;

            usedAIs.push_back((unsigned int)ai->getIdx());
            // [b28d-solo] P157：ORIG 0x0814c03b/814c053 两次独立调用 getAppearancePoint
            //   （无 ap 具名局部）。
            if (ai->getAppearancePoint() != 0)
            {
                std::map<int, int>::iterator st =
                    apStat.find(ai->getAppearancePoint());
                if (st != apStat.end())
                    st->second = std::max(st->second - 1, 0);
            }

            memset(&mob, 0, sizeof(mob));
            mob.m_mobId = counter;                            // +0x00 序号
            mob.m_mobIndex = (int)aiType;                     // +0x0c
            // [R19a06 2026-09-13 · 形态对齐 P0/P183] ORIG 0x0814c140..0x0814c152 为
            //   **单次读**：`mov 0xa30(%eax),%eax; mov %eax,-0xf4c; lea 0x1(%eax),%edx;
            //   mov %edx,0xa30(%eax)` ⇒ 原码是**后置自增复合式**（`= m_assignItemIdx++`），
            //   非两条语句（两条语句使 4.4 发第二次读，我方多 1 条物化）。语义等价。
            mob.m_instId = m_stageData.m_assignItemIdx++;     // +0x04 seq++
            mob.m_roleType = 5;                               // APC
            mob.m_level = (char)(unsigned char)ai->getLevel();
            mob.m_dropCnt = arr.m_camp;                       // +0x10 dword（STMapScript 批次命名 m_camp；
                                                              //   本调用面按等级消费，语义双证待裁决）
            m_stageData.Add_Mob(mob);                         // ctor 已清 +0x11/+0x12
            ++counter;
        }
    }

    // ---- 第二段：AP 随机怪（剩余点数预算扣减）----
    int remainAP = map->getAPCRandomPoint();
    if (remainAP != 0)
    {
        counter = 0x40;
        // ORIG 0x0814c240 `jmp 0814c5e5`：条件在循环尾（4.1 while 布局）；
        //   while(true)+头部 break 会被 4.4 条件置顶（P283）。
        while (remainAP >= G_CDataManager()->m_aiCharacterList->getMinAP())
        {
            CAICharacter* ai = G_CDataManager()->m_aiCharacterList->getByAP(
                (unsigned int)(get_rand_int(remainAP) + 1));
            if (ai == 0)
                return false;

            // [b28d-solo] P157/P220：ORIG 0x0814c326 用 `__gnu_cxx::ne`
            //   （find(...) != end）且 alreadyUsed 路径直落、新增路径跳转；
            //   getIdx×3/getAppearancePoint×2 均内联（无 aiIdx/ap 局部）。
            if (std::find(usedAIs.begin(), usedAIs.end(),
                          (unsigned int)ai->getIdx()) != usedAIs.end())
            {
                // 已用过的 AI：重算 ≤ 剩余点数的可用 AP 总量，为 0 则退出
                // [b28d-solo] ORIG 0x0814c252..c2dc begin/upper_bound 各求值一次
                //   （P13 缓存 end 迭代器）。
                std::map<int, int>::const_iterator st = apStat.begin();
                std::map<int, int>::const_iterator stEnd =
                    apStat.upper_bound(remainAP);
                int avail = 0;
                for (; st != stEnd; ++st)
                    avail += st->second;
                if (avail == 0)
                    break;
            }
            else
            {
                usedAIs.push_back((unsigned int)ai->getIdx());
                std::map<int, int>::iterator st =
                    apStat.find(ai->getAppearancePoint());
                if (st != apStat.end())
                    st->second = std::max(st->second - 1, 0);
                remainAP -= ai->getAppearancePoint();

                memset(&mob, 0, sizeof(mob));
                mob.m_mobId = counter;
                mob.m_mobIndex = ai->getIdx();
                mob.m_instId = m_stageData.m_assignItemIdx++;   // R19a06：ORIG 单次读形态
                mob.m_roleType = 5;
                mob.m_level = (char)(unsigned char)ai->getLevel();
                mob.m_dropCnt = 100;                          // ORIG 立即数 100
                m_stageData.Add_Mob(mob);
                ++counter;
            }
        }
    }

    // seed 值（ORIG get_rand_int(0) 后清 bit 0x40000）
    m_stageData.m_mapBasisLevel = get_rand_int(0);
    // [R19a06 2026-09-13 · 形态对齐] ORIG 0x0814c620..0x0814c634 为 **disp 直读直写**
    //   （`mov 0xa3c(%eax),%eax; mov %eax,%edx; and $0xfffbffff,%edx; mov %edx,0xa3c(%eax)`）；
    //   旧写法 `*(unsigned int*)&…` 的 cast 取址使 4.4 发两处 `add $0xa3c`（+3 条我方多出物化）。
    m_stageData.m_mapBasisLevel &= 0xfffbffffu;

    std::vector<int> singleSpawnSeqs;   // 非 named 单人刷怪序号表

    // ---- 第三段：脚本怪列表 ----
    counter = 0;
    // p167：ORIG 有 _List_const_iterator<mapMonster> 转换构造 ×2 ⇒ 非 const list
    //   + const_iterator（转换形）。
    std::list<_mapMonster> mobs = map->get_monster_list();
    for (std::list<_mapMonster>::const_iterator it = mobs.begin();
         it != mobs.end(); ++it)
    {
        const _mapMonster& em = *it;
        memset(&mob, 0, sizeof(mob));
        mob.m_mobId = counter;

        // 等级：em.m_flag04==0 直接取 em.m_field08；否则 标准等级+m_field08，
        // 字节域夹取 [1..99]
        unsigned char lv;
        if ((char)em.m_flag04 == 0)
            lv = (unsigned char)(char)em.m_field08;
        else
        {
            lv = (unsigned char)(m_dungeon->get_standard_level() +
                 (int)(signed char)em.m_field08);
            // ORIG 0x0814c704..c72d 双判 `cmp $0x62; ja` + `test; jle`（Ghidra
            //   `(lv < 99) && (lv == 0)`）；4.4 前端逻辑折叠成单判 `lv == 0`
            //   （&&/嵌套 if 两形实测同码）⇒ 6 条按跨版本 N 封顶。
            if (lv < 99 && lv == 0)
                lv = 1;
            else if (lv > 99)
                lv = 99;
        }
        mob.m_level = (char)lv;

        if (em.m_mobId == -1)
            mob.m_mobIndex =
                G_CDataManager()->m_monsterManager->get_levelrandom_mob((int)lv);
        else
            mob.m_mobIndex = em.m_mobId;

        // ORIG 分别写 +0x11/+0x12 两字节 = m_field11 半字（小端拼装）
        mob.m_field11 = (short)(
            ((unsigned short)(unsigned char)(char)em.m_levelAdd) |
            ((unsigned short)(unsigned char)(char)em.m_field14 << 8));
        mob.m_roleType = (char)(unsigned char)em.m_roleType;
        mob.m_instId = m_stageData.m_assignItemIdx++;           // R19a06：ORIG 单次读形态

        if ((signed char)mob.m_roleType == 0 && !mob.isNamedMonster())
            singleSpawnSeqs.push_back(mob.m_instId);

        mob.m_field13 = (char)(unsigned char)em.m_champion;   // em +0x1c
        // em +0x20 低字节（_mapMonster::m_posX 首字节；CBattle_Field 同型转写
        // BattleMonsterDepsC2.cpp mob.m_field14 ← ref.m_posX）
        mob.m_field14 = (char)(unsigned char)em.m_posX;
        mob.m_dropCnt = em.m_hpPct;                           // em +0x28
        m_stageData.Add_Mob(mob);
        ++counter;
    }

    // ---- 第四段：特殊被动物品生成 ----
    // [R13a12 2026-09-12 ④ 修复] ORIG 0x0814c8f5 `lea -0xac4(%ebp),%eax; mov %eax,(%esp);
    //   call 815088e <_ZN8_mapItemC1Ev>` = **循环外具名值对象**默认构造（位于
    //   getSpecialPassiveObjectItems 之前）；循环内 0x0814c98b..0x0814c9ad 为
    //   `mi = *it;` 的 4 dword 隐式拷贝赋值（`mov (%eax),%edx; mov %edx,-0xac4(%ebp)` ×4）。
    //   旧形 `const _mapItem& mi = *it;` 只把指针存槽（-0x24）⇒ p167 O-only
    //   `_ZN8_mapItemC1Ev`（我方缺 1 call）、槽/帧少 16 字节；引用在循环体内无别名
    //   破坏（items 为值拷贝、体内不修改）⇒ 语义等价，本改为按 ORIG 复刻具名对象形。
    _mapItem mi;
    // [b28d-solo] p167：ORIG 用 const 版 vector::at ⇒ specialItems 为 const 指针。
    const std::vector<stDungeonAssignItem_t>* specialItems =
        m_dungeon->getSpecialPassiveObjectItems();
    // ORIG 0x0814c925/c933：refGen1/refGen4 构造先于 getObjectDropItems()
    //   与 get_item_list()（Ghidra C 序：specialItems→refGen1→refGen4→
    //   objectDropItems→items）。
    stGenerateRefData_t refGen1;
    stGenerateRefData_t refGen4;
    // ORIG 同时取出 getObjectDropItems() 但未使用（忠实保留调用）
    m_dungeon->getObjectDropItems();
    // [b28d-solo] 两句式（C1Ev+move 赋值）实测引入 list aSEOS2_ NEW 符号且
    //   _mapItem C1Ev 仍未发射 ⇒ 回退拷贝构造形。
    std::list<_mapItem> items = map->get_item_list();

    std::list<_mapItem>::iterator it = items.begin();
    while (it != items.end())
    {
        mi = *it;
        const stDungeonAssignItem_t& assign =
            specialItems->at((unsigned int)mi.m_objectIdx);

        unsigned char genLevel;
        if (assign.m_assignItemIdx != -1)
        {
            genLevel = (unsigned char)(char)assign.m_assignItemIdx;
        }
        else
        {
            // [b28d-solo] P157：ORIG 0x0814c9ee/ca03/ca2f 三次独立调用
            //   get_standard_level（无 sl 具名局部）。
            if (m_dungeon->get_standard_level() <= 0 ||
                m_dungeon->get_standard_level() > 200)
            {
                // [2026-09-12 用户裁定：**完全复刻 ORIG 逻辑（即使是错误逻辑）**]
                // ORIG 0x0814ca0f `mov $0x1,%eax; jmp 0x0814ca1b` → `test %al,%al;
                // jne 0x0814cc86`（索引 811 = 循环判定块 `end(); _List_const_iterator::ne`）
                // ⇒ 该路径【不推进迭代器】直接回到循环判定 ⇒ 标准等级 ≤0 或 >200 时
                // 同元素重跑（**ORIG 自身在此域外输入下不前进/不终止**）。
                // 此处照抄 ORIG：**不写 ++it**（本循环的 ++it 位于循环体末尾）。
                // 域内输入（1..200）不触发该分支；脚本侧 mv_ac 无 clamp，含域外值即
                // 与 ORIG 同步不终止（复刻优先，安全阈值由运维侧脚本校验保证）。
                continue;
            }
            genLevel =
                (unsigned char)(char)m_dungeon->get_standard_level();
        }
        (void)genLevel;   // ORIG 计算后未再读取（忠实保留）

        stGenerateResult_t result;
        // ORIG consistMap（0x0814bef4 +0xb54..+0xb5a）此处把 mi.m_field04 低字节
        // 写入独立栈标量 -0xb2f(%ebp)——位于 -0xb20(%ebp) 的 result 对象【之外】，
        // 写后全函数无任何读取（Ghidra local_b33 死存储；stGenerateResult_t 布局
        // vector(0xc)+CLuckPoint(0x24)=0x30 已完整覆盖，不存在 +0x0f 成员；
        // +0x0f 落在 CLuckPoint::m_users._M_start 指针内部，不可字段化）。
        // 忠实转写 = 删除该死存储；不得向 result+0x0f 写入（会破坏 vector 头）。

        // [b28d-solo] P0：ORIG 0x0814ca60/ca9d 每处直调 G_CDataManager()->getItemGenerator()。
        G_CDataManager()->getItemGenerator()
            ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_1)
            ->generateSpecificItem(refGen1, result);
        G_CDataManager()->getItemGenerator()
            ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_4)
            ->generateSpecificItem(refGen4, result);

        // [R15a13 · ④ 修复（P13）] ORIG 0x0814caed/0x0814cb08：begin→-0xa??、end→-0xaf0
        //   后 `jmp 814cc30`（条件块只 `ne`）⇒ 同第一处：end() 循环外一次。
        std::vector<Inven_Item>::iterator d = result.m_dropItems.begin();
        const std::vector<Inven_Item>::iterator dEnd =
            result.m_dropItems.end();
        for (; d != dEnd; ++d)
        {
            map_item drop;
            drop.m_count = 1;              // ORIG movb $1（0830b5ab 同型）
            // ORIG 写 map_item+0x01 等级字节：add_item_curr_map 08302f84 /
            // consistMap 0814c9da 同型（来源 _mapItem.m_field0c 或标准等级）
            drop.m_standardLevel = (char)mi.m_field0c;
            // [R19a06 2026-09-13] ORIG 0x0814cb2d..0x0814cb3f 单次读形态
            //   （`mov 0xa34(%eax),%eax; mov %eax,-0xf4c; lea 0x1(%eax),%edx;
            //    mov %edx,0xa34(%eax)`）⇒ 源码为后置自增复合式。
            drop.m_itemIndex = m_stageData.m_dropItemCnt++;
            // [b28d-solo] N84：ORIG 为结构整体赋值（rep movsl），memcpy 是发明 call。
            drop.m_item = *d;
            m_stageData.Add_Item(drop);
        }
        ++it;
    }
    return true;
}