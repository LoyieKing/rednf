void COnlinePreliminaryTeam::SetRating(float rating)
{
    // ORIG 内联 float→int 位拷贝（mov），不调 memcpy（我们 17→ORIG 9）
    m_rating08 = rating;
    m_hasWinFlag = 1;
