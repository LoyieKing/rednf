// ---- AvatarRoulette::getStikerList ORIG T 0x08911dfe ----
// 同 getAvatarList 结构；out = m_stikers[job]（+0x90 再 +0x8 → +0x98）。
void AvatarRoulette::getStikerList(bool& isOk, ENUM_CHARACTERJOB job,
                                   std::vector<int>& out)
{
    isOk = false;
    if (job >= 0 && job <= 10)
    {
        out = m_stikers[job];
        isOk = true;
    }
    return;
}