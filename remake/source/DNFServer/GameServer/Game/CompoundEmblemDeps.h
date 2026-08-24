#ifndef DNF_GAME_COMPOUNDEMBLEMDEPS_H_
#define DNF_GAME_COMPOUNDEMBLEMDEPS_H_

// ============================================================================
// CompoundEmblem 依赖还原（df_game_r ORIG）
//   - CCompoundEmblemStatistic         纹章合成统计（尺寸 0x1c = 7 int）
//   - getCompoundEmblemStatistic()     单例访问器（_Z26getCompoundEmblemStatisticv）
//   - getAvatarEmblemTypeIdxForCompountEmblem(...)  纹章位图→槽位计数
//                                          （_Z39getAvatarEmblemTypeIdxForCompountEmblemtPiRi）
//   - LineFunc(int, const char*, int, unsigned)     包分发断言日志
//                                          （_Z8LineFunciPKcij）
// 语义依据 docs/class_func_reports/_lt_global_gt_/getCompoundEmblemStatistic.md、
// LineFunc.md 与 Dispatcher_CompoundEmblem/_selectJewelTypeIndex.md。共享头，
// 修改前先经 hub 通知相关子代理。
// ============================================================================

class CCompoundEmblemStatistic
{
public:
    CCompoundEmblemStatistic();
    ~CCompoundEmblemStatistic();

    void increaseCount(int idx);
    int  getGradeCount(int idx);            // 注意 ORIG 为非 const（_ZN 非 _ZNK）
    int  getTotalCount();
    void clear();

    int m_data[7];                          // +0，尺寸 0x1c
};

CCompoundEmblemStatistic* getCompoundEmblemStatistic();

int getAvatarEmblemTypeIdxForCompountEmblem(unsigned short type, int* counts, int& total);

int LineFunc(int line, const char* func, int errNo, unsigned int mid);

#endif  // DNF_GAME_COMPOUNDEMBLEMDEPS_H_
