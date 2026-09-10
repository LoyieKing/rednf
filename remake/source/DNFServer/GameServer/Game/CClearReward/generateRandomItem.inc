void CClearReward::generateRandomItem(stGenerateRefData_t const& ref,
                                      stGenerateResult_t& result)
{
    // [b27 封顶] 唯一残差 = ref+0x4(m_field04) float 读：ORIG 4.1 disp 单步
    // `mov 0x4(%eax),%edx`，4.4 对 cast 指针/具名字段/偏移表达式均折
    // `add $0x4;mov (%eax)` 两步（探针 35/36/37），N81 同族不可造。
    if (m_random.generateNumber(1, 10000) < m_dropKindProb[0])
    {
        _generateRandomItem(*reinterpret_cast<stGenerateRefClearReward_t const*>(
                                reinterpret_cast<char const*>(&ref) +
                                kRefClearRewardCtx),
                            *reinterpret_cast<int const*>(&ref),
                            result,
                            ref.m_field04f);
    }
    else
    {
        _generateSpecificItem(*reinterpret_cast<stGenerateRefClearReward_t const*>(
                                  reinterpret_cast<char const*>(&ref) +
                                  kRefClearRewardCtx),
                              result);
    }
