// ORIG IsGrowCreature @ 0x833f0ca：movzbl +0x4c 后 xor $1 —— 即 +0x4c==0 返回
// true、==1 返回 false（与 Reset 初值相反；GainExp/UseFeed/vaildItemExp 用裸字段判断，
// 本访问器供包序列化/查询路径使用，语义以汇编为准）。
// 定义点挪至 CCreatureMgr::IsGrowCreature_Equipped_Creature 之后：4.4 对同 TU 可见
// callee 收缩 caller 帧（sub $0x4），ORIG 0x833debf 为 sub $0x18（4.1 宽松帧）。
bool CCreature::IsGrowCreature()
{
    return (m_isGrow ^ 1) != 0;
}
