// ---------------------------------------------------------------------------
// ORIG T 0x08a9e7e0 sz 0x88（getStrikerSkillVector.md 全文已读）：
// m_skillMap(+0x78).find((int)(unsigned short)first)；命中返回 it->second
// （vector<StrikerSkill>*），未命中返回 0。ORIG 汇编中 second 形参仅被零扩展
// 存放（-0xe 槽），从未参与 find key——忠实保留未用语义。
std::vector<StrikerSkill>* StrikerScript::getStrikerSkillVector(short first,
