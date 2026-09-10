std::string NpcMovieData::getNextMoviePath()
{
    std::string result;
    if (m_movies.empty())
    {
        return result;
    }
    if ((int)m_movies.size() > m_movieIdx)
    {
        result = m_movies[m_movieIdx];
        m_movieIdx = (unsigned)(m_movieIdx + 1) % (unsigned)m_movies.size();
    }
    return result;
