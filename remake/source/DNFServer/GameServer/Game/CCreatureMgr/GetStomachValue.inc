float CStomach::GetStomachValue(CREATURE_STATE state, bool growFlag)
{
    if (!growFlag)
        return 100.0f;

    int curTime = GlobalData::s_systemTime_.getCurSec();
    float delta = (float)(curTime - m_lastTime);
    // ORIG 0x8336d1b：无 `int s` 槽；三分支二分下探树 `cmp $1;je B1 /
    // cmp $1;jl DEF / cmp $3;jg DEF`（技巧 34，switch 直写枚举形参）。
    switch (state)
    {
    case CREATURE_STATE_1:
    {
        m_value += delta / -60.0f;
        // [b28d-solo-CCreatureMgr AE] ORIG 0x8336d80 `test $0x45,%ah; sete`：
        // fucompp 比较 st0=0.0 / st1=m_value，掩码 0x45 含 C3 ⇒ 命中条件为
        // 0.0>m_value（严格 `<`）；此前写 `<=` 得 `test $0x5`（C3 位差）⇒ 改
        // `m_value < 0.0f` 即逐条全等。
        if (m_value < 0.0f)
            m_value = 0.0f;
        break;
    }
    case CREATURE_STATE_2:
    case CREATURE_STATE_3:
    {
        m_value += delta / 360.0f;
        if (m_value > 100.0f)
            m_value = 100.0f;
        break;
    }
    default:
        break;
    }
    m_lastTime = curTime;
    return m_value;
}
