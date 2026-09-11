// ---------------------------------------------------------------------------
// CAutoMarketConditionsControl::ChangeItemRarity —— ORIG T 0x082f8c8c
// （objdump 逐段转写；报告缺失）。m_field380==0 → false。
// out[2..5]=item[2..5]; i=2..0 向下：
//   delta=item[i+1]-item[i]; acc+=(int)(delta*factor)(round-to-nearest);
//   out[i]=item[i]-acc; if(out[i]<0) out[i]=0; 恒返 true。
// item/out 两表各取前 4 项打 trace（两次独立匿名计数器）。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureB_Misc] N 封顶：语义/调用点闭合（p167 CLEAR）。
// 已修：__PRETTY_FUNCTION__、logCount/logCount2 符号名（P187）+ 后置自增序、
// cMyTrace 第三参 0、格式串删多余 %s、P191 i 双初始化 + 两循环复用同槽、
// P59 正向包裹、P227 全表达式临时链。残余 = N85（4.4 对 double 局部拷贝恒
// fld/fstp 中转，ORIG 恒 2×mov；探针 /tmp/b28d_lcbm/p4.cpp 六形全败）⇒ 帧 +0x10。
bool CAutoMarketConditionsControl::ChangeItemRarity(int* itemRarity,
                                                    int* outItemRarity,
                                                    int raritySize,
                                                    double controlRarity)
{
    // ORIG 为两条 mov 直拷（无 fld/fstp 中转、无中间临时槽 ⇒ 帧少 0x10）。
    // [L3FLinkClosureQuickParty 2026-09-11] 负结论：五形实测均无法消除 4.4 的
    // double 拷贝中转（均为 214→221/218 或与声明+赋值形同）：①`double factor =
    // controlRarity;` ②`const double factor = controlRarity;` ③P246/P180 u64 别名
    // 位拷贝（221，多 lea/mov 对）④`__builtin_memcpy(&factor,&controlRarity,8)`
    // （221，退化为 memcpy 调用序）⑤`*(double*)&factor = *(const double*)&controlRarity;`
    // （218）。⇒ N85 封顶（4.4 对 double 局部拷贝恒 `mov→临时槽 + fld/fstp`）。
    double factor;
    factor = controlRarity;                      // ebp+0x18/0x1c 双槽载入
    // P59/P257 正向包裹：ORIG `test; je <mov $0 尾块>`，成功路径 mov $1 fall-through。
    if (m_field380 != 0)
    {
      {                                          // 头部 trace（0x82f8cf4..）
        // P187：ORIG 符号 _ZZ...E8logCount；后置自增形复现 sete/lea 序。
        static int logCount;                     // ORIG b 0x0943bd70
        if (logCount++ % 1000 == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xf4, 0)(
                "ChangeItemRarity itemRarity[%d,%d,%d,%d], "
                "outItemRarity[%d,%d,%d,%d], "
                "itemRaritySize(%d), controlRarity(%lf)",
                itemRarity[0], itemRarity[1], itemRarity[2], itemRarity[3],
                outItemRarity[0], outItemRarity[1], outItemRarity[2],
                outItemRarity[3], raritySize, factor);
        }
      }

      // P191/P160-162：ORIG i 双初始化（`movl $0,-0x20` 后 `movl $2,-0x20`），
      // 且两个循环复用同一 i/acc 槽。
      int acc = 0;                                // ebp-0x24
      int i = 0;
      for (i = 2; i <= 5; ++i)
          outItemRarity[i] = itemRarity[i];

      for (i = 2; i >= 0; --i)
      {
          int delta = itemRarity[i + 1] - itemRarity[i];
          acc += static_cast<int>(delta * factor); // fnstcw RC=nearest 后 fistpl
          outItemRarity[i] = itemRarity[i] - acc;
          if (outItemRarity[i] < 0)
              outItemRarity[i] = 0;
      }

      {                                          // 尾部 trace（0x82f8ebf..）
        static int logCount2;                    // ORIG b 0x0943bd74
        if (logCount2++ % 1000 == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x114, 0)(
                "ChangeItemRarity outItemRarity[%d,%d,%d,%d]",
                outItemRarity[0], outItemRarity[1], outItemRarity[2],
                outItemRarity[3]);
        }
      }
      return true;
    }
    return false;
}