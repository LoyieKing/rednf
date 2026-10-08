# -*- coding: utf-8 -*-
"""
asyncio 拍卖服客户端：伪装成一个游戏频道连接 df_auction_r（30803）。

握手：REGIST_GA(msg 0) -> 等 AG REGIST_ACK -> CHECK_AUCTION_READY_GA(msg 13) -> 等 db_work_done=1。
  * 每个连接只发一次 REGIST（重复 REGIST 会在拍卖服残留一个永不注销的 Character）。
  * 拍卖服不做空闲断线（ORIG 无 getLastAccessTime 调用者），也不给被动连接发心跳，这里不需要保活。
请求严格串行（同一时刻只有一个在途请求），带超时：拍卖服对检索参数错误等情况会直接不回包。
char_idx 字段原样回显，用作请求序号，过期回包按序号丢弃。
"""
import asyncio
import itertools
import time

from . import protocol as P


class AuctionClient:
    def __init__(self, host, port, log, request_timeout=8.0, min_interval=0.15):
        self.host = host
        self.port = port
        self.log = log                      # callable(kind, msg, level="info", **extra)
        self.request_timeout = request_timeout
        self.min_interval = min_interval
        self.reader = None
        self.writer = None
        self.connected = False
        self.ready = False
        self.service_available = True
        self.regist_ack = None
        self.connected_at = None
        self.last_rx = None
        self.last_tx = None
        self.stats = {"tx": 0, "rx": 0, "timeouts": 0, "reconnects": 0}
        self._lock = asyncio.Lock()
        self._seq = itertools.count(int(time.time()) % 100000 * 1000 + 1)
        self._pending = None                # (char_idx or key, expected types, future, collected list)
        self._rx_task = None
        self._last_send = 0.0
        self._ready_event = asyncio.Event()
        self._ctrl_waiter = None

    # ---------------- 连接 ----------------
    async def connect(self):
        self.log("connect", "连接拍卖服 %s:%d ..." % (self.host, self.port))
        self.reader, self.writer = await asyncio.wait_for(asyncio.open_connection(self.host, self.port), 10)
        self.connected = True
        self.connected_at = time.time()
        self.ready = False
        self._ready_event.clear()
        self._rx_task = asyncio.create_task(self._rx_loop())
        # 1) REGIST（只发一次）
        fut = asyncio.get_running_loop().create_future()
        self._ctrl_waiter = ("REGIST_ACK", fut)
        await self._send(P.build_regist(0))
        ack = await asyncio.wait_for(fut, 10)
        self.regist_ack = ack["result"]
        txt = {0: "成功", 1: "key 冲突，拍卖服已改用备用 key（正常）", 2: "拍卖服尚未就绪"}.get(ack["result"], str(ack["result"]))
        self.log("connect", "注册为频道：REGIST_ACK=%d（%s）" % (ack["result"], txt))
        # 2) CHECK_AUCTION_READY，直到 db_work_done
        for i in range(30):
            fut = asyncio.get_running_loop().create_future()
            self._ctrl_waiter = ("CHECK_AUCTION_READY", fut)
            await self._send(P.build_check_ready())
            r = await asyncio.wait_for(fut, 10)
            if r["ready"]:
                self.ready = True
                self.service_available = True
                self._ready_event.set()
                self.log("connect", "拍卖服就绪（CHECK_AUCTION_READY: db_work_done=1）")
                return True
            await asyncio.sleep(5)
        raise ConnectionError("拍卖服长时间未就绪")

    async def close(self):
        self.connected = False
        self.ready = False
        if self._rx_task:
            self._rx_task.cancel()
        if self.writer:
            try:
                self.writer.close()
                await asyncio.wait_for(self.writer.wait_closed(), 3)
            except Exception:
                pass
        self.writer = None
        self._fail_pending(ConnectionError("连接已关闭"))

    def _fail_pending(self, exc):
        if self._pending and not self._pending[2].done():
            self._pending[2].set_exception(exc)
        self._pending = None
        if self._ctrl_waiter and not self._ctrl_waiter[1].done():
            self._ctrl_waiter[1].set_exception(exc)
        self._ctrl_waiter = None

    async def _send(self, pkt):
        h = P.parse_header(pkt)
        if h["size"] != len(pkt) or h["size"] < P.HEADER_SIZE:
            raise P.ProtocolError("拒绝发送长度不符的包")
        if h["cat"] == P.CAT_GA and h["msg_no"] not in P.GA_SIZES:
            raise P.ProtocolError("拒绝发送未知 msg_no=%d" % h["msg_no"])
        wait = self.min_interval - (time.monotonic() - self._last_send)
        if wait > 0:
            await asyncio.sleep(wait)
        self.writer.write(pkt)
        await self.writer.drain()
        self._last_send = time.monotonic()
        self.last_tx = time.time()
        self.stats["tx"] += 1

    async def _rx_loop(self):
        fr = P.FrameReader()
        try:
            while True:
                data = await self.reader.read(65536)
                if not data:
                    raise ConnectionError("拍卖服关闭了连接")
                self.last_rx = time.time()
                for pkt in fr.feed(data):
                    self.stats["rx"] += 1
                    try:
                        msg = P.parse_ag(pkt)
                    except Exception as e:  # 解析失败只记日志
                        self.log("error", "回包解析失败：%s" % e, level="error")
                        continue
                    self._dispatch(msg)
        except asyncio.CancelledError:
            return
        except Exception as e:
            if self.connected:
                self.log("connect", "连接断开：%s" % e, level="warn")
            self.connected = False
            self.ready = False
            self._fail_pending(ConnectionError(str(e)))

    def _dispatch(self, msg):
        t = msg["type"]
        if t == "HEARTBEAT":
            return
        if t == "SERVICE_UNAVAILABLE":
            self.service_available = False
            self.log("connect", "拍卖服通知：服务不可用（可能正在停服），暂停操作", level="warn")
            return
        if t == "SERVICE_AVAILABLE":
            self.service_available = True
            self.log("connect", "拍卖服通知：服务恢复可用")
            return
        if self._ctrl_waiter and self._ctrl_waiter[0] == t:
            w = self._ctrl_waiter
            self._ctrl_waiter = None
            if not w[1].done():
                w[1].set_result(msg)
            return
        p = self._pending
        if p and msg.get("char_idx") == p[0] and t in p[1]:
            p[3].append(msg)
            if t == p[1][-1]:           # 最后一个期望包到达
                self._pending = None
                if not p[2].done():
                    p[2].set_result(p[3])
            return
        self.log("protocol", "丢弃未匹配的回包 %s char_idx=%s" % (t, msg.get("char_idx")), level="debug")

    async def request(self, build, expect, timeout=None):
        """build(char_idx)->bytes；expect: 期望回包类型序列（最后一个到达即完成）。返回回包列表。"""
        if not (self.connected and self.ready):
            raise ConnectionError("未连接或拍卖服未就绪")
        if not self.service_available:
            raise ConnectionError("拍卖服通知服务不可用")
        async with self._lock:
            idx = next(self._seq) & 0x7FFFFFFF
            pkt = build(idx)                       # 先构包（本地校验失败直接抛出，不发送）
            fut = asyncio.get_running_loop().create_future()
            self._pending = (idx, tuple(expect), fut, [])
            try:
                await self._send(pkt)
                return await asyncio.wait_for(fut, timeout or self.request_timeout)
            except asyncio.TimeoutError:
                self.stats["timeouts"] += 1
                raise
            finally:
                if self._pending and self._pending[2] is fut:
                    self._pending = None
                if not fut.done():
                    fut.cancel()

    # ---------------- 业务请求 ----------------
    async def probe_item(self, item_id):
        """ASK_REGISTED_ITEM_NUM：-2 表示物品不在拍卖服 iteminfo.dat。只读、无副作用。"""
        r = await self.request(lambda i: P.build_ask_registed_item_num(i, -1, item_id), ["ASK_REGISTED_ITEM_NUM"])
        return r[-1]["now_registed_num"]

    async def probe_item_info(self, item_id):
        """同 probe_item，但同时返回回包里的 item_id：未知物品回 item_id=0 或挂单数 -2。只读。"""
        r = await self.request(lambda i: P.build_ask_registed_item_num(i, -1, item_id), ["ASK_REGISTED_ITEM_NUM"])
        m = r[-1]
        return {"item_id": m["item_id"], "num": m["now_registed_num"],
                "missing": m["item_id"] == 0 or m["now_registed_num"] == -2}

    async def search_items(self, item_ids, max_pages=20):
        """按道具 ID 检索当前全部挂单（真人+系统），自动翻页。"""
        ids = list(dict.fromkeys(int(x) for x in item_ids))
        results = []
        for k in range(0, len(ids), P.MAX_ITEM_ID_PER_SEARCH):
            chunk = ids[k: k + P.MAX_ITEM_ID_PER_SEARCH]
            start = 0
            total = None
            for _ in range(max_pages):
                r = await self.request(lambda i, s=start, c=chunk: P.build_search_by_itemkey(i, 0, c, start_index=s),
                                       ["ITEM_LIST"])
                m = r[-1]
                if total is None:
                    total = m["total"]
                results.extend(m["items"])
                start += len(m["items"])
                if len(m["items"]) < P.SEARCH_PAGE or start >= total:
                    break
        return results

    async def regist_item(self, owner_id, owner_name, item_id, count, unit_price, price=-1):
        instant = unit_price * count
        r = await self.request(
            lambda i: P.build_regist_item(i, owner_id, owner_name, item_id, count, price, instant, unit_price),
            ["LOG_MESSAGE", "REGIST_RESULT"])
        log = next((m for m in r if m["type"] == "LOG_MESSAGE"), None)
        res = r[-1]
        return {"ok": res["ok"], "because": res.get("because"), "because_text": res.get("because_text"),
                "auction_id": log["auction_id"] if log else 0, "instant_price": instant}

    async def buy_apiece(self, buyer_id, auction_id, unit_price, count):
        price = unit_price * count
        r = await self.request(lambda i: P.build_buy_item_apiece(i, buyer_id, price, auction_id, count),
                               ["LOG_MESSAGE", "BUY_ITEM_APIECE"])
        return r[-1]

    async def buy_now(self, buyer_id, buyer_name, auction_id, instant_price):
        r = await self.request(lambda i: P.build_bidding(i, buyer_id, buyer_name, instant_price, auction_id),
                               ["LOG_MESSAGE", "BIDDING_RESULT"])
        return r[-1]

    async def my_registed(self, owner_id):
        r = await self.request(lambda i: P.build_my_registed_item_info(i, owner_id), ["MY_REGISTED_ITEM_INFO"])
        return r[-1]["items"]
