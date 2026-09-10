void CExpertJob::SetType(ENUM_EXPERT_JOB_TYPE type) { m_type = type; }

// ORIG 0x85d3d7e：IObject 基类构造 -> Rectangle(+0x04) -> CUser*+0x14 清零 ->
// std::string(+0x18) 默认构造。
