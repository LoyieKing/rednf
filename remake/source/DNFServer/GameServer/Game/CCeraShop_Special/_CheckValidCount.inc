// ============================================================================
// _CheckValidCount —— ORIG 0x08323af4 (T)
//   双重校验：1) 条目 count 累计（IsSpecialItem 分组），任一分组 >1 则失败；
//   2) 对每个 stack-buyable 商品，按 restrictCode 校验限制购买仍为可重复。
//   返回 true=通过；其间任何限制冲突返回 false。
// ============================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08323af4 orig/ours=320/327 ndiff=311。
// 帧/槽号平移（P112/N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82] 324/331；P214③ rep stos 与 P0
// type/value 循环外前置清零（ORIG -0x44/-0x48）已复现，P167 CLEAR 33/33；
// 余帧 0xbc/0xcc（16k 量化不可凑）+ 4.1/4.4 槽复用序差。
bool CCeraShop::_CheckValidCount(const CCeraShopGoods** goodsList)
{
    // P214③：ORIG 0x08323af4 用 `mov $0,%eax; mov $0xf,%edx; rep stos`（0x3c 字节）
    // ⇒ 显式零初始化形（memset 会退化为 call）。
    int typeCount[15] = {0};

    // ORIG 0x08323b16/0x08323b1d：type/value 在**循环外**一次性清零
    // （`movl $0,-0x44` / `movl $0,-0x48`），循环内仅 specialFlag 逐轮清零
    // （0x08323c1b `movb $0,-0x55`）⇒ 两出参声明在循环外。
    int type = 0;
    int value = 0;
    int n = 0;
    // ORIG 0x08323cd3/0x08323c09：循环条件仅 `n <= 0x22`（setle 有符号），
    // `goodsList[n]==0` 是体首独立 break（0x08323b3d/0x08323b5e 各落 nop 岛）。
    for (; n <= 0x22; ++n)
    {
        if (goodsList[n] == 0)
            break;
        for (int j = n + 1; j <= 0x22; ++j)
        {
            if (goodsList[j] == 0)
                break;
            if (goodsList[n]->GetItemIdx() == goodsList[j]->GetItemIdx())
            {
                // ORIG 0x08323ba8：GetGroupIdx() 结果留 %eax 连比 3/7/0（无槽）
                // ⇒ switch（P251：无槽多路相等判断原码是 switch）。
                switch (goodsList[n]->GetGroupIdx())
                {
                case 3:
                {
                    // 实测：帧 0xcc→0xbc（=ORIG）；报告 diff 207/205→200/199；
                    // our 326→325（ORIG 324）；TU 四数不变 ⇒ 保留。
                    // P0：ORIG 0x08323be1 `mov %eax,-0x24(%ebp)` 后回读 0x08323be9
                    // ⇒ 具名槽往返（4.4 的 register 提示会令其免槽，与本处 ORIG 相悖）。
                    CItem* item = G_CDataManager()->find_item(goodsList[n]->GetItemIdx());
                    // ORIG 0x08323be9 无判空（P140/P171：不补边界防御）。
                    if (!item->is_stackable())
                        return false;
                    break;
                }
                case 7:
                case 0:
                    return false;
                }
            }
        }

        bool specialFlag = false;
        // ORIG 0x08323c32：this 用全局 `GlobalData::s_pCeraShop`（内存装载），
        // 非隐式 this（0x8(%ebp)）；`xor $1; test; jne` ⇒ 否定早退形。
        if (!GlobalData::s_pCeraShop->IsSpecialItem(
                goodsList[n]->GetItemIdx(), type, value, specialFlag))
            continue;
        {
            ++typeCount[type];
            if (specialFlag)
            {
                // ORIG 0x8323cf4：以 std::max/std::min 实调钳制
                // min(1, max(count, count-1))（special 商品出现即回 1，
                // 不参与限购超额判定；两算法调用点照抄）。
                // P0/P261：ORIG 0x08323ca8 `call std::max` 的返回引用经 %eax 直传
                // （无具名 mx 槽）；0x08323cad `movl $0x1,-0x3c(%ebp)` 是 std::min
                // 第一实参（const int&）的**常量临时**，非具名 one（0x08323cc5
                // `mov (%eax),%eax; mov %eax,-0xa4(…,4)` 直落）⇒ 两具名局部删除。
                int dec = typeCount[type] - 1;
                typeCount[type] = std::min(1, std::max(typeCount[type], dec));
            }
        }
    }

    for (int i = 1; i < 0xf; ++i)
    {
        if (typeCount[i] > 1)
            return false;
    }

    // 可叠购买商品按 restrictCode 校验（CItemList::GetRestrictCode）。
    typedef std::pair<const CCeraShopGoods*, unsigned int> GoodsCount;
    std::vector<GoodsCount> counts;
    for (int i = 0; i < 0x23; ++i)
    {
        if (goodsList[i] == 0)
            continue;
        // ORIG 0x08323d4e..0x08323d95：`call isStackBuyable; xor $1; test; jne <真>`
        // + GetRestrictCode 后 `test; je <假>` ⇒ 真值条件为
        // `!isStackBuyable() || GetRestrictCode(...) != 0`（P258/P292 短路物化，
        // 无 bool 具名槽）。原 `A && B` 嵌套形语义不同（不可叠/无 restrictCode 的
        // 商品被漏计），属真语义偏差，按 ORIG 直证修正。
        if (!goodsList[i]->isStackBuyable() ||
            G_CDataManager()->m_itemList->GetRestrictCode(
                goodsList[i]->GetItemIdx()) != 0)
        {
            // 按商品指针找到已有计数并自增；ORIG 以循环后 it==end() 判缺
            // （operator== 调用点，无 bool 标志；begin/end 各一次、判缺复用
            // 迭代器无 end 重发）。
            std::vector<GoodsCount>::iterator itBeg = counts.begin();
            std::vector<GoodsCount>::iterator itEnd = counts.end();
            for (; itBeg != itEnd; ++itBeg)
            {
                if (itBeg->first == goodsList[i])
                {
                    ++(itBeg->second);
                    break;
                }
            }
            if (itBeg == itEnd)
                counts.push_back(GoodsCount(goodsList[i], 1));
        }
    }

    // P13：ORIG 0x08323e83/0x08323e98 循环外一次构造 begin/end 两具名迭代器
    // （-0x5c/-0x60），条件段只调 operator!=。
    for (std::vector<GoodsCount>::iterator it = counts.begin(),
             itEnd = counts.end();
         it != itEnd; ++it)
    {
        if (it->second > 1)
            return false;
    }

    return true;
}