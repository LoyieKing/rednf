# DNF 拍卖行机器人频道（auction_bot）

伪装成一个游戏频道连接拍卖服 `df_auction_r`（127.0.0.1:30803），用 GA 协议：

- **自动上架**：以系统卖家 `owner_id = -1`（拍卖行显示为系统 NPC 名）按配置补货：基础价、价格波动、目标挂单数、数量范围。
- **自动收购**（默认关闭）：按概率收购价格合理的玩家挂单，买家为假角色 `1999999999`。
- **网页控制台**：操作日志、行情图表（走势 / K 线 / 箱线 / 成交量 / 均价 / 分位数）、拍卖行检索（名称或 ID，真人+系统挂单）、系统挂单与成交记录、孤儿邮件清理预览。
- **孤儿邮件清理**（默认关闭 + 演练）：假买家收到的 `postal` / `letter` 行。

不修改任何服务端二进制；游戏库只读（孤儿清理显式开启后才定向 DELETE）。行情数据存在本目录 `data/bot.db`（SQLite），不写游戏库。

## 使用

```bash
cd /home/loyieking/dnf_workspace/auction_bot
./start.sh        # 后台启动；网页 http://localhost:8088/ （Windows 浏览器可直接打开）
./status.sh       # 状态
./stop.sh         # 停止（已上架的系统挂单保留，2 小时后由拍卖服自动下架）
./run_tests.sh    # 离线单测（不连真服）
tail -f logs/bot.log
```

一次性工具（各自单独建一个频道连接，用完断开）：

```bash
python3 -m auction_bot.cli probe 3037 3036      # 物品是否在拍卖服 iteminfo.dat（-2 = 不在）
python3 -m auction_bot.cli search 3037 1047     # 协议检索当前挂单
python3 -m auction_bot.cli buy-test --auction-id 1 --count 1   # 受控收购 1 个（会真实成交！）
```

## 配置（config.yaml）

| 项 | 说明 |
|---|---|
| `seller.items[]` | 上架物品：`item_id`、`base_price`（缺省取参考价表）、`target_listings`、`count_min/max`、`volatility` |
| `seller.max_new_listings_per_cycle / per_hour` | 上架限速 |
| `seller.default_drift / mean_revert_days` | 公允价随机游走（让行情自然波动） |
| `seller.clamp_to_reference_band` | 价格限制在参考价表 下限~上限 |
| `buyer.enabled` | 自动收购开关（默认 false） |
| `buyer.max_premium / probability / max_gold_per_day / max_buys_per_hour` | 收购条件与预算 |
| `orphan_cleanup.enabled / dry_run` | 孤儿邮件清理（默认关闭 / 演练） |
| `market.scan_interval` | 行情扫描间隔 |
| `web.bind / port` | 网页监听 |

修改后在网页「概览 → 重新加载配置」或重启。

## 协议要点（来自 remake 源码 + ORIG 反汇编核对）

- 包头 18 字节 `<bbIIII`；拍卖服**不校验** check_sum / sequence；sLength 必须与定长一致（<0x12 会让解析死循环）。
- 每个连接只 REGIST 一次；拍卖服无空闲断线、无频道数量上限；断开时自动注销频道。
- 系统挂单有效期写死 7200 秒（`Auction` 构造函数），不可配置。
- 物品不在 `iteminfo.dat` 时 REGIST_ITEM 返回“成功”但拍卖号为 0 —— 机器人先用无副作用的 ASK_REGISTED_ITEM_NUM 探测（-2 = 不存在）。
- 检索每次最多 20 个道具 ID、每页 60 条；参数非法时拍卖服不回包（本地先校验）。
- 堆叠品挂单：`price = -1`（仅一口价）、`instant = 单价×数量`、`unit_price` 填写，玩家可部分购买（BUY_ITEM_APIECE）。
- 玩家购买系统挂单：金币不发给任何人（owner -1 无邮件），物品邮寄给买家。机器人收购玩家挂单：卖家正常收到金币邮件，物品邮件寄给 1999999999（孤儿邮件）。
