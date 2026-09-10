// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CAuctionAveragePrice/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CAuctionAveragePrice（G2-4 批次）
// 参照 docs/class_func_reports/CAuctionAveragePrice.md 与 ORIG 反汇编逐函数实现。
// 原源文件 AuctionAveragePrice.cpp；数据表 auction_average_price。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN20CAuctionAveragePriceC2Ev`（ORIG 0x82f5f03）；FindPrice 类内 inline
// 成员为弱符号不参与键名，首个强符号即 CAuctionAveragePrice ctor。
#include <iostream>

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include "CEnvironment.h"
#include "DBMgr.h"
#include "GlobalData.h"

// ---- 拍卖均价表（item_id + upgrade 唯一；mangling 20CAuctionAveragePrice） ----
class CAuctionAveragePrice
{
public:
    CAuctionAveragePrice();
    ~CAuctionAveragePrice();

    bool LoadAveragePrice();
    void SetAveragePrice(int itemID, unsigned char upgrade, int price);
    int GetAveragePrice(int itemID, unsigned char upgrade) const;

private:
    std::map<int, std::vector<std::pair<unsigned char, int> > > m_averagePriceMap;
};

// ---- find_if 谓词（ORIG 独立弱符号 FindPrice，取同命名空间同名） ----
class FindPrice
{
public:
    FindPrice(unsigned char upgrade)
    {
        m_upgrade = upgrade;
    }

    bool operator()(const std::pair<unsigned char, int>& item) const
    {
        if (item.first == m_upgrade)
        {
            return true;
        }
        return false;
    }

private:
    unsigned char m_upgrade;
};

#include "CAuctionAveragePrice/CAuctionAveragePrice.inc"
}

#include "CAuctionAveragePrice/dtor_CAuctionAveragePrice.inc"
}

#include "CAuctionAveragePrice/LoadAveragePrice.inc"
}

#include "CAuctionAveragePrice/SetAveragePrice.inc"
}

#include "CAuctionAveragePrice/GetAveragePrice.inc"
}
