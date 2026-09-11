// ===========================================================================
// CWorldDrop::generateRandomItem — ORIG T 0x085383b8
// b21 同族：+0x34/+0x35 改成员直读（m_field34/m_field35，语义同 u8_at）。
// ===========================================================================
// [b28d-solo-WongWorkDrop_A] verdict=DIFF(N121)：p167 CLEAR；残余仅 4.4
// epilogue-threading 尾岛 2×`jmp+nop`（ORIG 无岛），break/goto 尾标签/return
// 三形实测不消 ⇒ N121 封顶。已修：ctx 具名指针局部（-0x20）、prod/100 去负数
// 特判、pick 复用 roll 槽、push 内联、int i + size() 有符号比较、slot 预清 0。
// [round4 L3CUser 复核 2026-09-10] ae_insn 残差仅 2 处：①`movl $&_ZL14gUnicodeBuffer
//   +0xe174,0x8(%esp)` 我方为 .o 未解析重定位（AE/N132 等价）；②尾 2×`jmp+nop`
//   为我方多出（ORIG 无岛）⇒ N121 维持，指令数 123/127 与判词一致。
// [A28 round5 复核 2026-09-10] 重跑 clean 口径：orig=123 / ours=125，残差仍仅尾部
//   2×`jmp`（ORIG `jne <T>; leave; ret` 直落 ⇒ 无岛）⇒ N121（4.4 epilogue-threading
//   尾岛不可消）维持封顶；p167 调用点 CLEAR。
// [A28 round5 · C05③ P223 实测 2026-09-10 19:42] 按 Main 转发的「单尾标签串消早返
//   中继岛」范式，把三处早返改为 `goto end;` + 尾 `end:;`（跨 goto 的 slot/threshold/acc
//   一律声明与赋值分两句）：**实测无效**，orig=123/ours=125 与残差 2×`jmp` 逐字不变
//   （CWorldDrop 的岛来自 4.4 自身 epilogue-threading，非早返平铺产物）⇒ 已回退为
//   `return;` 形，N121 维持。C05①② 对本 TU 其余件亦无命中（残差无 `cmp mem` 类、
//   无「失败出口落函数尾」布局差）。
// [M1S5 2026-09-11 **推翻 N121**] 钥匙 =「**空 then + else 包块**」：把两处早返
//   `if (cond) return;` 改写为 `if (cond) { } else { ...余下全部... }`，函数收敛为
//   单出口 ⇒ our 127→123=ORIG，尾岛 2×`jmp+nop` 全消，尾部与 ORIG
//   0x0853854c..0x0853855b `setg/test/jne; leave; ret` 逐条同形；本件 DIFF→AE
//   （TU ae 87→88、合计 138→139、diff 2→1）。中间读数：只包第一处时 127→125
//   （每处早返各贡献 1 个 `jmp+nop` 岛）；continue/while/break 三形 0 变化
//   （125/38 逐字不变）⇒ 岛源是 4.4 对早返出口的 epilogue-threading，包裹后消失。
void CWorldDrop::generateRandomItem(stGenerateRefData_t const& ref,
                                    stGenerateResult_t& result)
{
    // [b28d-solo-WongWorkDrop_A] ORIG 形：具名指针局部 ctx = &ref+0x34
    //（-0x20 槽），ctx[0]/ctx[1] 多次直读；threshold 直接 prod/100（无负数
    // 特判分支，ORIG 无 js）；pick 复用 roll 槽；push 展开为内联体。
    // ctx 基址取具名字段 m_field34（§4.3/P82；+0x35 = m_field35 字节视图）。
    StRefCtxView const* ctx =
        reinterpret_cast<StRefCtxView const*>(&ref.m_field34);
    int roll = m_random.generateNumber(0, 100000);     // CGenerateRandomNumber(this+8)

    if (ctx->m_level > 199)                            // ORIG movzbl cmp $0xc7 ja
    {
    }
    else
    {
        WorldDropInfo* slot = 0;                       // ORIG 预清 movl $0,-0x18
        if (ctx->m_type == 1)                          // warArea 分支
            slot = &m_warAreaDrop[ctx->m_level];
        else
            slot = &m_worldDrop[ctx->m_level];
        // threshold = first × dropRate / 100（ORIG imul 0x51eb851f 魔数除法）
        int threshold = (slot->m_first * ref.m_dropRate) / 100;
        if (roll > threshold)                          // jg 退出
        {
        }
        else
        {
            roll = m_random.generateNumber(1, slot->m_total);  // 复用 roll 槽
            int acc = 0;                               // local_14
            for (int i = 0; static_cast<int>(slot->m_chunks.size()) > i; ++i)
            {
                acc += slot->m_chunks[i].m_rate;       // 元素+0x04 权重
                if (roll < acc)
                {
                    Inven_Item inv;                    // 元素+0x00 idx
                    inv.m_addInfo = slot->m_chunks[i].m_itemIdx;
                    G_CDataManager()->m_itemList->create_item(
                        static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
                    result.m_dropItems.push_back(inv);
                    return;
                }
            }
        }
    }
}