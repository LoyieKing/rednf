// ---- CDisjointer::get_disjoint_result_（报告 get_disjoint_result_.md，
//      ORIG T 0x085d234a sz 0x6cc）。 ----
// [L2Free4to15.S3 复核] p167 CLEAR（仅 C1/C2 别名）。
// [R28a18 · 2026-09-13 · §34.2.3 精度语义轴 · 已修 ④] ORIG 4 处
//   `fstps X; flds X` 具名 float 局部舍入往返（0x85d264c / 0x85d2791 /
//   0x85d2878 / 0x85d2fa0 段）vs 我方 0 处 ⇒ 中间精度（x87 80 位 vs 落盘 float）
//   不同、后续 fistpl 结果可差 1（分叉实证见 /tmp/verify357/R28a18/probe_fp2.c：
//   PATH2 im=14 grade=644 ORIG=4600/NEW=4599、PATH4 ia=1178 ib=2822 mult=2300
//   ORIG=91/NEW=92）。修法 = 具名 float 局部 + 复合赋值（**同一**局部两次写）：
//   `float bonus = f(...); bonus *= 100.0f;` /
//   `float ratio = (float)grade / select.m_float8;`（两分支各一）/
//   `float variation = f(...); variation /= 100.0f;`。
//   新 .o 4 处往返逐条同形（槽 -0x2c/-0x28/-0x24/-0x10，各为单槽两次写）；
//   TU 四数零劣化（identical 96 / ae 189 / near 1 / diff 4 / missing 33 逐值不变）。
//   残余 = add-vs-disp 组（N107/P30）+ 帧 0xd4→0xe4（新增 2 个 ratio 局部，N82 族）。
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
            // [R28a18 · §34.2.3 精度语义轴] ORIG 0x085d264c 段实测
            //   `call; fstps -0x28; flds -0x28; flds 100.0f; fmulp; fstps -0x28`
            //   （**同一**具名 float 局部被写两次）⇒ 源码形 = 分两步赋值。
            //   直写 `100.0f * f(...)` 丢首段 fstps 舍入往返（我方 0 处 vs ORIG 4 处）
            //   ⇒ 中间精度不同（80 位 vs 舍入后 float），fadds+fistpl 结果可差 1。
            float bonus = inv->GetExpertJobSelfDisjointBigWinRate(
                              (ENUM_EXPERT_JOB_TYPE)3, jobLevel);
            bonus *= 100.0f;
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
            // [R28a18 · §34.2.3] ORIG 0x085d2791
            //   `fildl -0x48; flds -0x78; fdivrp; fstps -0x24; flds -0x24; …fistpl`
            //   ⇒ 除法结果先落**具名 float 局部**再取 (int)。
            float ratio = (float)grade / select.m_float8;
            result.m_f[1] = round_nearest(ratio);
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
            // [R28a18 · §34.2.3] 同上报（ORIG 0x085d2878 段，第二次出现）。
            float ratio = (float)grade / select.m_float8;
            result.m_f[1] = round_nearest(ratio);
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
            // [C07 · P252 试形已回退] 旧试形是**两个**局部（`float rawRate = f(...);
            //   float variation = rawRate / 100.0f;`）⇒ 双槽使整体平移 4、净负。
            // [R28a18 · §34.2.3] ORIG 0x085d2fa0 段
            //   `call; fstps -0x10; flds -0x10; flds 100.0f; fdivrp; fstps -0x10`
            //   = **同一**局部两次写 ⇒ 正确形是单局部 + 复合赋值（零额外槽）。
            float variation = inv->GetExpertJobSelfDisjointResultVariation(
                (ENUM_EXPERT_JOB_TYPE)3, jobLevel);
            variation /= 100.0f;                                  // _DAT_08cc0814

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
}