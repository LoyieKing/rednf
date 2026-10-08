# -*- coding: utf-8 -*-
"""入口：python3 -m auction_bot.main [-c config.yaml]"""
import argparse
import asyncio
import os
import signal
import sys

import yaml

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def _merge(a, b):
    for k, v in (b or {}).items():
        if isinstance(v, dict) and isinstance(a.get(k), dict):
            _merge(a[k], v)
        else:
            a[k] = v
    return a


def load_config(path):
    """config.yaml + 同目录 config.local.yaml（不入库，放密码等本机设置）+ 环境变量 AUCTION_BOT_DB_PASSWORD。"""
    with open(path, encoding="utf-8") as f:
        cfg = yaml.safe_load(f) or {}
    local = os.path.join(os.path.dirname(os.path.abspath(path)), "config.local.yaml")
    if os.path.exists(local):
        with open(local, encoding="utf-8") as f:
            _merge(cfg, yaml.safe_load(f) or {})
    pw = os.environ.get("AUCTION_BOT_DB_PASSWORD")
    if pw is not None:
        cfg.setdefault("game_db", {})["password"] = pw
    return cfg


def load_items(cfg, log=print):
    from .pvf import load_item_index
    p = (cfg.get("pvf") or {}).get("path")
    if not p or not os.path.exists(p):
        log("PVF 不存在：%s，物品名称将显示为 ID" % p)
        return {}
    data_dir = os.path.join(BASE, cfg.get("data_dir", "data"))
    os.makedirs(data_dir, exist_ok=True)
    return load_item_index(p, os.path.join(data_dir, "pvf_items.json"))


def main(argv=None):
    ap = argparse.ArgumentParser(description="DNF 拍卖行机器人频道")
    ap.add_argument("-c", "--config", default=os.path.join(BASE, "config.yaml"))
    ap.add_argument("--no-web", action="store_true")
    args = ap.parse_args(argv)
    cfg = load_config(args.config)
    from .bot import Bot
    from .web import start_web
    items = load_items(cfg)
    bot = Bot(cfg, BASE, items)
    bot.log("connect", "已加载 PVF 物品 %d 个" % len(items))
    w = cfg.get("web", {})
    if not args.no_web:
        start_web(bot, w.get("bind", "0.0.0.0"), int(w.get("port", 8088)), os.path.join(BASE, "web"), args.config)
        bot.log("connect", "网页控制台：http://%s:%d/" % (w.get("bind", "0.0.0.0"), int(w.get("port", 8088))))

    async def runner():
        loop = asyncio.get_running_loop()
        for s in (signal.SIGINT, signal.SIGTERM):
            loop.add_signal_handler(s, bot.stop)
        await bot.run()
    asyncio.run(runner())
    return 0


if __name__ == "__main__":
    sys.exit(main())
