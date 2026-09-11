// ============================================================================
// _processBoosterSelect —— ORIG 0x08327894 (T)
// 在 booster 表（m_booster.m_map，item+0x24c，key=pair<char,char>(c1,c2)）中定位，
// 收集其 value 中与 items 现有 Inven_Item（m_addInfo==m_itemIdx）匹配的子物品，
// 清空 items 后逐项 _processAdditionalItem；随后限制商品校验 + SaveInventory。
// 返回：find==end→0；item==0 或处理完成→1。
// ============================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08327894 orig/ours=233/238 ndiff=215。
// 帧/槽号平移（P112/N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82] 233/229 ndiff 85→68；P183/P157 删
// stackable/bmap 具名缓存（ORIG 0x08327894 每处 `mov item,%eax; lea 0x24c` 重算）+
// P323 `static_cast<const M&>` 视图 cast 消 iterator→const_iterator 转换构造；
// P167 CLEAR 34/34；余帧/槽平移封 N82。
// b28d-sweepCCeraShop（本轮逐位复核）233/229：jcc 目标下标逐条对齐（je/je/seta/jne×3/je/je
// 语义全同）、p167 CLEAR、无 `call *`；gap 4 = ORIG 的 esi/ebx 双寄存器共享尾
// （`je +0x310` 直达 `mov $1,%esi` + 尾部 `test %ebx,%ebx` 物化）vs 4.4 单 ebx 直返
// （`jne; mov $1,%ebx; jmp` + `mov %ebx,%eax`）共 ±4，加 +0xd/+0xe 寄存器 vs 槽分配差。
int CCeraShop::_processBoosterSelect(CUser* user, int itemIdx, int count, int& result,
                                     long curSec, bool flag,
                                     std::vector<Inven_Item>& items,
                                     stCeraShopItemParam_t& param, char c1, char c2)
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return 1;

    std::vector<stStackableBoosterItemInfo_t> boosterList;
    // ORIG const 视图（find/end/==/-> 与 vector operator[] 全部 const 形；
    // P167：非 const 形会多发 2 个 const_iterator 转换构造并选错 find/end 重载）。
    // ORIG 0x08327894：命中（it != end）走主流程并置 ok=true，else 置 ok=false，
    // 尾部统一 `if(!ok) return 0; return 1;`（局部 vector 析构在 return 之前）。
    // P183/P157：ORIG 0x08327894 无 stackable/bmap 具名槽——map 基址每处
    // 现场重算 `mov item,%eax; lea 0x24c(%eax),%ecx`（禁指针/引用缓存）。
    // const 视图：ORIG 调 `_ZNKSt3map…4find`/`end`（const 形，无 iterator→
    // const_iterator 转换构造），故按 const CStackableItem* 内联访问。
    // 4.4 对 `static_cast<const C*>(p)->m.method()` 内联形仍选非 const 重载
    // （探针 p2.cpp j vs i）；显式 `static_cast<const M&>` 视图 cast 才出
    // `_ZNKSt3map…find` 且不占额外槽。
    typedef std::map<std::pair<char, char>,
                     std::vector<stStackableBoosterItemInfo_t> > BoosterMap_t;
    BoosterMap_t::const_iterator it =
        static_cast<const BoosterMap_t&>(
            static_cast<const CStackableItem*>(item)->m_booster.m_map).find(
            std::make_pair(c1, c2));
    // ORIG 0x0832794d/0x08327b73/0x08327ba0 的 esi/ebx 共享尾出口为 4.1 对
    // `return 0;`/`return 1;` 的布局产物（4.4 把 return 1 直接落 ebx）⇒ N 封顶。
    if (it == static_cast<const BoosterMap_t&>(
                  static_cast<const CStackableItem*>(item)->m_booster.m_map).end())
        return 0;
    {
        const std::vector<stStackableBoosterItemInfo_t>& src = it->second;
        // ORIG 0x083279fa `setg %al`：外层索引 i 为 int（signed 比较）；内层
        // 0x083279e4 `seta %al` 为 unsigned size_type。
        for (int i = 0; i < (int)items.size(); ++i)
        {
            for (std::vector<stStackableBoosterItemInfo_t>::size_type j = 0;
                 j < src.size(); ++j)
            {
                if (src[j].m_itemIdx == items[i].m_addInfo)
                    boosterList.push_back(src[j]);
            }
        }

        items.clear();

        // P13：ORIG 0x08327a13/0x08327a28 循环外一次构造 begin/end 两具名迭代器
        // （-0x44/-0x48），条件段只调 operator!=（0x08327ae2）。
        for (std::vector<stStackableBoosterItemInfo_t>::iterator b =
                 boosterList.begin(),
             bEnd = boosterList.end();
             b != bEnd; ++b)
        {
            // ORIG 实参顺序：itemIdx(+0), addInfo(+0x8), avatarLevel(+0xe),
            // avatarType(+0xd), items, param, +0xa(flag)
            _processAdditionalItem(user, (unsigned long)b->m_itemIdx, b->m_addInfo,
                                   b->m_avatarLevel, b->m_avatarType, items, param,
                                   b->m_byteA);
        }

        // P0/P171：ORIG 0x08327b13 `mov %eax,0x14(%ebp)`（写回**第 4 形参 count
        // 的槽**）后 0x08327b16 `cmpl $0x0,0x14(%ebp)`，其后 IsRestrictedGoods /
        // SetRestrictedGoods / SaveFeaturedIdx 三处均自 0x14(%ebp) 回读（形参槽
        // 复用，非新局部）⇒ 照抄写回 count；我方新建 restrictCode 局部会多 1 槽
        // 并使整块槽链位移。
        count = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
        // b28d-solo(P225 推广)：`&&` 被 4.4 折成 bool 物化岛（mov $1/jmp/mov $0/
        // test/je，+4 条）；ORIG 两条件各发独立 je 直跳 ⇒ 拆嵌套 if 逐条复现。
        if (count != 0)
        {
            if (!user->IsRestrictedGoods(count))
            {
                user->SetRestrictedGoods(count);
                SaveFeaturedIdx(user, count, (int)curSec, 0);
            }
        }
        user->SaveInventory();
    }
    return 1;
}