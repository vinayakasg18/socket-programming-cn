#!/bin/bash

LAT="25ms"
IFACE="lo"
DMASK=0xff00

function _add() {
  local ID=$1
  local LOSS=$2
  local DPORT=$3
  sudo tc class add dev ${IFACE} parent 1:1 classid 1:${ID} htb rate 10000Mbps
  sudo tc qdisc add dev ${IFACE} parent 1:${ID} handle ${ID}0: netem delay ${LAT} loss ${LOSS} limit 10000
  sudo tc filter add dev ${IFACE} parent 1:0 protocol ip u32 match ip dport ${DPORT} ${DMASK} flowid 1:${ID}
}

function _add_all() {
  sudo tc qdisc add dev ${IFACE} root handle 1: htb
  sudo tc class add dev ${IFACE} parent 1: classid 1:1 htb rate 10000Mbps

  # emulate latency and loss on 3 port ranges
  _add 2 0 2048
  _add 3 1 4096
  _add 4 30 8192
}

function _del() {
  sudo tc qdisc del dev ${IFACE} root
}

function _show() {
  tc -graph -statistics class show dev ${IFACE}
  tc qdisc show dev ${IFACE}
  tc -statistics filter show dev lo
}

function _usage() {
  echo "Usage: $0 [show|add|del]"
}

if [ $# -eq 0 ]; then
  _usage
  exit 0
fi

case $1 in
  "show") _show    ;;
   "add") _add_all ;;
   "del") _del     ;;
   *)     _usage   ;;
esac
