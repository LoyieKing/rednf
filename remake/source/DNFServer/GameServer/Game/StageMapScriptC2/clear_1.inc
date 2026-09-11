// 两 ctor 置于各自 clear() 定义前：callee 未知时 4.4 的 outgoing 保守分配
// 恰为 ORIG 帧 0x18；clear 定义在前则继承其小帧（sub $0x4）失配。
void StageData::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
}