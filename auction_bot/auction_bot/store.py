# -*- coding: utf-8 -*-
"""机器人自己的 SQLite 持久化（data/bot.db），不碰游戏库。"""
import json
import sqlite3
import threading
import time

SCHEMA = """
CREATE TABLE IF NOT EXISTS ops_log(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  ts REAL NOT NULL, level TEXT NOT NULL, kind TEXT NOT NULL, msg TEXT NOT NULL, extra TEXT);
CREATE INDEX IF NOT EXISTS ix_ops_ts ON ops_log(ts);
CREATE INDEX IF NOT EXISTS ix_ops_kind ON ops_log(kind);

-- 机器人（owner -1）挂单
CREATE TABLE IF NOT EXISTS bot_listings(
  auction_id INTEGER PRIMARY KEY, item_id INTEGER NOT NULL, count0 INTEGER NOT NULL, count_now INTEGER NOT NULL,
  unit_price INTEGER NOT NULL, instant_price INTEGER NOT NULL, listed_at REAL NOT NULL, expire_at REAL NOT NULL,
  status TEXT NOT NULL DEFAULT 'active', closed_at REAL, last_seen REAL, note TEXT);
CREATE INDEX IF NOT EXISTS ix_bl_status ON bot_listings(status, item_id);

-- 成交：side = human_buy_bot（玩家买了系统货）/ bot_buy_human（机器人收购玩家货）/ human_human（玩家间）
CREATE TABLE IF NOT EXISTS deals(
  id INTEGER PRIMARY KEY AUTOINCREMENT, ts REAL NOT NULL, item_id INTEGER NOT NULL, auction_id INTEGER,
  count INTEGER NOT NULL, unit_price INTEGER NOT NULL, total INTEGER NOT NULL, side TEXT NOT NULL,
  seller_id INTEGER, buyer_id INTEGER, source TEXT NOT NULL, uniq TEXT UNIQUE);
CREATE INDEX IF NOT EXISTS ix_deals_item_ts ON deals(item_id, ts);

-- 行情快照：每次扫描的每条挂单（真人+系统），用于挂单价走势/分位数
CREATE TABLE IF NOT EXISTS listing_obs(
  ts REAL NOT NULL, item_id INTEGER NOT NULL, auction_id INTEGER NOT NULL, owner TEXT, is_bot INTEGER NOT NULL,
  unit_price INTEGER NOT NULL, count INTEGER NOT NULL);
CREATE INDEX IF NOT EXISTS ix_obs_item_ts ON listing_obs(item_id, ts);

CREATE TABLE IF NOT EXISTS kv(k TEXT PRIMARY KEY, v TEXT);

-- 经济模型：每一步的汇总、每个道具的状态快照
CREATE TABLE IF NOT EXISTS engine_steps(
  ts REAL NOT NULL, dt_h REAL, online INTEGER, items INTEGER, new_planned INTEGER, new_done INTEGER,
  vf_planned INTEGER, vf_done INTEGER, uf_planned INTEGER, uf_done INTEGER, user_gold INTEGER, errors INTEGER, elapsed REAL);
CREATE TABLE IF NOT EXISTS engine_snap(
  ts REAL NOT NULL, item_id INTEGER NOT NULL, fair REAL, stock REAL, scarcity REAL, x REAL, n_sys INTEGER,
  new INTEGER, vqty INTEGER, uqty INTEGER);
CREATE INDEX IF NOT EXISTS ix_esnap_item_ts ON engine_snap(item_id, ts);
"""


class Store:
    def __init__(self, path):
        self.path = path
        self._local = threading.local()
        self._wlock = threading.Lock()
        c = self.conn()
        c.executescript(SCHEMA)
        cols = {r[1] for r in c.execute("PRAGMA table_info(bot_listings)")}
        if "owner_id" not in cols:           # 2026-10-11：机器人卖家 -114514；旧行（NULL）= 系统卖家 -1
            c.execute("ALTER TABLE bot_listings ADD COLUMN owner_id INTEGER")
        c.commit()

    def conn(self):
        c = getattr(self._local, "c", None)
        if c is None:
            c = sqlite3.connect(self.path, timeout=30, check_same_thread=False)
            c.row_factory = sqlite3.Row
            c.execute("PRAGMA journal_mode=WAL")
            c.execute("PRAGMA synchronous=NORMAL")
            self._local.c = c
        return c

    def execute(self, sql, args=()):
        with self._wlock:
            c = self.conn()
            cur = c.execute(sql, args)
            c.commit()
            return cur

    def executemany(self, sql, rows):
        with self._wlock:
            c = self.conn()
            c.executemany(sql, rows)
            c.commit()

    def query(self, sql, args=()):
        return [dict(r) for r in self.conn().execute(sql, args).fetchall()]

    # ---- 日志 ----
    def log(self, level, kind, msg, extra=None):
        self.execute("INSERT INTO ops_log(ts,level,kind,msg,extra) VALUES(?,?,?,?,?)",
                     (time.time(), level, kind, msg, json.dumps(extra, ensure_ascii=False) if extra else None))

    # ---- kv ----
    def kv_get(self, k, default=None):
        r = self.conn().execute("SELECT v FROM kv WHERE k=?", (k,)).fetchone()
        return json.loads(r[0]) if r else default

    def kv_set(self, k, v):
        self.execute("INSERT INTO kv(k,v) VALUES(?,?) ON CONFLICT(k) DO UPDATE SET v=excluded.v",
                     (k, json.dumps(v, ensure_ascii=False)))

    def add_deal(self, ts, item_id, auction_id, count, unit_price, total, side, seller_id, buyer_id, source, uniq):
        try:
            self.execute("INSERT INTO deals(ts,item_id,auction_id,count,unit_price,total,side,seller_id,buyer_id,source,uniq)"
                         " VALUES(?,?,?,?,?,?,?,?,?,?,?)",
                         (ts, item_id, auction_id, count, unit_price, total, side, seller_id, buyer_id, source, uniq))
            return True
        except sqlite3.IntegrityError:
            return False

    def prune(self, keep_days_obs=30, keep_days_log=60):
        now = time.time()
        self.execute("DELETE FROM listing_obs WHERE ts<?", (now - keep_days_obs * 86400,))
        self.execute("DELETE FROM ops_log WHERE ts<?", (now - keep_days_log * 86400,))
        self.execute("DELETE FROM engine_snap WHERE ts<?", (now - keep_days_obs * 86400,))
