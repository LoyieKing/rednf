// TagRecommendNewbieOption_impl.cpp —— tagRecommendNewbieOption W 符号单 TU 载体
// （C1/C2/D1/D2/clear 均类内内联于 TagRecommendNewbieOption.h，本 TU 以
// force_emit 强制实例化产出符号，供 check_tu_game_orig.sh 与 ORIG 对比）。
// ORIG：C1 0x0891ad16 / clear 0x0891ad2a / D1 0x0893908c（均 W）。
// 报告：/tmp/df_batches/OrigNewbie.md
#include "TagRecommendNewbieOption.h"

namespace
{
// 内联成员无调用点不产出符号；此处构造/析构/clear 强制实例化。
void force_emit()
{
    tagRecommendNewbieOption t;
    t.clear();
}
}
