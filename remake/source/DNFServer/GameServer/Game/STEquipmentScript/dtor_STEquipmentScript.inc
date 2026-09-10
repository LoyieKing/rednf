STEquipmentScript::~STEquipmentScript()
{
    if (m_keyCommand)
    {
        delete m_keyCommand;
    }
    m_keyCommand = 0;
