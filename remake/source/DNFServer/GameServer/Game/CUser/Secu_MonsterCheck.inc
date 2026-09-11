// ORIG C1 0x08695ab0：清 +4/+5/+6（m_active223/224/225）。
// 定义挪至 Secu_HackLogCheck C1 之后（技巧 31 规则2：callee 同 TU 前置定义
// 会收紧 caller 帧；挪后 caller 0x8695ad3 ORIG sub $0x18 宽松帧恢复）。
Secu_MonsterCheck::Secu_MonsterCheck()
{
    m_active223 = 0;
    m_active224 = 0;
    m_active225 = 0;
}