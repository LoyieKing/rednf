int CDisjointer::OnUpgradeDisjointMachine(CUser* user)
{
    int grade = GetSelfExpand(user)->get_disjoint_machine_grade();

    if (!(grade + 1 < GetScript()->GetDisjointMachineMaxGrade()))
    {
        return 0x13;
    }
    int expertLevel = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    if (grade + 1 >= expertLevel)
    {
        return 0x13;
    }

    // Etc 脚本 [expertjob level limit] 表：key=grade+2 → 所需角色等级。
    // b28d-solo 修法（P48/P96/P270）：ORIG 0x085d386c keyByte 为 unsigned char
    // 且 find 直传其地址（无 cast 临时）；0x085d386f/0x085d388f 两次独立
    // GetEtcScript()（禁缓存 map 引用局部）；迭代器为非 const（ORIG 仅发射
    // _Rb_tree_iterator 实例，const_iterator 会多转换构造）。
    unsigned char keyByte = (unsigned char)(grade + 2);
    std::map<unsigned char, short>::iterator it =
        GetEtcScript()->m_levelMap.find(keyByte);
    // b28d-solo（P222 正向包裹）：ORIG 把 it==end 的 0x13 错误块放在 0xe 块
    // **之后**（0x085d38ee vs 0x085d38e4）；实测 else-if 链劣化（identical
    // 20→19），正向包裹为最优形（残差仅块序，见函数头判词）。
    // [2026-09-10 复测] 显式 `else { return 0x13; }` 形实测无效（仍 202/201，
    // 4.4 把 else 出口排到函数尾 landing pad 之后）⇒ 块序属 N93附/N121 族。
    if (it != GetEtcScript()->m_levelMap.end())
    {
        // ORIG 0x085d38c9 先取 it->second（movzwl+movswl→%ebx）再调
        // get_charac_level（→%eax）、`cmp %eax,%ebx; setg` ⇒ 比较方向写
        // `it->second > level`（等价改写，先算 RHS）。
        if (it->second > user->get_charac_level())
        {
            return 0xe;
        }

        ++grade;   // 升级
        int upgradeCost = GetScript()->GetUpgradeCost((unsigned)grade);
        // ORIG 0x085d3913/0x085d3919 两条**独立**比较（js <0 / jne !=0 共落
        // 0x137 错误块）⇒ 拆两条独立守卫（P133①）；money 检查独立成句。
        if (upgradeCost < 0 || upgradeCost == 0)
        {
            return 0x16;
        }
        if (user->getCurCharacMoney() < upgradeCost)
        {
            return 0x16;
        }

        user->getCurCharacInvenW()->use_money(upgradeCost,
                                              (eMoneySubReason)0x20, true);
        GetSelfExpand(user)->set_disjoint_machine_grade(grade);

        // 新等级满值耐久若存在则一并重置。
        stEnduranceRepairCost* full =
            GetScript()->_GetEnduranceRepairCost((unsigned)(grade + 1));
        if (full != 0)
        {
            GetSelfExpand(user)->set_disjoint_machine_endurance(full->m_field4);
        }

        PacketGuard packet;
        packet.put_header(1, 0xf4);
        packet.put_byte(1);
        packet.put_int(user->getCurCharacMoney());
        // b28d-solo 真语义修正：ORIG 0x085d3a2e `mov -0x18(%ebp),%eax;
        // add $0x1,%eax` ⇒ put_int(grade + 1)（原实现少 +1）。
        packet.put_int(grade + 1);
        packet.put_int(GetSelfExpand(user)->get_disjoint_machine_endurance());
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    return 0x13;
