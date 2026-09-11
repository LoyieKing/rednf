void DisjointSkillScript::clear()
{
    // ORIG 0x084735d2：{0xb, 0, 0, 4} 四连写。
    // 定义置于 ctor 之后：4.4 对"被调函数本 TU 未定义"的 ctor 帧发 sub $0x18，
    // 与 ORIG 0x084735fe 帧形一致（探针 /tmp/near7/p7-p8）。
    m_field00 = 0xb;
    m_field04 = 0;
    m_field08 = 0;
    m_field0c = 4;
}