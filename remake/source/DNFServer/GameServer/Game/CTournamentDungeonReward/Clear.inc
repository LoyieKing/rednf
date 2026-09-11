// ORIG T 0x08284750 (size 0x1f)：外联强符号定义，须先于本 TU 全部 caller
// （ctor 0x8284770 / ResetReward 0x8284820）——P210② 帧继承（4.4 按已见
// callee 定义精确继承 outgoing ⇒ ResetReward sub $0x14 与 ORIG 一致）。
void TournamentRewardType::Clear()
{
    m_id = -1;
    m_field04 = 0;
    m_flag08 = static_cast<char>(0xff);
}