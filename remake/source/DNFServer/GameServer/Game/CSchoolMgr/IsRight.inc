bool CSchoolMember::IsRight(CUser* user)
{
    // N96 清绑：原 `register bool right asm("eax")` 已删。ORIG 无 right 槽
    // （&& 短路直落 mov$1/mov$0 岛）⇒ 还原为单 return 短路形。清绑前后 25=25、
    // 与 ORIG（objdump 0x085690dc，报告缺失）逐条全等（仅 .o 重定位地址差）
    // ⇒ 残差 0，无需 N96+N18a 封顶。
    return m_user->GetSchoolNo() == user->GetSchoolNo() && m_user != user;
