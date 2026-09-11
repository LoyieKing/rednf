// ============================================================================
// LevelupSupportEventManger::Load —— 报告 Load.md @0x08146e3c。
// cMyTrace(pretty,0x18,9,true,false) → 清脚本 map → loadScript()==0 才继续：
// 清 m_stepIndexMap/m_events → 按 map 升序写入 level→档位序号 与
// EventInfo(level 截断 ushort, 0)。迭代器递增为 ORIG 的 operator++(int) 形态。
// [b28d-N82] fresh 复验 110/114：残差 = 迭代器三槽阶梯（ORIG -0x3c/-0x40/-0x44
// 深→浅，4.4 -0x18/-0x2c/-0x44）与 4.4 缺 push %ebx（callee-saved 需求差）
// —— P28/N82 槽方向族；块序/调用点全等，语义等价。
// [M2S1] 旧判词仅剩两处真旋钮（ORIG 有、我方无）：① ORIG 0x08146ede/0x08146ef6
// 在循环外各一次 begin→-0x3c / end→-0x40（我方旧形把 end() 留在条件里每次迭代
// 重算，多 1 call+2 条 / 槽 -0x2c）⇒ 提升为具名 endIt；② ORIG 0x08146f30
// `_ZNSt4pairIKiiEC1IRS0_iEEOT_OT0_`（第二模板参 `i`=int&& 右值）证明原码为
// `value_type(it->first, stepIndex++)` 单次构造，我方旧形 make_pair 中间临时 +
// 转换 ctor（-0x18/-0x20 两槽两 call）⇒ 直接 value_type + 后置++。
// ============================================================================
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
bool LevelupSupportEventManger::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 9, true, false);
    trace("\t- Loading Levelup Support Event script - ");

    m_script.m_stepItems.clear();
    if (m_script.loadScript() == 0)
    {
        m_stepIndexMap.clear();
        m_events.clear();

        // ORIG 0x08146ed4 begin→-0x3c 先、0x08146ef6 end→-0x40 后；4.4 按声明序浅→深
        // ⇒ it 先声明（浅）、endIt 后（深），与 ORIG 槽序一致。
        std::map<int, std::vector<LevelupSupportSciprt::ItemInfo>*>::iterator it =
            m_script.m_stepItems.begin();
        std::map<int, std::vector<LevelupSupportSciprt::ItemInfo>*>::iterator endIt =
            m_script.m_stepItems.end();   // 循环外单次求值（非每次迭代重算）
        // [M2FreeNext15A] ORIG 0x08146f9b `movl $0x0,-0xc` 发在 end() 调用**之后** ⇒
        // 原码 stepIndex 的初始化位点晚于两个迭代器（P328⑥ 同族）。
        int stepIndex = 0;   // ORIG -0xc 循环外计数器
        for (; it != endIt; it++)
        {
            // ORIG 0x08146f30 单次 value_type ctor（第二实参 int&& = 后置++临时）
            m_stepIndexMap.insert(std::map<int, int>::value_type(it->first, stepIndex++));
            // ORIG：*pair.first（int 键）截断 ushort 作为达标等级门槛，state=0 默认未领。
            m_events.push_back(EventInfo(static_cast<unsigned short>(it->first), 0));
        }
    }
    // [N1C·2026-09-11 · 语义轴修正 → IDENTICAL] ORIG 0x08146ea7 `sete %al` +
    // 0x08146eac `je 0x08146fc9`（= false 块 `mov $0x0,%eax`）⇒ 原码为
    // `if (loadScript() == 0) { 成功体 } else { return false; } return true;`
    // （loadScript 是**错误码式**：0=成功）。旧 goto 串 `if (!loadScript()) goto
    // load_false` 生成 `jne <false 块>` ⇒ loadScript()==0 时直接 return false，
    // **返回值与 ORIG 相反**（真语义 bug，本次修正）。改 if/else 后 `je` 极性、
    // 尾块序（+0x18d false / +0x194 true / +0x199 epilogue）与 ORIG 全等：
    // 探针核账 110/110、blocks=0（IDENTICAL）。stepIndex 初值位点（ORIG
    // 0x08146f9b，晚于两迭代器声明）保持原状。
    else
    {
        return false;
    }
    return true;
}