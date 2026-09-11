// ORIG T 0x08588436：rating（位模式）写入 +0x08；+0x3c=1。
void COnlinePreliminaryTeam::SetRating(float rating)
{
    // ORIG 内联 float→int 位拷贝（mov），不调 memcpy（我们 17→ORIG 9）
    m_rating08 = rating;
    m_hasWinFlag = 1;
}