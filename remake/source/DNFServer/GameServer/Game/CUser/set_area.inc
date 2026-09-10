void CUser::set_area(int area)
{
    int frame_pad; (void)frame_pad;   // ORIG 0x8647867 sub $0x18：2 outgoing 槽 + 16B 帧舍入（4.1 特征），dead 局部无指令
    set_before_area(m_field8cfbc);
    m_field8cfbc = area;
