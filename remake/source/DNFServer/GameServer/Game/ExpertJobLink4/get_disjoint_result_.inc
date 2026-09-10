int CDisjointer::get_disjoint_result_(CItem* item, Inven_Item* invenItem,
                                      int level, DisJointResult& result,
                                      bool selfService, CUser* user)
{
    int itemIndex  = item->get_index();     // local_54（ORIG 计算后未再使用）
    int sellPrice  = item->GetSellPrice();  // local_50（×1.1 后作基价）
    int rarity     = item->get_rarity();    // local_4c
    int grade      = item->get_grade();     // local_48
    (void)itemIndex;

    bool unidentified = false;
    // P198①/P194：ORIG 0x85d2395 `movb $0,-0x41` 预清 + 条件 mov$1/mov$0 物化。
    unidentified =
        (invenItem != 0 && !invenItem->m_amp.isIdentified());

    // map<DisjointMachineGrade,stDisjointResult> 键：(a=rarity, b=level,
    // c=unidentified) → {m_grade=level, m_field1=rarity, m_field2=unidentified}。
    // ORIG find/end 各独立 `call GetScript`（无 script 具名槽）。
    std::map<DisjointMachineGrade, stDisjointResult>::iterator it =
        GetScript()->m_disjointResultMap.find(
            DisjointMachineGrade((unsigned char)rarity, (unsigned char)level,
                                 unidentified));
    if (it == GetScript()->m_disjointResultMap.end())
        return unidentified ? 0xd3 : 0x11;

    stDisjointResult& res = it->second;

    sellPrice = round_nearest(1.1f * sellPrice);          // ROUND(_DAT_08cc080c × price)
    // [b28d-solo-ExpertJobLink4 P252]：ORIG `mov 0x4(%eax),%eax; mov %eax,-0x3c`
    // 具名 float 局部（后续 `fmuls -0x3c` 槽往返），非直读对象字段。
    float field4 = res.m_field4;
    result.m_f[5] = res.m_field0;                         // +0x14 cube 索引

    int money = getItemSellPrice(sellPrice, 0, 0, item->IsRandomOption());
    money = (money * 1000) / GetScript()->m_baseConst;    // +0x1d4 [base const]
    result.m_f[0] = round_nearest(money * field4 / 1000.0f);   // _DAT_08cc0810
    if (result.m_f[0] < 1)
        result.m_f[0] = 1;
    result.m_f[6] = 0;                                    // +0x18 附加物 idx
    result.m_f[1] = 0;                                    // +0x04 附加物数量

    // 大奖基础率：res+0x20 ×100（导入侧第 6 列，当前导入 TU 未持久化——见
    // STExpertJobScript.h 注释）。
    int bigwinRate = res.m_field20 * 100;

    if (selfService)
    {
        unsigned char key = (unsigned char)rarity + 1;
        unsigned int rate = 0;
        std::map<unsigned char, unsigned char>::iterator etcIt =
            GetEtcScript()->m_disjointerBigWinRate.find(key);
        if (etcIt != GetEtcScript()->m_disjointerBigWinRate.end())
            rate = etcIt->second;
        bigwinRate += (int)(rate * 100);

        // ORIG 无 active 具名槽（条件直测 `mov $0,%eax; test %al,%al; je`）。
        if (user != 0 && user->get_state() >= 3 && user->getCurCharacR() != 0)
        {
            const CInventory* inv = user->getCurCharacInvenR();
            int jobLevel =
                user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
            float bonus = 100.0f * inv->GetExpertJobSelfDisjointBigWinRate(
                                     (ENUM_EXPERT_JOB_TYPE)3, jobLevel);
            bigwinRate = round_nearest(bigwinRate + bonus);
        }
    }

    if (get_rand_int(10000) < bigwinRate)
    {
        stItemSelectRate select;
        select.m_itemIndex = -1;
        get_additional_item(res.m_bigwinItems, get_rand_int(10000), select, grade);
        if (select.m_itemIndex != -1)
        {
            result.m_f[6] = select.m_itemIndex;
            result.m_f[1] = round_nearest((float)grade / select.m_float8);
            if (result.m_f[1] < 1)
                result.m_f[1] = 1;
        }
    }
    else
    {
        stItemSelectRate select;
        select.m_itemIndex = -1;
        get_additional_item(res.m_additionalItems, get_rand_int(10000), select,
                            grade);
        if (select.m_itemIndex != -1)
        {
            result.m_f[6] = select.m_itemIndex;
            result.m_f[1] = round_nearest((float)grade / select.m_float8);
            if (result.m_f[1] < 1)
                result.m_f[1] = 1;
        }
    }

    if (selfService)
    {
        // script+0x30 [disjoint self service]：{概率, 奖励A, 奖励B}
        // （stSelfDisjoinMaterialInfo m_field0/m_field4/m_field8）。
        int* svc = &GetScript()->m_selfDisjoin.m_field0;
        // [C07 · Main 范式②(F2) 落地] ORIG 0x85d2f? 段 `mov -0x1c(%ebp),%eax;
        //   mov (%eax),%eax; mov %eax,-0x18(%ebp); cmpl $0x1,-0x4c(%ebp); jle;
        //   call get_rand_int(100); cmp -0x18(%ebp),%eax; jge`（具名 int 局部 +
        //   内存直比）。内联 `svc[0] > get_rand_int(100)` 使 4.4 把 svc[0] 升格
        //   %ebx 跨 call 并折成 register-cmp（多 1 条 mov 且判据寄存器化）。
        int selfRate = svc[0];
        if (rarity >= 2 && selfRate > get_rand_int(100))
        {
            result.m_f[2] = svc[2];   // +0x08
            result.m_f[7] = svc[1];   // +0x1c
        }

        if (user != 0 && user->get_state() >= 3 && user->getCurCharacR() != 0)
        {
            const CInventory* inv = user->getCurCharacInvenR();
            int jobLevel =
                user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
            // [C07 · P252 试形已回退] 加具名 float 中间量（`float rawRate = f(...);
            //   float variation = rawRate / 100.0f;`）实测 gap 14→12 但 ndiff
            //   106→344（槽整体平移 4、difflib 对齐坍缩）⇒ 净负，恢复直写形。
            float variation =
                inv->GetExpertJobSelfDisjointResultVariation(
                    (ENUM_EXPERT_JOB_TYPE)3, jobLevel) / 100.0f;   // _DAT_08cc0814

            // [C07 · P214③] ORIG 在循环前发 5 条 `movl $0x0,-0xa8..-0x98(%ebp)`
            //   （连续寻址）⇒ 原码是**显式零初始化数组**（我方裸 `int tmp[5];` 无
            //   清零，缺 5 条）。
            int tmp[5] = { 0, 0, 0, 0, 0 };
            for (int i = 0; i <= 4; ++i)
            {
                tmp[i] = round_nearest(result.m_f[i] * variation);
                result.m_f[i] += tmp[i];
            }
        }
    }

    return 0;
