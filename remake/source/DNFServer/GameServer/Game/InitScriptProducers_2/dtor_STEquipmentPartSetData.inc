STEquipmentPartSetData::~STEquipmentPartSetData()
{
    if (m_script != 0)
        delete m_script;
    m_script = 0;
