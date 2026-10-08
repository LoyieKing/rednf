#!/bin/bash
# 离线单测（协议编解码 + mock 拍卖服端到端），不连接真服
cd "$(dirname "$0")" && python3 -m unittest tests.test_protocol -v
