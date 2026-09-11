// ORIG execute 0x084b2a08：全服掉落测试两轮采样（默认掉率 → cltr_normal.txt；
// ref+0x3c 置 1 金币档 → cltr_gold.txt），各 100000 次采样后按 item idx 计数输出。
// b28d-solo-GmCmds_B：按 ORIG 指令形复现 —
// ① 五个独立 if 早退（ORIG 五个 return 岛 +0x977/+0x97a/+0x97d/+0x980/+0x983），
//    非 `||` 短路链（原链式使 3 条件共用一个 setcc/je 岛 + -8 槽漂移）；
// ② 无 battleField 具名槽（ORIG `mov party; add $0xb24` 直取 this）与无 mgr 具名槽
//    （getItemGenerator() 结果直入 (%esp) 后链式取生成器）；
// ③ diff 为 1 字节（ORIG `mov %al,-0x31` 存 / `movzbl -0x31` 读回）⇒ unsigned char；
// ④ 分支方向 `get_dimension_possible() > 0`（ORIG setg，真分支 = party 标准维度等级）；
// ⑤ ref 书写序 +0x35(diff)/+0x34(level)/+0x40(dungeon)/+0x38(float 1.0f 经 eax)/
//    +0x3c 整字写 0（ORIG movl，非两 movw）；
// ⑥ result/statMap/out 单作用域复用（ORIG 一次构造一次析构），第二轮按
//    vector/string/map 序 clear 后重采样；
// ⑦ `char line[1024] = {0}` 置于循环体内（P214③ 内联 rep stos；if 内 memset 保留 call）；
// ⑧ 迭代器后置 ++（ORIG ppEi）+ `find(...) == end()` 判定（ORIG eq + je）；
// ⑨ P0/P183：删 `int itemIdx` 具名局部 —— ORIG 的 find 关键字槽 -0xac/-0x74 是
//    `find(it->m_addInfo)` 的 unsigned→int 隐式转换**临时**（两轮槽位不同即 temp
//    分配特征），保留具名 int 会把槽压到 -0xcc/-0xdc 并使整片临时区漂 4~8 字节
//    （删除后本函数栈槽表与 ORIG 逐槽全等）。
// 残差（fresh .o 597/595，gap +2）：ORIG 两轮各 1 条 `out += line;` 后的
//  `jmp +1; nop` 空块中继岛（0x084b2e90 `eb 01` + 0x084b2e92 `90`，同 GmCmds_A
//  _selectJewelTypeIndex 的 P184 结论：4.1 空块发射产物，4.4 前端 DCE，17 形
//  探针 + 空 else/空块/do-while(0)/continue/空 switch 全败）⇒ P184 封顶；
//  两轮采样循环的 `mov (%eax),%edx` vs 我方 `mov (%eax),%eax`（vtable fn 指针
//  寄存器角色）为 4.1↔4.4 reload 差，6 形调用写法实测不迁移 ⇒ N7 封顶。
void CClearRewardTest::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    if (user->getCurCharacR() == 0)
    {
        return;
    }
    if (user->get_state() != 5)
    {
        return;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return;
    }
    if (party->m_battleField.m_pDungeon == 0)
    {
        return;
    }
    CDungeon* dungeon = party->m_battleField.m_pDungeon;

    // CBattle_Field 子对象位于 CParty+0xb24（经 m_battleField 视图访问）。
    unsigned char diff = party->m_battleField.get_dungeon_diff();

    int standardLevel = 0;
    // ORIG `call get_dimension_possible; test %al,%al; setg %al` ⇒ 比较是**有符号**
    // 视角（头文件声明 unsigned char，unsigned 与 0 比较在 4.4 恒折 setne）。
    // 类型轴直证：ORIG 0x0859f1ef 调用点 `call 0x826b938; movsbl %al,%eax; sub $1`
    // ⇒ 返回类型是 **signed char**（unsigned char 会走 movzbl）；31 个调用点全 setg。
    // 头（CDungeon.h `unsigned char get_dimension_possible()`）待父泳道裁决；
    // 本 TU 按 ORIG 视图比较（头修正为 char 后可撤）。
    if ((char)dungeon->get_dimension_possible() > 0)
    {
        standardLevel = party->getStandardDimensionLevel();
    }
    else
    {
        standardLevel = dungeon->get_standard_level();
    }

    WongWork::stGenerateRefData_t ref;                    // ORIG 0814a706 ctor
    ref.m_field35 = (char)diff;                           // +0x35（难度差【推断】）
    ref.m_field34 = (char)standardLevel;                  // +0x34（基准值）
    ref.m_dungeon = dungeon;                              // +0x40
    ref.m_rankBonusRate = 1.0f;                           // +0x38（float 1.0f 经 eax 中转）
    ref.m_field3c = 0;                                    // +0x3c 整字写 0

    WongWork::CItemGenerator* generator = G_CDataManager()->getItemGenerator()
        ->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2);

    // ---- 第一轮：默认掉率 → cltr_normal.txt ----
    WongWork::stGenerateResult_t result;
    for (int i = 0; i < 100000; ++i)
    {
        generator->generateRandomItem(ref, result);
    }

    std::map<int, int> statMap;
    statMap.clear();                                      // ORIG ctor 后显式 clear
    for (std::vector<Inven_Item>::iterator it = result.m_dropItems.begin();
         it != result.m_dropItems.end(); it++)
    {
        std::map<int, int>::iterator mit = statMap.find(it->m_addInfo);
        if (mit == statMap.end())
        {
            // ORIG make_pair 实例 mangled `IRmiE`（unsigned long&）：Inven_Item.h
            // `unsigned int m_addInfo`（4 字节）与 ORIG 的 32 位 unsigned long 同宽，
            // 此处按 ORIG 视图取引用（P235 视图手法；头若修为 unsigned long 可撤）。
            statMap.insert(std::make_pair((unsigned long&)it->m_addInfo, 1));
        }
        else
        {
            ++mit->second;
        }
    }

    std::string out;
    for (std::map<int, int>::iterator it = statMap.begin(); it != statMap.end(); it++)
    {
        char line[1024] = {0};                            // ORIG 内联 rep stos（P214③）
        if (it == statMap.begin())
        {
            memcpy(line,
                   "    Item Index\t\t                     Item Name\t\tRarity\t\tCount\n",
                   0x3f);
            out += line;
            memset(line, 0, 0x400);
        }
        CItem* item = G_CDataManager()->find_item(it->first);
        if (item != 0)
        {
            sprintf(line, "%14d\t\t%30s\t\t%6d\t\t%5d\n", item->get_index(),
                    item->GetItemName(), item->get_rarity(), it->second);
            out += line;
        }
    }
    printClient(GetUser(), "cltr_normal.txt", out.c_str());

    // ---- 第二轮：ref+0x3c 置 1（金币基数）→ 清空容器后重采样 ----
    ref.m_field3c = 1;                                    // ORIG movl $0x1,-0x120(%ebp)
    result.m_dropItems.clear();
    out.clear();
    statMap.clear();

    for (int i = 0; i < 100000; ++i)
    {
        generator->generateRandomItem(ref, result);
    }

    for (std::vector<Inven_Item>::iterator it = result.m_dropItems.begin();
         it != result.m_dropItems.end(); it++)
    {
        std::map<int, int>::iterator mit = statMap.find(it->m_addInfo);
        if (mit == statMap.end())
        {
            statMap.insert(std::make_pair((unsigned long&)it->m_addInfo, 1));
        }
        else
        {
            ++mit->second;
        }
    }

    for (std::map<int, int>::iterator it = statMap.begin(); it != statMap.end(); it++)
    {
        char line[1024] = {0};                            // ORIG 内联 rep stos（P214③）
        if (it == statMap.begin())
        {
            memcpy(line,
                   "    Item Index\t\t                     Item Name\t\tRarity\t\tCount\n",
                   0x3f);
            out += line;
            memset(line, 0, 0x400);
        }
        CItem* item = G_CDataManager()->find_item(it->first);
        if (item != 0)
        {
            sprintf(line, "%14d\t\t%30s\t\t%6d\t\t%5d\n", item->get_index(),
                    item->GetItemName(), item->get_rarity(), it->second);
            out += line;
        }
    }
    printClient(GetUser(), "cltr_gold.txt", out.c_str());
}