// ============================================================================
// b28c-lane3：Village::get_gate_area 定义由 Village.cpp 迁入（P210 帧继承②）。
// ORIG 中 Village 函数与 GameWorld 函数地址相邻（get_gate_area 0x086c3b58 →
// GameWorld::C1 0x086c3d4a）⇒ 同 TU，4.1 对本 caller 精确继承小帧 sub $0x4；
// 我方拆 TU 后 4.4 仅见声明恒保守 sub $0x18。Village.cpp 内无本函数调用
//（grep 实证），符号全局唯一、mangled 不变。
// ============================================================================
int Village::get_gate_area()
{
    for (int i = 0; i < m_areaCount; ++i)
    {
        if (m_pAreas[i].m_areaType == 1)
            return i;
    }
    return 1;
}