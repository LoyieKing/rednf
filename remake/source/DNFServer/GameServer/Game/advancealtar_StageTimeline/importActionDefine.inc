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
        //   esi/ebx 寄存器承载成立，但本波 P231 纯 register（非 asm 绑定，P0 合规）
        //   实测 286→288 insns（反劣）⇒ 回滚，判 N（err/state 槽形为 4.1/4.4 分配差）。
        int err = 0;                                // esi：state==0 时的返回码
        int state;                                  // ebx：0=返 err / 1=返 0 / 2=续

        if (!ScanType(tok, true))                   // ORIG 0889a5a1
        {
            err = 0x253;
            state = 0;
        }
        else if (tok == "[/action define]")         // ORIG 0889a5bc
        {
            state = 1;
        }
        else if (tok == "[normal]" || tok == "[named]" ||
                 tok == "[boss]")                   // ORIG 0889a5dd..64c
        {
            ad.m_id = ScanInt(&ok);                 // ORIG 0889a652
            if (!ok)
            {
                err = 0x26b;
                state = 0;
            }
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
                        err = 0x27a;
                        state = 0;
                        break;
                    }
                    act.m_pSummon = &it->second;    // ORIG 0889a70b：iter 值 +0x04

                    act.m_firstDelayMs = ScanInt(&ok);  // ORIG 0889a71c
                    if (!ok)
                    {
                        err = 0x27f;
                        state = 0;
                        break;
                    }
                    if (act.m_firstDelayMs < 0)
                    {
                        printf("advancealtar :  action.firstDelayMs %d < 0\n",
                               act.m_firstDelayMs);
                        err = 0x285;
                        state = 0;
                        break;
                    }

                    act.m_nextDelayMs = ScanInt(&ok);   // ORIG 0889a76d
                    if (!ok)
                    {
                        err = 0x289;
                        state = 0;
                        break;
                    }
                    if (act.m_nextDelayMs < 500)
                    {
                        printf("advancealtar :  action.nextDelayMs %d < 500\n",
                               act.m_nextDelayMs);
                        err = 0x290;
                        state = 0;
                        break;
                    }

                    int flagVal = ScanInt(&ok);     // ORIG 0889a7c1（-0xc）
                    if (!ok)
                    {
                        err = 0x294;
                        state = 0;
                        break;
                    }
                    if (flagVal != 0)               // ORIG 0889a7e3..7f6
                    {
                        act.m_flag = 1;
                        act.m_flagValue =
                            static_cast<unsigned short>(flagVal);   // mov %ax
                    }
                    else
                    {
                        act.m_flag = 0;             // ORIG 0889a7e9 显式写 0
                    }
                    ad.m_actions.push_back(act);    // ORIG 0889a80a
                }

                if (state != 0)
                {
                    // ORIG 0889a815..85f：make_pair(ad.m_id, ad) → map insert。
                    m_actionMap.insert(std::make_pair(ad.m_id, ad));
                    state = 2;                      // 回外层继续读段
                }
            }
        }
        else
        {
            printf("advancealtar : ActionDefine.Type undefined='%s'\n",
                   tok.c_str());
            err = 0x267;
            state = 0;
        }

        // 公共尾部（ORIG LAB_0889a8c0）：tok 析构后按 state 分派。
        if (state == 1)
            return 0;                               // 正常收段
        if (state == 0)
            return err;
        // state == 2 → 继续外层循环。
    }
