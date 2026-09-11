// ORIG 0x0816bcea（size 0x1c）：+0x0c == param_1（分支形态 cmp/jne+mov 1/0）。
bool EventNewCharacterReward::isEventCharacter(int job)
{
    if (m_job == job)
        return true;
    return false;
}