bool CSyncScript::truncate_charac_action_point_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return APSystem::CSyncScript::TruncateDescTable();
}