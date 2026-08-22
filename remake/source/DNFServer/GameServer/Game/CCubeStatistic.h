// ============================================================================
// df_game_r 还原 —— CCubeStatistic（购买流程引用的缺失符号批次）
// 参照 docs/class_func_reports/CCubeStatistic/*.md + ORIG 反汇编。
// 与 Statics/CubeStatistics.cpp（df_statics_r）不同：本文件为 game 域构建的
// CCubeStatistic（GetInstanceCubeStatistic 单例 + findCubeStuff/sendCubeStatistic/
// collectCubeStatistics），不持有 ORIG 的单例 m_data map（这些方法不访问成员）。
// ============================================================================
#ifndef CCUBESTATISTIC_H_
#include <map>
#include <string>

// ---- CUBE_STATISTIC_FIELD（ORIG 全局枚举，mangled 20CUBE_STATISTIC_FIELD） ----
// 仅确认 field 0x6a 被 CSecretShop::IBuyRule::LogCubeStatistic 使用；其余值
// 「推断」为按序递增（不影响类型 mangling，A 签名只取决于类型名与长度）。
enum CUBE_STATISTIC_FIELD
{
    CUBE_STATISTIC_FIELD_0 = 0,
    CUBE_STATISTIC_FIELD_CUBE_ITEM = 0x6a   // [推断] CSecretShop.cpp 使用值
};

class CUser;
class CItem;

// ---- CCubeStatistic（game 域；ORIG 单例对象 0x94733d0，ctor 仅 ret） ----
class CCubeStatistic
{
public:
    CCubeStatistic();   // ORIG 0x86103b8（空构造）
    bool findCubeStuff(int groupValue);   // ORIG 0x860de96
    int  sendCubeStatistic(int itemIndex, int count, CUser* user,
                           CUBE_STATISTIC_FIELD field);   // ORIG 0x860e0d8
    void collectCubeStatistics(int itemIndex, int count, CUser* user,
                               CUBE_STATISTIC_FIELD field);   // ORIG 0x860dfb8
    void collectCubeStatistics(CItem* item, int count, CUser* user,
                               CUBE_STATISTIC_FIELD field);   // ORIG 0x860e07a
};

// ---- 全局单例 / 表（ORIG 在 game 域） ----
CCubeStatistic* GetInstanceCubeStatistic();   // ORIG 0x860de50
std::map<std::string, int>& getItemGroupNameTable();   // ORIG 0x88c0409

#endif  // CCUBESTATISTIC_H_