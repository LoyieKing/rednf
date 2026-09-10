bool CUser::CalLevelUpItemState(char& state, int& needExp, int fromLevel, int toLevel)
{
    // [P214①] ORIG 0x08689d8b/0x08689d9b 两条 jcc（jl/jg）**同落** +0x30 的
    // `mov $0x0,%eax`，+0x29 `mov $0x1,%eax` 汇聚后 +0x35 `test %al,%al; je <函数尾0>`
    // ⇒ 源形是正极性 `if (a >= from && a <= to) { body } return false;`（负极性 `||`
    // 早退产 4.4 `jl <body>` + 独立 `mov $0; jmp` 近处块，见改前 diff）。
    if (get_charac_level() >= fromLevel && get_charac_level() <= toLevel)
    {
        state = 1;
        // [P0/P52] ORIG 0x08689dd1 `mov %eax,%esi`（get_level_up_exp 结果跨后续 3 次 call
        // 保活）/ 0x08689df1 `mov %eax,%ebx`（get_level_exp 结果）—— 两值**无栈槽往返**
        // ⇒ 原码无 luExp/lvTop 具名局部，是内联实参表达式（旧具名版生 -0x10/-0xc 双槽 +
        // push 数少 2）。求值序照 ORIG：先 get_level_up_exp，后 get_level_exp。
        needExp = get_level_up_exp(get_charac_level())
                + (G_CDataManager()->get_level_exp(get_charac_level())
                   - get_charac_exp());
        // ORIG：wrong/success 两分支各自独立重调 getCurCharacNo/get_acc_id/NumberToString
        // 并各自构造 cMyTrace（访问器结果禁缓存，战术 P183/P48）。
        if (needExp < 0)
        {
            // [CUserOnlyOrig 2026-09-09 P139] ORIG 用局部静态符号
            // _ZZN5CUser19CalLevelUpItemStateERcRiiiE19__PRETTY_FUNCTION__（nm 实证 0x08cf2620）
            // ⇒ 必须传 __PRETTY_FUNCTION__，手写同内容字面量只生匿名 .rodata。
            cMyTrace trace((char*)__PRETTY_FUNCTION__, 0xa0d7, 0);
            trace("Level_up_item:wrong value(%d) m_id(%s),charno(%u)", needExp,
                  NumberToString(get_acc_id(), 0), getCurCharacNo());
            return false;
        }
        cMyTrace trace((char*)__PRETTY_FUNCTION__, 0xa0da, 0);
        trace("Level_up_item:success value(%d) m_id(%s),charno(%u)", needExp,
              NumberToString(get_acc_id(), 0), getCurCharacNo());
        return true;
    }
    return false;
