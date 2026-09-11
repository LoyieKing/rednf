// ============================================================================
// ProcUpgrade_3rdChronicle @ 0x0854a006（ProcUpgrade_3rdChronicle.md）
//   三代强化落点：按 key(job/growType/equipType/param) 查 itemScript+0x2f4 附魔表；
//   命中 → item+0x1f 或 item+0x20（按首字节 &1 二选一）写入附魔类型/槽位，返回 1；
//   未命中 → 0。
// [b28d-solo-CItemUpgrade 判词] AE（P285 正向 if/else{return 0} 尾 return 1 +
//   val 作用域收窄 + 位域视图）。原 DIFF 134/133：
//   本轮修：非 const map/迭代器（P96/P270，消 p167 MISSING/EXTRA 各 4）、位域赋值
//   复现三段 RMW、内联 map2f4/stackItem/f 局部。残余 = N93附（return 1 的
//   `mov $1,%ebx` 在 vector 析构之后 vs 4.4 之前）+ %al/%dl 寄存器名 ⇒ N 封顶。
// ============================================================================
// [P248 档①/P292] 调用点 ORIG 0x0854a743 `call; xor $0x1,%eax; test %al,%al; je`
//   ⇒ 返回类型 bool（int 返回恒产 sete 三段）。
bool CItemUpgrade::ProcUpgrade_3rdChronicle(CUser* user, ENUM_EQUIPMENTTYPE equipType,
                                            unsigned char param, Inven_Item& item,
                                            const CItem* itemScript)
{
    // ORIG 0x0854a006：四字段键（job/growType/equipType/param）查
    // CStackableItem+0x2f4 附魔 map（键 STEnchantSystemMapKey，值
    // STEnchantSystemMapData 含 vector<EquipmentSkillDataUp>）；命中 →
    // item+0x1f 或 item+0x20（按首字节 &1 二选一）写入附魔类型/槽位，返回 1；
    // 未命中 → 0。值 vector 仅作命中判定（拷贝后立即析构），不读取内容。
    STEnchantSystemMapKey key;
    key.m_a = user->get_charac_job();
    key.m_b = user->getCurCharFirstGrowType();
    key.m_c = (int)equipType;
    key.m_fieldC = param;

    // [b28d-solo-CItemUpgrade] ORIG 0x0854a048 无 map2f4 引用/无 f 指针局部
    // （P0）：find/end 各自重算 `mov itemScript,%eax; lea 0x2f4(%eax),%ecx`，
    // 字段访问逐次 `mov &item,%eax; movzbl 0x1f(%eax)`。
    // [b28d-solo-CItemUpgrade] ORIG 0x0854a062 用**非 const** map/迭代器
    // （`map::findERS5_` + `_Rb_tree_iterator::ne/ptEv`）；const 版会换成
    // `findERKS5_` + `_Rb_tree_const_iterator`（p167 MISSING/EXTRA 各 4）。
    std::map<STEnchantSystemMapKey, STEnchantSystemMapData>::iterator it =
        ((CStackableItem*)itemScript)->m_enchantMap.find(key);

    if (it != ((CStackableItem*)itemScript)->m_enchantMap.end())
    {
        // ORIG：命中后拷贝值 vector（仅作命中判定，内容不读取）再修改 item。
        // itemScript 实参为 CStackableItem*（ORIG 逐次读 +0x30c 附魔类型字节，
        // 无具名 stackItem 局部）。
        {
            std::vector<EquipmentSkillDataUp> val(it->second.m_vec0);
            // [b28d-solo-CItemUpgrade] ORIG 0x0854a0bc..0x0854a169 三段 RMW 是
            // **位域赋值**形：`or $0x1` 置 bit0 / `and $0xfffffff9; or` 置 bit1-2 /
            // `and $0x7; lea (,%edx,8)` 置 bit3-7。写裸 `field |= 1; field =
            // (field&0xf9)|...` 4.4 每条多 `mov %eax,%edx` 中转 + 重载 this
            // （144 vs 134）。位域成员即 m_bits{m_flag:1,m_type:2,m_skillId:5}。
            if (item.m_random.m_skill0.m_bits.m_flag == 0)
            {
                item.m_random.m_skill0.m_bits.m_flag = 1;
                item.m_random.m_skill0.m_bits.m_type =
                    static_cast<const CStackableItem*>(itemScript)->m_char30c;
                item.m_random.m_skill0.m_bits.m_skillId =
                    param;
            }
            else
            {
                item.m_random.m_skill1.m_bits.m_flag = 1;
                item.m_random.m_skill1.m_bits.m_type =
                    static_cast<const CStackableItem*>(itemScript)->m_char30c;
                item.m_random.m_skill1.m_bits.m_skillId =
                    param;
            }
        }
    }
    else
    {
        return 0;
    }
    // [b28d-solo-CItemUpgrade] P285 形：正向 if + `else{return 0;}` + 函数尾
    // `return 1`。ORIG 0x0854a172 `call ~vector` 之后是 `jmp a180`（跳到
    // mov $1 块，unmatched 的 mov $0 块夹在中间）——直接双 return 时 4.4 把
    // mov $1 放 fall-through（少 1 条 jmp，N93附）；正向包裹后逐条复现。
    return 1;
}