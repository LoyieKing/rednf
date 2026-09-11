float CDataManager::BaseExpPenalty(int levelA, int levelB)
{
    int diff = levelB - levelA;
    float result;
    if (diff < -6)
    {
        result = 0.05f;
    }
    else if (diff < -5)
    {
        result = 0.2f;
    }
    else if (diff == -5)
    {
        result = 0.5f;
    }
    else if (diff == -4)
    {
        result = 0.75f;
    }
    else if (diff == -3)
    {
        result = 1.0f;
    }
    else if (diff == -2)
    {
        result = 1.0f;
    }
    else if (diff == -1)
    {
        result = 1.0f;
    }
    else if (diff == 0)
    {
        result = 1.0f;
    }
    else if (diff == 1)
    {
        result = 1.12f;
    }
    else if (diff == 2)
    {
        result = 1.12f;
    }
    else if (diff == 3)
    {
        result = 1.12f;
    }
    else if (diff == 4)
    {
        result = 1.0f;
    }
    else if (diff == 5)
    {
        result = 1.0f;
    }
    else if (diff == 6)
    {
        result = 0.75f;
    }
    else if (diff == 7)
    {
        result = 0.7f;
    }
    else if (diff == 8)
    {
        result = 0.6f;
    }
    else if (diff == 9)
    {
        result = 0.5f;
    }
    else if (9 < diff)
    {
        result = 0.05f;
    }
    return result;
}