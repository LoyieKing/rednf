# -*- coding: utf-8 -*-
"""Web 控制台（标准库 http.server，线程模式）。"""
import json
import os
import re
import threading
import time
import traceback
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlparse

from . import market

MIME = {".html": "text/html; charset=utf-8", ".js": "application/javascript; charset=utf-8",
        ".css": "text/css; charset=utf-8", ".svg": "image/svg+xml", ".ico": "image/x-icon"}


def resolve_items(bot, q, limit=20):
    """把检索词解析为道具 ID 列表：纯数字 = 道具 ID；否则按 PVF 名称模糊匹配。"""
    q = (q or "").strip()
    if not q:
        return []
    ids = []
    for part in re.split(r"[,，\s]+", q):
        if part.isdigit():
            ids.append(int(part))
    if ids:
        return ids[:limit]
    ql = q.lower()
    hits = []
    for iid, it in bot.items.items():
        n = it.get("name") or ""
        if not n or ql not in n.lower():
            continue
        score = (0 if n == q else 1, 0 if it.get("kind") == "stackable" else 1, len(n), iid)
        hits.append((score, iid))
    hits.sort()
    return [i for _, i in hits[:limit]]


def make_handler(bot, web_dir, cfg_path):
    class H(BaseHTTPRequestHandler):
        server_version = "AuctionBot/1.0"

        def log_message(self, fmt, *args):
            pass

        def _json(self, obj, code=200):
            data = json.dumps(obj, ensure_ascii=False, default=str).encode("utf-8")
            self.send_response(code)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.send_header("Content-Length", str(len(data)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(data)

        def _static(self, path):
            if path in ("", "/"):
                path = "/index.html"
            fp = os.path.normpath(os.path.join(web_dir, path.lstrip("/")))
            if not fp.startswith(os.path.normpath(web_dir)) or not os.path.isfile(fp):
                self.send_error(404)
                return
            with open(fp, "rb") as f:
                data = f.read()
            self.send_response(200)
            self.send_header("Content-Type", MIME.get(os.path.splitext(fp)[1], "application/octet-stream"))
            self.send_header("Content-Length", str(len(data)))
            self.send_header("Cache-Control", "no-cache")
            self.end_headers()
            self.wfile.write(data)

        def do_GET(self):
            u = urlparse(self.path)
            qs = {k: v[-1] for k, v in parse_qs(u.query).items()}
            try:
                if not u.path.startswith("/api/"):
                    return self._static(u.path)
                fn = getattr(self, "api_" + u.path[5:].replace("/", "_"), None)
                if not fn:
                    return self._json({"error": "未知接口"}, 404)
                return self._json(fn(qs))
            except Exception as e:
                return self._json({"error": str(e), "trace": traceback.format_exc()[-800:]}, 500)

        def do_POST(self):
            u = urlparse(self.path)
            n = int(self.headers.get("Content-Length") or 0)
            try:
                body = json.loads(self.rfile.read(n) or b"{}") if n else {}
                fn = getattr(self, "post_" + u.path[5:].replace("/", "_"), None) if u.path.startswith("/api/") else None
                if not fn:
                    return self._json({"error": "未知接口"}, 404)
                return self._json(fn(body))
            except Exception as e:
                return self._json({"error": str(e)}, 500)

        # ---------- GET ----------
        def api_status(self, qs):
            st = bot.status()
            st["overview"] = market.overview(bot.store, 1)
            for r in st["overview"]:
                r["name"] = bot.item_name(r["item_id"])
            return st

        def api_logs(self, qs):
            where, args = ["1=1"], []
            if qs.get("kind"):
                where.append("kind=?")
                args.append(qs["kind"])
            if qs.get("level"):
                lv = {"debug": 0, "info": 1, "warn": 2, "error": 3}
                allowed = [k for k, v in lv.items() if v >= lv.get(qs["level"], 0)]
                where.append("level IN (%s)" % ",".join("?" * len(allowed)))
                args += allowed
            if qs.get("before"):
                where.append("id<?")
                args.append(int(qs["before"]))
            if qs.get("q"):
                where.append("msg LIKE ?")
                args.append("%" + qs["q"] + "%")
            lim = min(1000, int(qs.get("limit", 200)))
            return bot.store.query("SELECT id,ts,level,kind,msg,extra FROM ops_log WHERE %s ORDER BY id DESC LIMIT %d"
                                   % (" AND ".join(where), lim), args)

        def api_items(self, qs):
            ids = resolve_items(bot, qs.get("q"), int(qs.get("limit", 30)))
            return [{"item_id": i, "name": bot.item_name(i), "kind": bot.item_meta(i).get("kind"),
                     "base_price": bot.base_price(i)} for i in ids]

        def api_watch(self, qs):
            rows = bot.store.query("SELECT item_id, COUNT(DISTINCT ts) scans, MAX(ts) last FROM listing_obs GROUP BY item_id")
            dl = {r["item_id"]: r for r in bot.store.query("SELECT item_id, COUNT(*) n, SUM(count) v FROM deals GROUP BY item_id")}
            ids = sorted(set(bot.watch_ids) | {r["item_id"] for r in rows} | set(dl))
            m = {r["item_id"]: r for r in rows}
            return [{"item_id": i, "name": bot.item_name(i), "base_price": bot.base_price(i),
                     "seller": i in bot.seller_items, "deals": (dl.get(i) or {}).get("n", 0),
                     "last_obs": (m.get(i) or {}).get("last")} for i in ids]

        def api_market(self, qs):
            iid = int(qs["item_id"])
            r = market.series(bot.store, iid, int(qs.get("bucket", 3600)), float(qs.get("days", 7)))
            r["name"] = bot.item_name(iid)
            r["base_price"] = bot.base_price(iid)
            r["ref"] = bot.ref.get(iid)
            st = bot.store.kv_get("fair:%d" % iid)
            if st and r["base_price"]:
                r["fair_value"] = st["base"] * __import__("math").exp(st["x"])
            return r

        def api_bot_listings(self, qs):
            st = qs.get("status", "active")
            rows = bot.store.query("SELECT * FROM bot_listings WHERE (?='all' OR status=?) ORDER BY listed_at DESC LIMIT 500",
                                   (st, st))
            for r in rows:
                r["name"] = bot.item_name(r["item_id"])
            return rows

        def api_deals(self, qs):
            args = []
            w = "1=1"
            if qs.get("item_id"):
                w = "item_id=?"
                args.append(int(qs["item_id"]))
            rows = bot.store.query("SELECT * FROM deals WHERE %s ORDER BY ts DESC LIMIT %d" % (w, min(1000, int(qs.get("limit", 200)))), args)
            for r in rows:
                r["name"] = bot.item_name(r["item_id"])
            return rows

        def api_search(self, qs):
            q = (qs.get("q") or "").strip()
            mode = qs.get("mode", "live")
            mine = {r["auction_id"]: r for r in bot.store.query("SELECT auction_id FROM bot_listings")}
            npc = bot.store.kv_get("npc_name")
            if q.startswith("#") and q[1:].isdigit():          # 按拍卖号
                rows = bot.db.auction_main("auction_id=%d" % int(q[1:]))
                return {"mode": "db", "ids": [], "rows": [self._dbrow(r, mine) for r in rows]}
            ids = resolve_items(bot, q, 20) if q else []
            if mode == "db" or not ids:
                where = ("item_id IN (%s)" % ",".join(map(str, ids))) if ids else "1=1"
                rows = bot.db.auction_main(where + " ORDER BY item_id, unit_price LIMIT 1000")
                return {"mode": "db", "ids": ids, "rows": [self._dbrow(r, mine) for r in rows]}
            t0 = time.time()
            ls = bot.call(lambda: bot.client.search_items(ids), timeout=60)
            rows = []
            for l in ls:
                cnt = l["item"]["add_info"]
                rows.append({"auction_id": l["auction_id"], "item_id": l["item_id"], "name": bot.item_name(l["item_id"]),
                             "count": cnt, "price": l["price"], "instant_price": l["instant_price"],
                             "unit_price": l["unit_price"], "average_price": l["average_price"],
                             "owner_name": l["owner_name"], "expire_hours": l["expire_hours"],
                             "upgrade": l["item"].get("upgrade"),
                             "is_bot": l["auction_id"] in mine or (npc is not None and l["owner_name"] == npc)})
            rows.sort(key=lambda r: (r["item_id"], r["unit_price"] or r["instant_price"]))
            return {"mode": "live", "ids": ids, "names": {i: bot.item_name(i) for i in ids}, "rows": rows,
                    "elapsed": round(time.time() - t0, 3)}

        def _dbrow(self, r, mine):
            return {"auction_id": r["auction_id"], "item_id": r["item_id"], "name": bot.item_name(r["item_id"]),
                    "count": r["add_info"], "price": r["price"], "instant_price": r["instant_price"],
                    "unit_price": r["unit_price"], "owner_name": r["owner_name"], "owner_id": r["owner_id"],
                    "expire_time": r["expire_time"], "upgrade": r["upgrade"], "buyer_id": r["buyer_id"],
                    "is_bot": r["owner_id"] == -1}

        def api_orphans(self, qs):
            return bot.orphan_preview()

        def api_config(self, qs):
            with open(cfg_path, encoding="utf-8") as f:
                txt = f.read()
            txt = re.sub(r"(password:\s*).*", r"\1\"******\"", txt)
            return {"path": cfg_path, "text": txt}

        # ---------- POST ----------
        def post_control(self, body):
            a = body.get("action")
            if a == "pause":
                bot.paused = True
                bot.log("control", "网页操作：暂停自动上架/收购")
            elif a == "resume":
                bot.paused = False
                bot.log("control", "网页操作：恢复自动上架/收购")
            elif a == "restock_now":
                bot.last["restock"] = 0
                bot.log("control", "网页操作：立即补货")
            elif a == "scan_now":
                bot.last["scan"] = 0
                bot.last["history"] = 0
            elif a == "reload":
                from .main import load_config
                bot.reload_config(load_config(cfg_path))
                bot._check_buyer_id()
                bot.log("control", "网页操作：已重新加载配置")
            else:
                return {"error": "未知操作"}
            return {"ok": True, "paused": bot.paused}

        def post_orphans_cleanup(self, body):
            return bot.orphan_cleanup_run()

    return H


def start_web(bot, bind, port, web_dir, cfg_path):
    srv = ThreadingHTTPServer((bind, port), make_handler(bot, web_dir, cfg_path))
    srv.daemon_threads = True
    t = threading.Thread(target=srv.serve_forever, name="web", daemon=True)
    t.start()
    return srv
