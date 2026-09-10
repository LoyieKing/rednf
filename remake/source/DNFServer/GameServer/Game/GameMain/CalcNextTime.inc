int TimerStatisticLevel::CalcNextTime()
{
    std::time_t now = static_cast<std::time_t>(s_systemTime_.getCurSec());
    std::tm cur;
    localtime_r(&now, &cur);
    // [b28d-solo-GameMain] 槽序实验：ORIG next@-0x14 / found@-0xd（next 先于
    // found 分配）⇒ next 提前声明（P244 声明/赋值分离）。
    std::time_t next;
    // ORIG 0x08635944 `movb $0x0,-0xd` 位于 target 拷贝**之前**（声明序证据）。
    bool found = false;
    std::tm target = cur;
    target.tm_min = 0;
    target.tm_sec = 0;
    for (int i = 3; i >= 0; --i)
    {
        // ORIG 0x086359a1 先载 hour 到 %edx 再载表项到 %eax（`cmp %eax,%edx; jl`）
        // ⇒ 源形 `cur.tm_hour >= m_nTimeTable[i]`（交换操作数翻装载序）。
        if (cur.tm_hour >= m_nTimeTable[i])
        {
            target.tm_hour = m_nTimeTable[i + 1];
            found = true;
            break;
        }
    }
    next = mktime(&target);
    // [b28d-solo-GameMain] 比较方向旋钮：ORIG `cmp -0x14(%ebp),%eax`（dst=%eax）
    // vs 4.4 `cmp %eax,-0x14(%ebp)`（N18b）；试交换操作数。
    if (!found || now > next)
    {
        target.tm_hour = m_nTimeTable[0];
        target.tm_mday += 1;
        next = mktime(&target);
    }
    // ORIG 0x08635a24：`add %eax,-0x14(%ebp); mov -0x14(%ebp),%eax` —— 两语句形
    // （+= 后再 return 具名局部），单表达式 `next + call()` 会被 4.4 折成 `add mem,%eax`。
    next += get_rand_int(300);
    return static_cast<int>(next);
