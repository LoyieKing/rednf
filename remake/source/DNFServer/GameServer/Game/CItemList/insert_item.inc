bool CItemList::insert_item(int group, int idx, CItem* item)
{
    __gnu_cxx::hash_map<int, CItem*>::iterator it;
    it = m_gradeItemList[group].find(idx);
    // [b28d-20] P167：ORIG 调用 operator==（无 ne）；[b28d-solo] 改正向早返守卫：
    //   ORIG 0x081fxxxx `test %al,%al; je <fail 块 +0x154>`（无 xor$1 物化）⇒
    //   源码为 `if (it == end()) { fail; return false; }` 正极性形（P59/P222）。
    // [b28d-tu-CItemList] 块序三形实测（并列读数，均不可兼得）：
    //   ① 本形 `if (it==end()){fail;return false;} {main}`：test/je 同 ORIG
    //     0x08511341、无 xor，但 4.4 把 fail 块内联主段前 ⇒ 42 行块位移，
    //     ndiff 123/gap 1（**最优，保持**）；
    //   ② `if (!(it==end())){main}fail`：4.4 生 `xor $1,%eax` 物化（ORIG 无）
    //     + 全函数位移，ndiff 123→400 反劣；
    //   ③ `if (it!=end()){main}fail`：改调 iterator::ne ⇒ 发明 ORIG 无的
    //     `_Ne` 符号，触铁律1，弃。
    //   ORIG（4.1）fail@+0x154 后置 = 4.1 块布局启发（热路径前置），4.4 不可
    //   从源码复现 ⇒ N104 封顶。[修正版 p167 复判] 45=45，10 对
    //   C2↔C1/D2↔D1 镜像别名，豁免后零差；取证以 CItemList/insert_item.md
    //   汇编段 + objdump 0x0851129e 直证（§3.2 兜底：本件改动仅回滚实验形，
    //   维持前轮封项）。
    if (it == m_gradeItemList[group].end())
    {
        m_gradeItemList[group].insert(std::pair<const int, CItem*>(idx, item));
        m_itemMap.insert(std::pair<const int, CItem*>(idx, item));
        if (item->GetQuestItemDropBouns())
        {
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
        }
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
    // [b28d-tu-CItemList2 2026-09-09] 真语义修正：ORIG 0x08511343
    //   `test %al,%al; je 0x085113f2`（fail 段）⇒ `it == end()` 时 fall-through
    //   进插入段、`it != end()`（已存在）才打印 "Insert Fail" 并 return false。
    //   原重建把两分支写反（未找到即报错、已存在反而重复插入）。条件仍调
    //   operator==（ORIG 无 `ne` 符号），不发明符号。
    LogManager::logFormat(1, "item.cpp",
        __PRETTY_FUNCTION__, 0x81f,
        "Insert Fail Item Group[%d] Index: %d", group, idx);
    return false;
