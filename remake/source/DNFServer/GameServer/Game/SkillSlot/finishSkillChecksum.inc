int finishSkillChecksum(int* sum, int key)
{
    if (sum == 0)
    {
        return 0;
    }
    *sum = *sum ^ key;
    return *sum;
