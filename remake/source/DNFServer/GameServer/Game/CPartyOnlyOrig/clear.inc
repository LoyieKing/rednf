// RidableObject::clear 定义置于本 TU（与 CDungeon.cpp 中的 C1 调用者分离）：
// 4.4 unit-at-a-time 对 TU 内可见 callee 塌缩 outgoing 帧（C1 sub $0x4），
// ORIG 4.1 恒发 sub $0x18（探针 /tmp/nl4p/p6.cpp vs p7.cpp 同族实证）。
void RidableObject::clear()
{
    // ORIG 0x830e47c：前 5 个字段写 -1，+0x14（m_field18）写 0xc8（200）。
    m_x = -1;
    m_y = -1;
    m_field0c = -1;
    m_field10 = -1;
    m_field14 = -1;
    m_field18 = 0xc8;
}