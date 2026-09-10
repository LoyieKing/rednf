CEventBase::CEventBase()
    : m_addInfo()
{
    // ORIG：*(vptr) = &vtable_08b4bae0（编译器自动）；
    // stADDINFO ctor 0x0810af10：两个 ushort 置 0xffff；+0x08 flag 置 0。
    // [b28 判词·N·帧阶梯不可达] ORIG 帧 0x18 vs 4.4 0x14：探针 /tmp/l2_p8.cpp
    // 实测 4.4 本形帧仅 {0x14,0x24} 两档——4 dead int=0x14、+double/第5 int 跳
    // 0x24（P190 double 撑帧此 TU 过冲，P31 int 垫无中间值）；stADDINFO ctor 为
    // 他 TU 外联（P210 同 TU 继承律不适用）。唯一真差 = sub 立即数 1 行，N82/P182 族。
    int d1, d2, d3, d4;
    m_isEventing = false;
