bool CAutoMarketConditionsControl::ChangeItemRarity(int* itemRarity,
                                                    int* outItemRarity,
                                                    int raritySize,
                                                    double controlRarity)
{
    // ORIG 为两条 mov 直拷（无 fld/fstp 中转）⇒ 声明与赋值分两句。
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
