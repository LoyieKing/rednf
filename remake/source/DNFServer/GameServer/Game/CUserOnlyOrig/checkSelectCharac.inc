// ---------------------------------------------------------------------------
// 44) CUser::checkSelectCharac(int slot)
//     报告：docs/class_func_reports/CUser/checkSelectCharac.md。ORIG 0x0864aaf6。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0864aaf6：p167=M0/E0:，gap=-7；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864aaf6：修法（本轮）——trace 字符串改
// __PRETTY_FUNCTION__ 直压立即数（P118/P139，消静态槽装载）；lvl 改表达式直用 + 嵌套 if
// （P200/P225）复现 `movzwl 0x27(%eax),%eax; movswl %ax,%ebx` + 两次完整下标链 operator[]。
// 残余：首守卫 `jle` vs 4.4 `jg`（P258 德摩根 or 形）+ setcc 物化 + 帧差
// （ORIG push ebx/sub $0x24 vs 4.4 sub $0x38，N115 outgoing 预留）⇒ N82/N115 封顶。
bool CUser::checkSelectCharac(int slot)   // [b28d-Main·P292] int→bool
{
    if (getCurCharacR() != 0)
    {
        // [b28d-solo-CUserOnlyOrig P118/P139] ORIG `movl $<A>,0x4(%esp)` 直压
        // __PRETTY_FUNCTION__ 局部静态符号；旧 static const char* PRETTY 多一次
        // 静态槽装载（mov <A>,%eax; mov %eax,0x4(%esp)）。
        cMyTrace trace(__PRETTY_FUNCTION__, 0xea4, 5);
        trace("CUser::SelectCharac m_selected is not null");
        return false;
    }
    // [b28d-solo-CUserOnlyOrig P133/P214① 修法] ORIG 0x0864ab51 段
    // `js L0; …call size; cmp; jle L0; mov$1; jmp; L0: mov$0; test; je L_false`
    // = 正向 `slot>=0 && size>slot` 条件物化岛；0x0864abbd 段
    // `cmp min; jl A(mov$1); cmp max; jle B(mov$0); A/B; test; je TRUE` =
    // `return (lvl>=min) && (lvl<=max);`（&& 短路物化，min 先调）。旧
    // `slot<0||size<=slot` 早返 + 嵌套 if 产 jg 直跳 + setge/setg 物化（50 条残差）。
    if (slot >= 0 && (int)m_characList.size() > slot)
    {
        // [b28d-solo-CUserOnlyOrig P59/P133 修法] ORIG 0x0864ab8d `call IsPVPChannel;
        // test %al,%al; je 864ac08(mov$1)`（正向 if/else，无 xor）；`==0` 早返形在 4.4
        // 恒多一条 xor（bool 取反物化）⇒ 改正向 if/else。
        // [b28d-solo-CUserOnlyOrig P214②] ORIG 三个 return true 共享同一 `mov $1` 块
        // （864ac08 同时是 IsPVPChannel 失败跳点与函数尾）⇒ 内层不写 return true，
        // fall-through 到函数尾 return true（早返形 `==0` 产 xor、if/else 形不合并）。
        if (G_GameWorld()->IsPVPChannel())
        {
            if (!((int)(short)m_characList[slot].m_level
                    >= G_CDataManager()->GetMinLevelPvPChannel()
                  && (int)(short)m_characList[slot].m_level
                    <= G_CDataManager()->GetMaxLevelPvPChannel()))
                return false;
        }
        return true;
    }
    return false;
}