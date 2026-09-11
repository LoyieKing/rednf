// [b28d-20] insert_item（ORIG 0x0851129e，309/309，nd 43）：本轮修 P5 裸虚表
//   → `item->IsRandomOption()`（CItem vtable +0x4c 第 19 槽）与 P167
//   `operator==` 形；p167 调用点差集 45/45 **CLEAR**（0 missing/0 extra）。
//   残差 = 尾部 map 插入段的槽号/寄存器分配（BaseItemKey 临时 + inner map）⇒
//   语义完成、byte 层 N7/N82 封顶。
// [b28d-solo-CItemList] DIFF | 修：正向早返守卫（消 xor$1 物化；p167 CLEAR
//   45/45）；残 N7/N82 槽与寄存器分配。
bool CItemList::insert_item(int group, int idx, CItem* item)
{
    __gnu_cxx::hash_map<int, CItem*>::iterator it;
    it = m_gradeItemList[group].find(idx);
    // [b28d-20] P167：ORIG 调用 operator==（无 ne）；[b28d-solo] 改正向早返守卫：
    //   ORIG 0x081fxxxx `test %al,%al; je <fail 块 +0x154>`（无 xor$1 物化）⇒
    //   源码为 `if (it == end()) { fail; return false; }` 正极性形（P59/P222）。
    // [b28d-tu-CItemList] 旧「块序 N104 封顶」判词已由 [b28d-tu-CItemList3
    //   2026-09-11] 推翻（旧三形：① if-then 形 ⇒ fail 段落函数尾 + quest 内联；
    //   ② `if (!(it==end()))` ⇒ 多 xor 物化；③ `if (it!=end())` ⇒ 发明 `_Ne`
    //   符号触铁律1）。改显式 goto 块序后 ndiff 398→8、gap −1（父泳道编译读数；
    //   TU 四数 identical/ae/near/diff 逐项不变）。p167 45=45，10 对
    //   C2↔C1/D2↔D1 镜像别名豁免后零差（CItemList/insert_item.md 汇编段 +
    //   objdump 0x0851129e 直证）。
    // [b28d-tu-CItemList2 2026-09-09] 真语义修正：ORIG 0x08511343
    //   `test %al,%al; je 0x085113f2`（fail 段）⇒ `it == end()` 时 fall-through
    //   进插入段、`it != end()`（已存在）才打印 "Insert Fail" 并 return false。
    //   条件仍调 operator==（ORIG 无 `ne` 符号），不发明符号。
    // [b28d-tu-CItemList3 2026-09-11 块序] ORIG 物理块序 = insertBlock(0x0ab)
    //   → failBlock(0x154) → questBlock(0x198) → afterQuest=IsRandomOption
    //   (0x214)；4.4 对 `if(cond){body}` 恒把 body 内联并置 fail 段于函数尾，
    //   导致 ORIG 侧 `jne 0x08511436`/`jmp 0x085114b2` 两跳布局无对应
    //   （ndiff 398 全为 difflib 块错位放大）。源级 goto 是显式排布唯一手段：
    //   `if (==end) goto insertBlock;`（4.4 产 test; jne）+ `goto failBlock;`
    //   （jmp）复现 ORIG 的 0x08511343 `je <fail>` 落点后块序。
    if (it == m_gradeItemList[group].end())
    {
        goto insertBlock;
    }
    goto failBlock;

insertBlock:
    m_gradeItemList[group].insert(std::pair<const int, CItem*>(idx, item));
    m_itemMap.insert(std::pair<const int, CItem*>(idx, item));
    if (item->GetQuestItemDropBouns())
    {
        goto questBlock;
    }
    goto afterQuest;

failBlock:
    LogManager::logFormat(1, "item.cpp",
        __PRETTY_FUNCTION__, 0x81f,
        "Insert Fail Item Group[%d] Index: %d", group, idx);
    return false;

questBlock:
    // [SweepCItemList2 2026-09-10] 真语义 bug 修正（判据⑳ make_pair 键值序）：
    //   ORIG 0x08511441..0x0851146d 直证：call GetQuestItemDropBouns → `mov
    //   %eax,-0x8c`；`mov 0x10(%ebp),%eax → -0x88`；压栈 `0x8(%esp)=&-0x8c`
    //   (arg2) / `0x4(%esp)=&-0x88` (arg1) ⇒ arg1 = **idx**、arg2 = questBonus
    //   （实例 `_ZSt9make_pairImiE…` 要求 arg1 为 unsigned long ⇒ 源码
    //   `make_pair((unsigned long)idx, (int)questBonus)`）。语义互证：
    //   CUser.cpp:8529 查询键是 `Inven_Item::m_addInfo`（Inven_Item.h:269 =
    //   item idx）⇒ 本表键必须是 idx、值才是 questBonus；原键值互换使
    //   GetQuestItemDropBouns(idx) 恒查不到。
    m_lottoCount.insert(
        std::make_pair((unsigned long)idx,
                       (int)item->GetQuestItemDropBouns()));

afterQuest:
    // [b28d-20] P5 直调：ORIG 0x081f???? `mov (%eax),%eax; add $0x4c,%eax;
    //   mov (%eax),%edx; … call *%edx` = CItem vtable +0x4c = IsRandomOption
    //   （CItem.h 虚序第 19 槽）；裸函数指针强转已删。
    if (item->IsRandomOption())
    {
        BaseItemKey key(item->get_rarity(), item->getUsableLevel(),
                        item->getItemGroupName());
        std::map<int, std::map<int, BaseItemValue> >::iterator it2 =
            ((CDataManagerExt*)G_CDataManager())
                ->m_baseItemInfo.find(*(int*)&key);
        int value = item->get_index();
        if (it2 !=
            ((CDataManagerExt*)G_CDataManager())->m_baseItemInfo.end())
        {
            it2->second.insert(std::make_pair<int, BaseItemValue&>(
                item->get_grade(), *(BaseItemValue*)&value));
        }
        else
        {
            std::map<int, BaseItemValue> inner;
            inner.insert(std::make_pair<int, BaseItemValue&>(
                item->get_grade(), *(BaseItemValue*)&value));
            ((CDataManagerExt*)G_CDataManager())
                ->m_baseItemInfo.insert(
                    std::make_pair<int&, std::map<int, BaseItemValue>&>(
                        *(int*)&key, inner));
        }
    }
    return true;
}