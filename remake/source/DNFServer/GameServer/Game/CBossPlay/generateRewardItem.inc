// ============================================================================
// generateRewardItem（ORIG 0x08149914）
// [b28d-46 判词·N(死 ebx 物化) + 3 真 bug] 327 vs 324（gap 3）、p167 44:44 CLEAR
// （仅 C1/D1↔C2/D2 同址别名，P93）、帧 0x13c 两侧全等。本轮修：
// ① 真 bug·虚槽互换：ORIG vtable `_ZTVN8WongWork15CItemGeneratorE` @0x08c9a788
//    word[4]=0x0853a400 generateRandomItem（vptr+0x08）、word[5]=0x0853a406
//    generateSpecificItem（vptr+0x0c）；ORIG 段 A（0x08149a87 `add $0xc`）与
//    段 B（0x08149c03 `add $0x8`）各调其一，原实现两分支调用名互换。
// ② 真 bug·舍入：ORIG 0x08149b18 为 `fildl; fmuls; 设截断 CW; fistpl`
//    （= (int)(float) 截断），原 ROUND(+0.5f) 宏（fadd 0.5 + fistpll）错误。
// ③ 形态：外层条件原 `< || ==` 双算 imul 且块序反 ⇒ 单条件 + 随机块 fall-through
//    （P328⑧/P272）；内层同理改 `rare2*b > randVal`。
// ④ P0/P52/P248：rare/rare2/randVal 同型化 unsigned（消 edx 中转）、rare 三段链式
//    （消 dungeonIdx/dm/tower 具名槽）、ref/result 先于 get_rand_int 构造、
//    dst 去具名引用（逐点重算地址）、特定分支补 `Inven_Item rewardItem = *begin()`
//    值拷贝（ORIG 0x08149c9b..0x08149d25 共 17 条 mov）、随机分支独立 randResult
//    对象（-0x9c 自带 ctor/dtor）、删 m_party 判空（P140/P171）、
//    addInfo 回写同槽（ORIG fildl/fistpl 同址 -0x12c）。
// 残差 gap 3 = ORIG 段 A/B 公共出口 `mov $0x1,%ebx` / item==0 路径 `mov $0x0,%ebx`
//  + 循环尾 `test %ebx,%ebx`（**无消费者死物化**：4.1 条件值保留；ebx 全程无栈槽
//  ⇒ 非具名局部，4.4 无对应源形，同 P197④/跨版本族）⇒ 语义闭合，封 N。
// [L5Free49to60 2026-09-11 复核] fresh .o 327 vs 324（gap 3）不变；bcmp 逐条对比：
// 指令序列全等，唯一实质差 = 全帧槽相位 4B —— ORIG 把 addInfo 排到深区 -0x12c
// （fildl/fistpl 同址，ORIG 0x08149b18 段）而 4.4 分到浅区 -0x20；帧 sub 两侧同为
// 0x13c（ORIG 深区多 4B、我方浅区多 4B）。属 N82附（4.4 聚合对象优先占低槽、标量
// 分区排布；声明序/位置为 N98 不控）⇒ 维持封 N。
// ============================================================================
bool CBossPlay::generateRewardItem(int memberIdx, int itemCount, unsigned int a,
                                  ENUM_DUNGEON_DIFFICULTY diff, unsigned int b,
                                  const CDungeon* dungeon)
{
    m_memberReward[memberIdx].m_count = itemCount;
    // [b28d-46 P248 档③] ORIG 比较段 `mov rare,%eax; imul b,%eax; cmp randVal,%eax`
    // 无 edx 中转 ⇒ rare/rare2 与 b 同型（unsigned int）。
    unsigned int rare = 0;
    unsigned int rare2 = 0;
    float rare3 = 0.0f;
    // 掉宝概率三档累计表 / 金币费率（ORIG 0x08149971/0x08149992/0x081499b9
    // 分别 `mov 0xc88(%eax)`、`mov 0xc8c(%eax)`、`mov 0xc94(%eax)`，
    // 全为 4 字节读 ⇒ m_rewardItemProb[0]/[1]（int）、m_rewardGoldWeight（float））。
    // [b28d-46 P0/P52] ORIG 无 dungeonIdx/dm/tower 具名局部：get_index 结果留
    // %ebx 跨 G_CDataManager call 保活（`mov %eax,%ebx` 无栈往返）⇒ 逐条链式。
    rare = G_CDataManager()->GetTowerScript(dungeon->get_index())->m_rewardItemProb[0];
    rare2 = G_CDataManager()->GetTowerScript(dungeon->get_index())->m_rewardItemProb[1];
    rare3 = G_CDataManager()->GetTowerScript(dungeon->get_index())->m_rewardGoldWeight;
    for (int i = 0; i < itemCount; ++i)
    {
        // [b28d-46 P0] ORIG 构造序：ref ctor → result ctor → get_rand_int
        // （0x081499ce/0x081499e2 在 0x081499ee 之前）⇒ 标量声明置于对象之后。
        stGenerateRefData_t ref;
        stGenerateResult_t result;
        unsigned int randVal = (unsigned int)get_rand_int(1000);

        // [b28d-46 P328⑧] ORIG 单次 imul + `cmp -0x20(%ebp),%eax; jbe`（无符号
        // `rare*b <= randVal`）；原 `< || ==` 写法双算 imul 且块序反。
        // [b28d-46 P272] ORIG 块序：随机块 fall-through、`jbe` 落特定判断块
        // ⇒ 源码须写成 `if (rare*b > randVal) {随机} else if (rare2*b > randVal)
        // {特定}`；嵌套 `<=` 形 4.4 反产 `ja 随机块`（块序相反）。
        if ((unsigned int)(rare * b) > (unsigned int)randVal)
        {
            // 随机掉落分支（ORIG +0x0f2 起，ref 基址同上 -0x124）：
            //   movb -0xef ⇒ +0x35=difficulty；movb -0xf0 ⇒ +0x34=a；
            //   movb -0xee ⇒ +0x36=member_count；movl $0x1 -0xec ⇒ +0x38=1；
            //   4×movw $0x1 ⇒ +0x3c/+0x3e/+0x40/+0x42 各 =1
            //   （+0x40/+0x42 合成指针位型 0x10001，随机路径不解引用）；
            // 虚槽 +0x8 = generateRandomItem。
            ref.m_field35 = (char)(unsigned char)diff;
            ref.m_field34 = (char)a;
            // [b28d-46 P140/P171] ORIG 无 m_party 判空（0x08149a18 直接
            // `mov 0x4(%eax),%eax; mov %eax,(%esp); call get_member_count`）。
            ref.m_rateSlot = (char)m_party->get_member_count();
            ref.m_rankBonusIndex = 1;
            ref.m_goldBase = 1;
            ref.m_goldFactor = 1;
            ref.m_field40 = 1;
            ref.m_field42 = 1;
            // [b28d-46] ORIG 随机分支自带独立 result 对象（-0x9c，ctor 0x08149a63 /
            // dtor 0x08149b7f 均在分支内），与循环级 result（-0x6c）并存。
            // [b28d-46 真 bug] ORIG vtable（_ZTVN8WongWork15CItemGeneratorE @0x08c9a788）
            // word[4]=0x0853a400 generateRandomItem（vptr+0x08）、word[5]=0x0853a406
            // generateSpecificItem（vptr+0x0c）；本段 ORIG 0x08149a87 `add $0xc`
            // ⇒ 调 generateSpecificItem（原实现调 generateRandomItem 为虚槽取错）。
            stGenerateResult_t randResult;
            G_CDataManager()->getItemGenerator()->getGenerator(
                CItemGeneratorMgr::eGenerateType_2)->generateSpecificItem(ref, randResult);
            m_memberReward[memberIdx].m_items[i].setCopy(
                *randResult.m_dropItems.begin());
            int addInfo = m_memberReward[memberIdx].m_items[i].get_add_info();
            // [b28d-46 真 bug] ORIG 0x08149b18 段为 `fildl; fmuls; 设截断 CW;
            // fistpl`（无 fadd 0.5、无 fistpll）⇒ 原码是 `(int)(float)` 截断，
            // 原 ROUND(+0.5f) 宏为舍入错误（少截断多 flds/faddp 两条）。
            // [b28d-46 P0] ORIG `fistpl -0x12c` 与 `fildl -0x12c` 同槽 ⇒ 结果
            // 回写 addInfo 局部再传参（非匿名临时槽）。
            addInfo = (int)((float)addInfo * rare3);
            m_memberReward[memberIdx].m_items[i].set_add_info(addInfo);
        }
        // ORIG 内层同样单次 imul：`mov rare2,%eax; imul b; cmp randVal; jbe 跳过`
        // ⇒ 单条件 `rare2*b > randVal` 才进特定掉落（特定块 fall-through）。
        else if ((unsigned int)(rare2 * b) > (unsigned int)randVal)
        {
            // 特定掉落分支（ORIG +0x2a0 起，ref 基址 = 本帧 -0x124，
            // ctor `lea -0x124(%ebp),%eax` 实证）：
            //   movb -0xef ⇒ +0x35=difficulty；movb -0xf0 ⇒ +0x34=a；
            //   mov $0x3f800000/movl -0xec ⇒ +0x38=float 1.0f；
            //   movl -0xe4 ⇒ +0x40=dungeon；movl $0x0 -0xe8 ⇒ +0x3c=0；
            // 虚槽 +0xc = generateSpecificItem。
            ref.m_field35 = (char)(unsigned char)diff;
            ref.m_field34 = (char)a;
            ref.m_rankBonusRate = 1.0f;
            ref.m_dungeon = const_cast<CDungeon*>(dungeon);
            ref.m_field3c = 0;
            // [b28d-46 真 bug] ORIG 0x08149c03 `add $0x8` ⇒ vptr+0x08 =
            // generateRandomItem（原实现调 generateSpecificItem 为虚槽取错）。
            G_CDataManager()->getItemGenerator()->getGenerator(
                CItemGeneratorMgr::eGenerateType_2)->generateRandomItem(ref, result);
            if (!result.m_dropItems.empty())
            {
                // [b28d-46 P167] ORIG 调 vector::begin + iterator::operator*
                // （各 3 处）⇒ 迭代器解引用形。
                m_memberReward[memberIdx].m_items[i].setCopy(
                    *result.m_dropItems.begin());
                // [b28d-46 P0] ORIG 第二次 begin()+operator* 后把 *begin() 值拷贝
                // 到栈临时（0x08149c9b..0x08149d25 共 17 条 mov，0x3d 字节），
                // 再自临时读 +0x02/+0x01 ⇒ 具名 Inven_Item 局部（非引用）。
                Inven_Item rewardItem = *result.m_dropItems.begin();
                // ORIG +0x3f2：按首项 idx 查 CItemList，命中后按
                // itemType(+0x01)/rarity/grade 扣幸运点（::CLuckPoint）。
                // [b28d-46 真 bug/P167] ORIG 0x08149d42 调 CItemList::find_item
                // （receiver = G_CDataManager()+0xc = m_itemList），非
                // CDataManager::find_item；原实现调错类（EXTRA find_item）。
                CItem* item = G_CDataManager()->m_itemList->find_item(
                    rewardItem.m_addInfo);   // +0x02 = item idx
                if (item != 0)
                {
                    ::CLuckPoint::UseLuckPoint(
                        m_party->get_user(memberIdx),
                        rewardItem.m_itemType, item->get_rarity(), item->get_grade());
                }
            }
        }
    }
    return 1;
}
