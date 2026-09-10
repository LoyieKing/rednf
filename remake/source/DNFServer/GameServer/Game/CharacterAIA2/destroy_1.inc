void STAICharacterCommonScript::destroy()
{
    // ORIG 0x088c98bc：仅转发 STAIInfoScript::destroy。
    m_info.destroy();
