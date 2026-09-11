// ORIG GetName @ 0x833f058：返回 this+0x1c。
// 定义点挪至 CCreatureMgr::GetCreatureName 之后：同 TU 可见 callee 使 4.4 caller
// 帧塌缩为 sub $0x4，ORIG 0x833a68b sub $0x18 要求 callee 在调用点不可见。
const char* CCreature::GetName()
{
    return m_name;
}
