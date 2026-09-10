void TOD_UserState::setEnterLayer(const TOD_Layer& layer)
{
    m_data.m_todayLayer = layer;    // ORIG movzwl (%edx); mov %dx,0x14(%eax)
