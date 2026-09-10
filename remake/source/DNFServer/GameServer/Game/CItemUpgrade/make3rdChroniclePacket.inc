int CItemUpgrade::make3rdChroniclePacket(CUser* user,
                                         const Inven_Item& item,
                                         PacketGuard* packet)
{
    if (user == 0)
        return 0;
    // ORIG 0x854a9aa：局部 STEnchantSystemMapKey keys[2]（0x10 步长，循环 C1 构造）。
    STEnchantSystemMapKey keys[2];
    // [b28d-solo-CItemUpgrade] ORIG 0x0854a9aa：count 是 **char** 局部 -0x21，
    // [判词] AE（b28d-solo：返回类型 int + 位域视图 + 位域数组视图 m_skills[2]
    //   + m_job/m_equipType enum 视图 + m_bSigned；原 DIFF 233/231 残余 = 4.1 void 早退路径的
    //   `mov $0x0,%ebx` 物化 ×2 + uchar 位与加宽（P168a③）+ 槽号平移 ⇒ N 封顶。
    // 累加形为 `addb $0x1,-0x21`（两处）；fields 指针局部不存在（逐次
    // `mov &item,%eax; movzbl 0x1f/0x20(%eax),%eax`）。
    // enchantId 声明在 count 之前（ORIG 0x0854a9e7 `movl $0x0,-0x28` 早于
    // 0x0854a9ee 的 count `movb $0x0,-0x21`）。
    int enchantId = 0;
    char count = 0;
    // ORIG 0x0854a9f5/0x0854aa07 位测试为 `movzbl; and $0x1; test %al`（无
    // uchar→int 加宽）⇒ 位域视图（探针 c8 实证）。
    if (item.m_random.m_skill0.m_bits.m_flag)
    {
        ++count;
        if (item.m_random.m_skill1.m_bits.m_flag)
            ++count;
    }
    else
    {
        // ORIG 0x0854aa18 `movsbl -0x21(%ebp),%edx` ⇒ 实参是 count 变量（P97）。
        packet->put_byte(count);
        return 0;
    }
    CItem* itemScript = G_CDataManager()->find_item(item.m_addInfo);
    if (itemScript == 0)
        return 0;
    packet->put_byte(count);
    // [b28d-solo-CItemUpgrade] ORIG 0x0854aa74 循环体无 field/kind 具名局部：
    //   `mov i,%eax; mov &item,%edx; movzbl 0x1f(%edx,%eax,1),%eax; shr %al;
    //    and $0x3,%eax` 每分支各自重读（P48 无 CSE），enchantId 是循环外声明的
    //   int 槽 -0x28（ORIG 0x0854a9e7 `movl $0x0,-0x28`），if-else 链末支赋 -1。
    for (int i = 0; i < count; ++i)
    {
        if (item.m_random.m_skills[i].m_bits.m_type == 0)
            enchantId = 0x4e6;
        else if (item.m_random.m_skills[i].m_bits.m_type == 1)
            enchantId = 0x4e7;
        else if (item.m_random.m_skills[i].m_bits.m_type == 2)
            enchantId = 0x4e8;
        else if (item.m_random.m_skills[i].m_bits.m_type == 3)
            enchantId = 0x4e9;
        else
            enchantId = -1;
        keys[i].m_a = user->get_charac_job();
        keys[i].m_b = (unsigned char)user->getCurCharFirstGrowType();
        keys[i].m_c = itemScript->GetItemType();
        keys[i].m_fieldC =
            (unsigned char)item.m_random.m_skills[i].m_bits.m_skillId;
        packet->put_int(enchantId);
        // ORIG 0x0854aba0 `mov (%eax),%eax; mov %eax,%edx` 两段 ⇒ enum 视角读取
        // （P254；int 字段 4.4 恒折单段）。
        packet->put_byte(keys[i].m_job);
        // ORIG 0x0854abc2 `movzbl 0x4(%eax),%eax; movsbl %al,%edx`（零扩展读 +
        // 符号扩展到 int）⇒ m_b 按 signed char 语义传出（视图，字段保持 uchar）。
        packet->put_byte((int)keys[i].m_bSigned);
        packet->put_byte(keys[i].m_equipType);
        packet->put_byte(keys[i].m_fieldC);
    }
    return 1;
