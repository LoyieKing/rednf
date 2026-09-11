// ============================================================================
// importActionDefine @ 0x889a56e：
//   外层循环每轮重建 ActionDefine；ScanType(token,true)：
//     - 失败（段内 EOF）→ 错误 0x253；
//     - "[/action define]" → 返 0（正常收段，调用方按 0 继续）；
//     - "[normal]"/"[named]"/"[boss]" → ad.id=ScanInt(ok)（失败 0x26b），随后
//       内层循环逐行 _Action：summonId=ScanInt(ok)（失败=行耗尽→插入并续外层），
//       find(summonId) 未命中 → printf + 0x27a；m_pSummon=&命中值；
//       firstDelayMs<0 → 0x285；nextDelayMs<500 → 0x290；
//       第 4 列 !=0 → flag=1 且 flagValue=该列低 16 位。push_back 后循环。
//       插入 make_pair(ad.id, ad) 后外层继续。
//     - 其它 token → printf "Type undefined" + 0x267。
// ============================================================================
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=新口径 n1_v 275/274 gap=+1 blocks=8；`||` 链 bool 物化（我方 `mov $0x1,%eax; jmp; mov $0x0,%eax; test %al,%al` 岛）vs ORIG 三独立 cmp + 块序；P231 register×2（286→288）、S3Stage 去 err/state 具名（287 反劣 1）均回滚 ⇒ 维持（ORIG esi/ebx 为多 return 点返回临时，非源码变量）。
int StageTimeLineParameter::importActionDefine()
{
    // b28d-35: DIFF→N82（帧/槽 + 表遍历形状）。ORIG @0x0889a56e。
    bool ok = false;                                // ORIG -0x41，入口置 0
    for (;;)                                        // ORIG 0889a57d
    {
        ActionDefine ad;                            // ctor：id=0 + vector 构造
        std::string tok;
        // [G5 2026-09-10 widthscan 线索·实测反劣已回滚] `clr_l orig=0 our=10`
        //   （我方 9×`movl $0x0,-0x10` = state、1×`-0x14` = err）；ORIG 注释的
        //   esi/ebx 寄存器承载成立，本波 P231 纯 register（非 asm 绑定，P0 合规）
        //   实测 286→288 insns（反劣）⇒ 回滚，判 N（err/state 槽形为 4.1/4.4 分配差）。
        // [S3Stage 2026-09-11 重攻] 实测 register×2（去 err 初值）= 287 反劣 1 条
        //   （多 push %edi、帧 0x80→0x9c），已回滚。⇒ ORIG 的 esi/ebx 非源码变量，
        //   而是 **GCC 为多 return 点生成的返回临时 + 状态标记**（0x889a8d6/dd/e4 的
        //   `mov $N,%ebx` 常数重物化只在「临时」上出现；具名变量必落槽）。
        //   本轮改法 = 直接 `return <码>;`（去掉 err/state 具名变量），让 4.4 自产该结构。
        if (!ScanType(tok, true))                   // ORIG 0889a5a1
            return 0x253;                           // ORIG 0889a5ad：esi=0x253/ebx=0
        else if (tok == "[/action define]")         // ORIG 0889a5bc
            return 0;                               // ORIG 0889a5d3：ebx=1
        else if (tok == "[normal]" || tok == "[named]" ||
                 tok == "[boss]")                   // ORIG 0889a5dd..64c
        {
            ad.m_id = ScanInt(&ok);                 // ORIG 0889a652
            if (!ok)
                return 0x26b;                       // ORIG 0889a665
            else
            {
                for (;;)                            // ORIG 0889a674（动作行循环）
                {
                    _Action act;                    // ctor memset 0x14

                    act.m_summonId = ScanInt(&ok);  // ORIG 0889a685
                    if (!ok)
                        break;                      // 行耗尽 → 插入本 ActionDefine

                    std::map<int, _SummonObject>::iterator it =
                        m_summonMap.find(act.m_summonId);   // ORIG 0889a6b0
                    if (it == m_summonMap.end())
                    {
                        printf("advancealtar : not found SummonObject Id = %d\n",
                               act.m_summonId);
                        return 0x27a;               // ORIG 0889a6f6
                    }
                    act.m_pSummon = &it->second;    // ORIG 0889a70b：iter 值 +0x04

                    act.m_firstDelayMs = ScanInt(&ok);  // ORIG 0889a71c
                    if (!ok)
                        return 0x27f;               // ORIG 0889a72f
                    if (act.m_firstDelayMs < 0)
                    {
                        printf("advancealtar :  action.firstDelayMs %d < 0\n",
                               act.m_firstDelayMs);
                        return 0x285;               // ORIG 0889a758
                    }

                    act.m_nextDelayMs = ScanInt(&ok);   // ORIG 0889a76d
                    if (!ok)
                        return 0x289;               // ORIG 0889a780
                    if (act.m_nextDelayMs < 500)
                    {
                        printf("advancealtar :  action.nextDelayMs %d < 500\n",
                               act.m_nextDelayMs);
                        return 0x290;               // ORIG 0889a7ac
                    }

                    int flagVal = ScanInt(&ok);     // ORIG 0889a7c1（-0xc）
                    if (!ok)
                        return 0x294;               // ORIG 0889a7d4
                    // [S3Stage 2026-09-11] ORIG 0889a7e7 `jne 0889a7ef` ⇒ 假路径
                    //   （flag=0）在**前**、真路径在后 = 源码条件为 `== 0`（否定式），
                    //   我方旧形 `!= 0` 产 then-first 多 1 条 jmp。
                    if (flagVal == 0)
                    {
                        act.m_flag = 0;             // ORIG 0889a7e9 显式写 0
                    }
                    else
                    {
                        act.m_flag = 1;
                        act.m_flagValue =
                            static_cast<unsigned short>(flagVal);   // mov %ax
                    }
                    ad.m_actions.push_back(act);    // ORIG 0889a80a
                }

                // ORIG 0889a815..85f：make_pair(ad.m_id, ad) → map insert。
                // （行耗尽/插入成功都落到这里；ORIG 的 `state = 2` 由 4.4 自产临时承载）
                m_actionMap.insert(std::make_pair(ad.m_id, ad));
            }
        }
        else
        {
            printf("advancealtar : ActionDefine.Type undefined='%s'\n",
                   tok.c_str());
            return 0x267;                           // ORIG 0889a63d
        }
        // 成功插入 → 继续外层循环（ORIG LAB_0889a8c0 之后的 continue 路径）。
    }
}