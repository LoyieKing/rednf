float TeamInfoThird::GetTeamRating()
{
    float sum = 0.0f;
    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        sum += m_players[i].m_rating;
        if (m_players[i].m_rating != 0.0f)
            ++count;
    }
    if (count != 0)
        sum = sum / (float)count;
    return sum;
