// [b28d-20] _isPossibleResultItem（ORIG 0x832db4e，140/135，nd 27→14）：
//   本轮修 P5 裸虚表→`item.IsRandomOption()`、P50 先声明后赋值（复现 ORIG
//   0x832db8d 独立 `iterator::C1Ev`）、正极性返回 `if (IsRandomOption())
//   return false; return true;`（ORIG 0x832dd09 `test; je <mov $1>`）。
//   残差 = it/endIt 槽互换（ORIG it=-0x14/endIt=-0x10，4.4 先声明占浅槽）
//   与帧 0x38/0x28 ⇒ N93 封顶（交换声明序会倒转 C1Ev/end 调用序，同
//   FindGoods 实证）。
// [b28d-solo-CItemList] NEAR | 修：尾 return 改 if-return 补 ORIG 的
//   bool 物化岛（sete→test/je/mov$1/jmp/mov$0；27→7 差异行）；残 N93 槽。
bool CCompound_ExtreamDun_Item::_isPossibleResultItem(              // ORIG 0x832db4e
    CEquipItem& item) const
{
    if ((unsigned int)item.getItemGroupName() > 0x3a ||
        item.getItemGroupName() <= 0)
    {
        return false;
    }

    // 强制规则命中即一票裁决：flag==0（强制允许）→ true，非零 → false。
    // [b28d-20] P50 复活：ORIG 0x832db8d 有独立 `iterator::C1Ev`（lea -0x14;
    //   call C1Ev）先于 end()，⇒ 原码 it 先声明后赋值；it@-0x14/endIt@-0x1c。
    // [b28d-tu-CItemList] P326 翻案实测（本件不适用 ⇒ 回滚具名形）：
    //   P326 形 `it != const_iterator(m.end())` 把 end() 挪到比较点，但
    //   ORIG 0x0832db8d 起入口序为 C1Ev(it)→end→find→cmp（end 构造在
    //   find **之前**）⇒ 内联形多 2 处调用序倒转，ndiff 14→20、NEAR→DIFF
    //   倒退。具名 endIt 形（4.4 先声明占浅槽 -0x10? 实测 it=-0x10/
    //   endIt=-0x14 反槽）保留为最优 NEAR/nd14 形。N93 判词对本件仍有效。
    std::map<int, forceRule_ResultItem>::const_iterator it;
    std::map<int, forceRule_ResultItem>::const_iterator endIt =
        m_data.m_forceRules.end();
    it = m_data.m_forceRules.find(item.get_index());
    if (it != endIt)
    {
        if (it->second.m_flag != 0)
        {
            return false;
        }
        if (it->second.m_flag != 1)
        {
            return true;
        }
    }

    if (_convertInputItem(
            (ENUM_UNUSABLE_EQUIPMENT_TYPE)item.GetUsableEquipmentType()) == 2 &&
        item.getItemGroupName() > 0x19 && item.getItemGroupName() < 0x33)
    {
        return false;
    }

    if (item.getUsableLevel() > 0x46)
    {
        return false;
    }

    if (item.get_gen_rate() == 0)
    {
        if (_convertInputItem(
                (ENUM_UNUSABLE_EQUIPMENT_TYPE)
                    item.GetUsableEquipmentType()) == 1)
        {
            return true;
        }
        return false;
    }

    // gen_rate!=0 → 虚表槽 +0x4c 回调裁决（非零→false，零→true）。
    // [b28d-20] P5 直调：ORIG 0x832dcf7 `mov (%eax),%eax; add $0x4c,%eax;
    //   mov (%eax),%edx; … call *%edx` = 虚成员直调，非裸函数指针缓存；
    //   返回按 ORIG 0x832dd09 分支形态（`test %al,%al; je <mov $1>`）写 if/return。
    if (item.IsRandomOption())
    {
        return false;
    }
    return true;
}