# -*- coding: utf-8 -*-
"""
一次性工具（独立连接，不依赖主进程）：
  python3 -m auction_bot.cli probe 3187 3205          # 探测物品是否在拍卖服 iteminfo.dat（只读）
  python3 -m auction_bot.cli search 3187 1047         # 协议检索当前挂单
  python3 -m auction_bot.cli buy-test --auction-id 1 --count 1   # 受控收购测试（买家 = 配置 buyer_id）
注意：每次运行都会作为一个新频道 REGIST 一次，退出时断开（拍卖服会注销该频道）。
"""
import argparse
import asyncio
import json
import os

from .main import BASE, load_config, load_items
from .client import AuctionClient


def out(kind, msg, level="info", **kw):
    print("[%s] %s: %s" % (level, kind, msg), flush=True)


async def amain(args, cfg):
    a = cfg.get("auction_server", {})
    c = AuctionClient(a.get("host", "127.0.0.1"), int(a.get("port", 30803)), out, float(a.get("request_timeout", 8)))
    await c.connect()
    try:
        if args.cmd == "probe":
            for i in args.ids:
                print(i, "now_registed_num =", await c.probe_item(int(i)))
        elif args.cmd == "search":
            ls = await c.search_items([int(x) for x in args.ids])
            for l in ls:
                print(json.dumps({k: l[k] for k in ("auction_id", "item_id", "count", "price", "instant_price",
                                                    "unit_price", "owner_name", "expire_hours")}, ensure_ascii=False))
            print("共 %d 条" % len(ls))
        elif args.cmd == "buy-test":
            from .bot import Bot
            bot = Bot(cfg, BASE, load_items(cfg))
            bot.client = c
            bot.loop = asyncio.get_running_loop()
            bot._check_buyer_id()
            r = await bot.auto_buy(force_auction_id=args.auction_id, force_count=args.count)
            print("结果：", r)
        elif args.cmd == "list-test":
            from .bot import Bot
            bot = Bot(cfg, BASE, load_items(cfg))
            bot.client = c
            bot.loop = asyncio.get_running_loop()
            await bot.restock()
    finally:
        await c.close()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-c", "--config", default=os.path.join(BASE, "config.yaml"))
    sp = ap.add_subparsers(dest="cmd", required=True)
    p = sp.add_parser("probe"); p.add_argument("ids", nargs="+")
    p = sp.add_parser("search"); p.add_argument("ids", nargs="+")
    p = sp.add_parser("buy-test"); p.add_argument("--auction-id", type=int, required=True); p.add_argument("--count", type=int, default=1)
    sp.add_parser("list-test")
    args = ap.parse_args()
    asyncio.run(amain(args, load_config(args.config)))


if __name__ == "__main__":
    main()
