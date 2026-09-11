void DungeonClearCondition::clear()
{
    // 定义置于 ctor 之后：4.4 对"被调函数定义未前置"的 ctor 帧发 sub $0x18，
    // 与 ORIG 0x088f29f4 帧形一致（探针 /tmp/near7/p7-p8）。
    m_0 = 0;
    m_4 = 0;
    m_8 = 0;
}