int getRandomChampionElement(const STAICharacterCommonScript* script, int seed)
{
    int result = 4;
    const int size = (int)script->m_championElements.size();
    srand(seed);
    if (size == 0)
    {
        result = rand() % 4 + 1;
    }
    else
    {
        result = script->m_championElements[rand() % size];
    }
    return result;
