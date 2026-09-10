void CUser::set_ingame_event_history(SIG_INGAME_EVENT_HISTORY* sig)
{
    // set_ingame_event_history(SIG*).md @0x868d664：条目 stride 0xc ×30 循环；
    // key==0 跳过；get_event_script_mng()->is_current_ingame_event_record_type(key)
    // 通过才逐条转发 set_ingame_event_history(key,+4,+8)。
    if (sig == 0)
        return;
    // [b28d-solo-CUserOnlyOrig P167/P95 真形] ORIG 0x0868d664 无任何 memcpy
    // （旧写法 EXTRA memcpy ×3、OUR calls 7 vs ORIG 4）：每条目三次完整
    // `idx*3; shl 2; add sig` 链直读 int 字段 ⇒ 局部具名视图 + 逐字段表达式
    // （P0 无 view 指针槽；ORIG 直接 `add 0xc(%ebp),%eax`）。
    struct IngameEventRecord { int m_key; int m_v1; int m_v2; };
    // [A06 2026-09-10 P319 第二形 试行记录·已回退] ORIG 每处寻址尾为 `add 0xc(%ebp),%eax`
    // （整数加法、形参作内存操作数，O13/O22/O37/O44/O51）。实测 `(unsigned int)sig + i*0xc`
    // 回 cast 形在 4.4 产 **3 条**（`mov %eax,%edx; mov 0xc(%ebp),%eax; lea (%edx,%eax,1),%eax`）
    // 反劣于原下标形的 2 条（`mov 0xc(%ebp),%ecx; lea (%ecx,%eax,1)`）⇒ 回退，记 N18a 族
    // （4.1 `add mem,%eax` 单条不可造）。
    // 视图元素仍具名（§4.3 合规）。
    // ORIG 0x0868d714 尾测 `cmpl $0x1d,-0xc; setle %al; test %al,%al; jne`
    // = 有符号 `i <= 0x1d`（P2/P51）；`i < 0x1e` 在 4.4 生 jcc 直跳无 setle。
    for (int i = 0; i <= 0x1d; ++i)
    {
        // [G6Sub3 2026-09-10 P296 新旋钮·待父泳道测] ORIG 0x0868d681/868d694/868d6c2 三处失败
        // 出口**同落** 868d710(=i++)、体尾**无**中继岛；旧 `continue` 形令 4.4 在体尾多发
        // `jmp +2; nop` 岛（OUR 79 vs ORIG 70 中的 2 条）。按 P296 改嵌套正向 if 包裹 ⇒ 期望
        // 消 jmp+nop；若四数不改善即回退。
        // ORIG 0x0868d690 `test %eax,%eax; je 868d710`（key==0 → i++）= 正极性 `!= 0` 守卫。
        if (reinterpret_cast<const IngameEventRecord*>(sig)[i].m_key != 0)
        {
            // ORIG 0x0868d6a5 `mov (%eax),%ebx` 跨 G_CDataManager/get_event_script_mng
            // 保活 ⇒ 链式内联实参（P52），勿用具名 scriptMng 槽。
            // [L6 判据①/P222 极性重核] ORIG 0x0868d6c0 `call is_current_ingame_event_record_type;
            // test %al,%al; je 868d710` = **正极性**守卫（体在 je 之后直落）；旧 `if (!…) continue;`
            // 使 4.4 反产 `xor $1,%eax; test %al,%al; jne`。
            if (static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())->
                    is_current_ingame_event_record_type(
                        reinterpret_cast<const IngameEventRecord*>(sig)[i].m_key))
            {
                set_ingame_event_history(
                    reinterpret_cast<const IngameEventRecord*>(sig)[i].m_key,
                    reinterpret_cast<const IngameEventRecord*>(sig)[i].m_v1,
                    reinterpret_cast<const IngameEventRecord*>(sig)[i].m_v2);
            }
        }
    }
