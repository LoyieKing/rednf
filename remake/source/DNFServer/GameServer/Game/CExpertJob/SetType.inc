// ORIG 0x849f148：*(int*)(this+0x04) = type。
// b28：字段枚举化（CExpertJob.h m_type）后 4.4 store 序翻为 LHS-first，
// 与 ORIG mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx 同形。
void CExpertJob::SetType(ENUM_EXPERT_JOB_TYPE type) { m_type = type; }
