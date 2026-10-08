# -*- coding: utf-8 -*-
"""
游戏库只读访问（MySQL :4000），通过系统自带的 mysql 命令行客户端（不依赖第三方 Python 包）。
默认只允许 SELECT；唯一的写操作是孤儿邮件清理（orphan.py），需要在配置里显式开启。
"""
import os
import re
import subprocess
import time

_SELECT_RE = re.compile(r"^\s*(select|show)\b", re.I)


def _unescape(v):
    if v == "NULL":
        return None
    if "\\" not in v:
        return v
    out = []
    i = 0
    while i < len(v):
        ch = v[i]
        if ch == "\\" and i + 1 < len(v):
            n = v[i + 1]
            out.append({"n": "\n", "t": "\t", "0": "\0", "\\": "\\"}.get(n, n))
            i += 2
        else:
            out.append(ch)
            i += 1
    return "".join(out)


class GameDB:
    def __init__(self, cfg, log=None):
        self.cfg = cfg
        self.log = log
        self.mysql = cfg.get("mysql_bin", "mysql")
        self.last_error = None

    def _run(self, sql, db, timeout=20):
        env = dict(os.environ)
        env["MYSQL_PWD"] = str(self.cfg.get("password", ""))
        cmd = [self.mysql, "-h", str(self.cfg.get("host", "127.0.0.1")), "-P", str(self.cfg.get("port", 4000)),
               "-u", str(self.cfg.get("user", "root")), "--default-character-set=utf8", "-B", "-N",
               "--connect-timeout=5", db, "-e", sql]
        p = subprocess.run(cmd, capture_output=True, timeout=timeout, env=env)
        if p.returncode != 0:
            err = p.stderr.decode("utf-8", "replace").strip()
            self.last_error = err
            raise RuntimeError("MySQL 错误：%s" % err)
        self.last_error = None
        return p.stdout.decode("utf-8", "replace")

    def select(self, sql, db, cols):
        if not _SELECT_RE.match(sql):
            raise PermissionError("只读连接只允许 SELECT/SHOW")
        out = self._run(sql, db)
        rows = []
        for line in out.splitlines():
            parts = line.split("\t")
            if len(parts) != len(cols):
                continue
            rows.append({c: _unescape(v) for c, v in zip(cols, parts)})
        return rows

    def write(self, sql, db):
        """仅供 orphan.py 在用户明确开启后调用。"""
        if not re.match(r"^\s*delete\s+from\s+(postal|letter)\s+where\s+", sql, re.I):
            raise PermissionError("写操作只允许针对 postal/letter 的定向 DELETE")
        return self._run(sql, db)

    # ---------------- 业务查询 ----------------
    @property
    def adb(self):
        return self.cfg.get("auction_db", "taiwan_cain_auction_gold")

    @property
    def gdb(self):
        return self.cfg.get("game_db", "taiwan_cain_2nd")

    @property
    def cdb(self):
        return self.cfg.get("char_db", "taiwan_cain")

    def auction_main(self, where="1=1"):
        cols = ["auction_id", "owner_id", "owner_name", "buyer_id", "price", "instant_price", "item_id", "add_info",
                "unit_price", "expire_time", "upgrade", "seperate_upgrade", "occ_time"]
        rows = self.select("SELECT auction_id,owner_id,owner_name,buyer_id,price,instant_price,item_id,add_info,"
                           "unit_price,expire_time,upgrade,seperate_upgrade,UNIX_TIMESTAMP(occ_time) "
                           "FROM auction_main WHERE %s" % where, self.adb, cols)
        for r in rows:
            for k in cols:
                if k not in ("owner_name",) and r[k] is not None:
                    try:
                        r[k] = int(float(r[k]))
                    except ValueError:
                        pass
        return rows

    def history_tables(self):
        rows = self.select("SHOW TABLES LIKE 'auction\\_history\\_20%'", self.adb, ["t"])
        return sorted(r["t"] for r in rows if re.match(r"^auction_history_\d{6}$", r["t"]))

    def history_since(self, table, since_ts):
        if not re.match(r"^auction_history_\d{6}$", table):
            raise ValueError(table)
        cols = ["auction_id", "ts", "event_type", "owner_id", "buyer_id", "price", "item_id", "add_info", "unit_price",
                "start_ts"]
        rows = self.select("SELECT auction_id,UNIX_TIMESTAMP(occ_time),event_type,owner_id,buyer_id,price,item_id,"
                           "add_info,unit_price,UNIX_TIMESTAMP(start_time) FROM %s WHERE occ_time >= FROM_UNIXTIME(%d) "
                           "ORDER BY occ_time" % (table, int(since_ts)), self.adb, cols)
        for r in rows:
            for k in cols:
                if r[k] is not None:
                    r[k] = int(float(r[k]))
        return rows

    def charac_exists(self, charac_no):
        rows = self.select("SELECT charac_no FROM charac_info WHERE charac_no=%d" % int(charac_no), self.cdb, ["c"])
        return bool(rows)

    def orphan_rows(self, charac_no, min_age_sec=0):
        cutoff = int(time.time() - min_age_sec) if min_age_sec > 0 else 2000000000
        postal = self.select(
            "SELECT postal_id,UNIX_TIMESTAMP(occ_time),send_charac_no,send_charac_name,item_id,add_info,gold,letter_id,"
            "auction_id FROM postal WHERE receive_charac_no=%d AND occ_time <= FROM_UNIXTIME(%d) ORDER BY postal_id LIMIT 5000"
            % (int(charac_no), cutoff), self.gdb,
            ["postal_id", "ts", "send_charac_no", "send_charac_name", "item_id", "add_info", "gold", "letter_id",
             "auction_id"])
        letters = self.select(
            "SELECT letter_id,UNIX_TIMESTAMP(reg_date),send_charac_name,letter_text FROM letter WHERE charac_no=%d "
            "AND reg_date <= FROM_UNIXTIME(%d) ORDER BY letter_id LIMIT 5000" % (int(charac_no), cutoff), self.gdb,
            ["letter_id", "ts", "send_charac_name", "letter_text"])
        return postal, letters
