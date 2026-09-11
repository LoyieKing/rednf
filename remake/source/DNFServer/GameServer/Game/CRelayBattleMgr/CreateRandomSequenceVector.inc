void CRelayBattleMgr::CreateRandomSequenceVector(std::vector<int>& seq,
                                                 int count)
{
    int n;
    int val;
    int i;
    if ((unsigned int)count > seq.size())
    {
        count = (int)seq.size();
    }
    // b28d-31: ORIG 0x085e8e1c 块序 = else 体（count>=1 随机段）在前、
    // count<1 递减段在后（P222/P272 块序交换）。
    if (count >= 1)
    {
        CRandomValue rv(seq, 1, 100, 10);
        std::generate(seq.begin(), seq.end(), rv);
        std::sort(seq.begin(), seq.end(), CDescending());
        std::random_shuffle(seq.begin(), seq.begin() + count);
    }
    else
    {
        n = (int)seq.size();
        val = n;
        for (i = 0; i < n; ++i)
        {
            seq[i] = val;
            --val;
        }
    }
}