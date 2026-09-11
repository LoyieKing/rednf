// ---- OnUpgradeDisjointMachine（报告，ORIG T 0x085d37e8 sz 0x2e2）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（块序 N93附/N121 类）+ 真语义修复。
// 修复：①P49 宏化 GetSelfExpand（原 static inline 发射发明符号
//   _ZN10expert_job12_GLOBAL__N_113GetSelfExpandEP5CUser，ORIG 无）⇒ 每使用点
//   独立重发 GetCharacExpandData 调用（P48）；②**真 bug** put_int(grade + 1)
//   （ORIG 0x085d3a2e `mov -0x18(%ebp),%eax; add $0x1,%eax`，原实现漏 +1）；
//   ③keyByte unsigned char + find 直传其地址（无 cast 临时）+ 两次独立
//   GetEtcScript() + 非 const iterator（P48/P96/P270）；④upgradeCost<0||==0
//   双独立守卫（P133①）；⑤比较方向 `it->second > level`（ORIG 先算 RHS）。
// [W13HExpertJob 2026-09-11 收口·DIFF→AE] ⑥**块序根因消除**：把正常流程从
//   `it != end` 的 then 块**移出**到 if/else 之后，it==end 改走显式 else 早退
//   （`else { return 0x13; }`）。ORIG 形状 = 0xe 块与 0x13 块**相邻**且各带
//   `jmp 尾`（0x085d38e4 `mov $0xe,%ebx; jmp 0x085d3abf`；0x085d38ee
//   `mov $0x13,%ebx; jmp 0x085d3abf`），正常流程自 0x085d38f8 起。4.4 在此形
//   下把内层 `je` 经 jump-threading 直指正常流程入口 → 两侧 clean 后 202/202
//   逐条全等（verdict=AE，raw 多 1 条为函数尾对齐 nop）。同前 ①–⑤ 全保留。
//   ⇒ 旧「N93附/N121 块序封顶」判词**作废**（原判源于内层 else 包裹形）。
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
    // 块序（W13HExpertJob 2026-09-11 收口）：正常流程必须在 if/else **之外**，
    // 才能让 4.4 把 it==end 的 0x13 块排在 0xe 块之后（ORIG 0x085d38ee）并把
    // 内层 `je` 直指正常流程入口（0x085d38f8）。语义与「then 内续写 + 尾
    // return 0x13」等价（else 分支提前 return，正常流程仅 it!=end 可达）。
    if (it != GetEtcScript()->m_levelMap.end())
    {
        // ORIG 0x085d38c9 先取 it->second（movzwl+movswl→%ebx）再调
        // get_charac_level（→%eax）、`cmp %eax,%ebx; setg` ⇒ 比较方向写
        // `it->second > level`（等价改写，先算 RHS）。
        if (it->second > user->get_charac_level())
        {
            return 0xe;
        }
    }
    else
    {
        return 0x13;
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