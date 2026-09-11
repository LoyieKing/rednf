void CUser::resetPlayExpAdd()
{
    m_gameOption.m_playExpAdd = 0;  // 字段 unsigned 化后直写复原 ORIG `movl $0x0,0x703a4(%eax)` 单条
}