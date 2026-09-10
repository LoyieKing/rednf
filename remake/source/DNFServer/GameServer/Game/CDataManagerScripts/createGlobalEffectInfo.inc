int createGlobalEffectInfo(std::vector<stGlobalEffect_t>& out,
                           std::vector<int>* source)
{
    out.clear();
    if (source->size() % 3 != 0)
        return 0;
    std::vector<int>::iterator it = source->begin();
    while (it != source->end())
    {
        stGlobalEffect_t effect;
        effect.m_field0 = *it++;
        effect.m_field4 = static_cast<float>(*it++) / 10000.0f;
        effect.m_field8 = *it++;
        out.push_back(effect);
    }
    return 1;
