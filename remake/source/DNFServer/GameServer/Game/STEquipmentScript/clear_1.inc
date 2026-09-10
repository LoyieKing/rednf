void STEquipmentAniScript::clear()  // ORIG 0x0898ce62（m_field0=0，各 string 置 ""）
{
    // [SweepSTEquip 2026-09-09] ORIG 形：`.clear()` 实为 `s = ""`（operator=(const
    // char*)，实参 "" 落 0x4(%esp)）；第一个循环 `i<=1` 只清 m_arr10，第二个循环
    // `i<=3` 同体清 m_arr2c[i]+m_arr3c[i]（0x0898cf65/0x0898cf84 两个赋值）；
    // +0x60 在 vector clear 之后（0x0898d026）。修后 118=118 逐条同形。
    m_field0 = 0;
    m_str04 = "";
    m_str08 = "";
    m_str0c = "";
    // ORIG 0x0898ceb3/0x0898ceba 两次 `movl $0x0,-0xc(%ebp)` ⇒ 具名 `int i = 0;`
    // 后接 `for (i = 0; …)`；第二个循环（0x0898cf5c）只清一次 ⇒ 复用同一 i。
    // 第二个循环体含**两个**数组赋值（0x0898cf65 m_arr2c[i] + 0x0898cf84
    // m_arr3c[i]），同一 i 同一循环。
    int i = 0;
    for (i = 0; i <= 1; ++i)
    {
        m_arr10[i] = "";
    }
    m_str18 = "";
    m_str1c = "";
    m_str20 = "";
    m_str24 = "";
    m_str28 = "";
    for (i = 0; i <= 3; ++i)
    {
        m_arr2c[i] = "";
        m_arr3c[i] = "";
    }
    m_str4c = "";
    m_str50 = "";
    m_str54 = "";
    m_str58 = "";
    m_vec64.clear();
    m_vec70.clear();
    // ORIG 0x0898ce62 未清 +0x5c（拷贝构造/operator= 均拷贝 +0x5c，故 +0x5c 是 string
    // 成员，仅 clear 漏清——按 ORIG 实证保留不清）
    m_str60 = "";
