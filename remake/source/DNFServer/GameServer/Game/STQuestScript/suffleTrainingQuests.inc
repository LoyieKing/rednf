void TrainingQuestScript::suffleTrainingQuests()
{
    int day = computeDayIndex();
    int count = m_selectProb.m_dayProb[day].at(0);
    // ORIG 0x08aafcbc：MyRandom(count) 作随机数器（3 参 random_shuffle）。
    MyRandom rnd(count);
    for (int i = 0; i <= 0x45; ++i)
    {
        // b28c-28（P223 + P168a①）：ORIG 0x08aafd2f 段为
        // `call empty; test %al,%al; jne <nop 岛>` + then 体尾 `jmp <岛后>`——
        // 即「正向 empty 判定 + goto 尾空标签」形；`if(!empty()){}` 被 4.4 折成
        // `xor$1; test; je`（极性反、少 jmp 岛）。
        if (m_questProb[i].empty())
            goto SHUFFLE_DONE;
        std::random_shuffle(m_questProb[i].begin(), m_questProb[i].end(), rnd);
SHUFFLE_DONE:;
    }
}