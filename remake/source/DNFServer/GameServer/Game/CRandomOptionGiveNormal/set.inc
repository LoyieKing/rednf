void CRandomOptionGiveNormal::set(
    std::map<RandomApplicationKey, std::map<int, RandomOptionGive> >* typeMap,
    std::map<unsigned long long, OptionGroupChoiceValue>* choiceMap)
{
    m_pMapType4 = typeMap;
    m_pMapChoice8 = choiceMap;
}
