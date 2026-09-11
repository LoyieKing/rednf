// ORIG 0x08195794：SearchEventPeriod 谓词 find_if +0x0c vector；
// 命中后起点 = GM ? m_field8 * (-86400) + 0x5005D250 : 0x5005D250（32 位回绕，
// -0x15180 = -86400 秒/天）；begin <= time < 条目 m_endTime(+0x08) 返回 true。
// 注意：ORIG 此函数不加锁。
// [b28d-28] DIFF | ORIG 0x08195794 | N86/P28：同 isApplied（find_if 槽序 +
// 谓词实参序）+ isGM 分支块序；语义等价。
bool Arad_EventPeriodDataManager::checkCreateTime(
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
    ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time, unsigned int gmId)
{
    SearchEventPeriod pred(eventCode);
    std::vector<SCRIPT::SEventPeriodData>::iterator it =
        std::find_if(m_eventPeriodList.begin(), m_eventPeriodList.end(), pred);
    // P167：ORIG 生成 operator!=（0x08195829 `test %al,%al; je` 直落成功路径，
    //   [L4fEventAvatar 2026-09-11] 无 xor 取反 ⇒ 原码是 if(命中){...} 而非
    //   `if(!(it != end)) return false;`）。
    if (it != m_eventPeriodList.end())
    {
        // [L23-r2 判据⑫ widthscan] `clr_b orig=1 our=0`：ORIG 有 `movb $0x0,-N(%ebp)`
        //   预清（call 前）⇒ P198① 两句式「声明带初值 + 独立赋值」；合写会丢预清行，
        //   且该 bool 局部同时是槽集合差 4 的来源（我方槽整体浅 4）。
        bool isGM = false;
        // ORIG 0x08195845 `test %al,%al; je <跳过>` + 0x08195849 `movb $0x1,-0x11(%ebp)`
        //   ⇒ 条件赋值（if 形），非 `isGM = 调用()`（后者为 `mov %al,-0x11`）。
        if (GlobalData::s_pGMAccounts_->isGM(gmId))
            isGM = true;
        // [L4fEventAvatar 2026-09-11] ORIG 0x0819584d..0x0819586b 是
        //   `cmpb $0,-0x11; je <mov $0x5005d250>` 的双臂选择（非 `+=`）⇒
        //   源码为 if/else 整体赋值；0x0819586e 再把 it->m_endTime 读进具名槽
        //   -0xc（我方原先直读进比较，缺该槽 ⇒ 槽整体浅 4）。
        // ORIG 0x08195864 `jmp 0x0819586b`（真支算完跳汇合）+ 0x08195866
        //   `mov $0x5005d250,%eax` + 0x0819586b 唯一 `mov %eax,-0x10(%ebp)`
        //   ⇒ 三目整体赋值（if/else 会各自落槽，多一对 store）。
        unsigned int beginTime = isGM
            ? ((unsigned int)(m_field8 * -86400) + 0x5005D250u)
            : 0x5005D250u;
        unsigned int endTime = it->m_endTime;
        // [L4fEventAvatar 2026-09-11] 负结论①：内层改「失败早退 + 成功直落」
        //   （`if(!(A&&B)) return false; return true;`）⇒ 4.4 生 `jb <早退>; mov $0;
        //   jmp`，比 ORIG 的 `ja/jae → 尾 mov $0` 更差（实测 82 vs 80），已回滚。
        // 现形：ORIG 0x08195885/0x0819588d 两条跳转与 0x0819582b（外层未命中）
        //   共享尾部唯一 `mov $0`（0x08195896），4.4 对 `return (A&&B);` + 后置
        //   `return false;` 会给内层 0 出口另生 `jmp <epilogue>` 中继（多 2 条）
        //   ⇒ 用 goto 把内层失败并入函数尾唯一 false 出口。
        if (!((beginTime <= time) && (time < endTime)))
            goto L_false;
        return true;
    }
L_false:
    return false;
}