bool CMission::canUpdate(const MissionClearCondition_Parameter& param) const
{
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG %edx vs 4.4 %eax（N7，探针实证），
    // 跳转/其余助记符全等，ndiff 12 封顶。
    if (m_conditionType == 0xff)
    {
        if ((param.m_mode != 1) && (param.m_mode != 2)) return 0;
    }
    else if (m_conditionType != param.m_mode)
    {
        return 0;
    }
    return _canUpdate(param);
}