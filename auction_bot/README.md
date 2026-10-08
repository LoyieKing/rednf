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

## 网页手动上架（「手动上架」标签页）

立刻以系统卖家（owner -1，显示名「拍卖行」）上架指定数量的某个物品，与经济模型上架走同一个 `client.regist_item(-1, …)`。

1. 「物品」框输入道具 ID 或名称（输入时按 PVF 名称自动补全），回车或点「查询」，显示类型、建议单价、每条数量、是否在排除名单。
2. 填「总数量」；「单价」留空 = 建议单价；「每条数量」留空 = 默认拆堆。点「立即上架」，结果里列出每条挂单的拍卖号和失败原因。

- **单价默认值**：建模道具用模型当前公允价（与模型上架一样按价位取整、夹在价格带内）；未建模道具用参考价表；都没有时必须手填。手填单价不受价格带限制。
- **拆堆**：建模道具每条 = 模型单笔挂单上限（`lot_qty.max`，如扳手 120、尖晶石 6），否则 PVF 堆叠上限（没有则 100）；都不超过 PVF 堆叠上限，单条总价不超过 20 亿。一次提交最多 200 条挂单，超过会拒绝（加大每条数量或分批）。
- **装备等不可堆叠物品**：暂不支持（耐久、随机属性等字段的上架格式未在真服验证），会直接拒绝。
- **探测**：每次提交先实时发 ASK_REGISTED_ITEM_NUM（只读）；不在拍卖服 iteminfo.dat 的物品（回包挂单数 -2 或 item_id 0）直接拒绝，不会发上架包。上架回包拍卖号为 0 也记为失败。
- **排除名单**：会显示警告，必须勾选「我确认要上架排除名单里的物品」才能提交（不硬性禁止，是用户的手动选择）。
- **与经济模型的关系**：手动挂单在 `bot_listings` 里 `note='manual'`。模型构建挂单簿时完全跳过它们：不计入系统挂单数和上限（模型照常挂自己的货）、虚拟买家不会买走、到期（2 小时）后不补挂。玩家在游戏里买走时照常对账，记为「玩家买系统货」。模型页的「系统挂单」列另外显示「另手动 N 条」。
- **自动收购**：只查询 `owner_id NOT IN (-1, 假买家)` 的挂单，永远不会买系统卖家的挂单（包括手动挂单）。
- **记录**：每条挂单写 `logs/bot.log` 和 bot.db 操作日志（类型「上架」，内容以「手动上架」开头；汇总和被拒绝的操作记在「控制」），页面下方「手动上架记录」和「系统挂单 / 成交 → 来源」可查看。

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
