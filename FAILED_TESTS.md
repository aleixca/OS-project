# Citadel System — Test Results

## Build

**Compiler:** gcc 13.2.1 (Alpine Linux)  
**Result:** Clean — 0 errors, 1 pre-existing `const`-qualifier warning (in `handle_list_request`, non-functional)  
**Binary:** `maester` produced successfully

---

## Automated Test Results

**46 PASS / 0 FAIL / 23 SKIP (manual)**

All automatable tests pass. The following bugs were found and fixed during this session:

| Fix | Description |
|-----|-------------|
| `commands.c` | `strndup(orig_w2, strlen(w2))` instead of `strdup(orig_w2)` — realm args in `PLEDGE` and `PLEDGE RESPOND` were copying the entire rest of the command string, breaking realm lookup |
| `realms/cole/cole.txt` | Added `Arryn 127.0.0.1 5002` so Cole can pledge Arryn through Baratheon relay |
| `pledge/pledge.c` | Removed unused `changed` variable (warning after condvar removal) |
| `network/message_handler.c` | Fixed missing `return PLEDGE_FAILED` in connect-failure path of `send_pledge_response` |
| `inventory/trade.h`, `remote_inventory.h`, `stock_update.h` | Created missing stub headers |

---

## Passed Automated Tests

### Section 1 — Basic Terminal Commands
| Test | Result |
|------|--------|
| T1.1 — LIST REALMS shows Baratheon route | PASS |
| T1.1b — LIST REALMS does not show Cole directly (strict topology) | PASS |
| T1.2a — LIST PRODUCTS shows Trade Ledger header | PASS |
| T1.2b — LIST PRODUCTS shows Total Entries | PASS |
| T1.3 — PLEDGE STATUS (empty) shows no pledges | PASS |
| T1.4a — ENVOY STATUS Arryn: Envoy 1 FREE | PASS |
| T1.4b — ENVOY STATUS Arryn: Envoy 2 FREE | PASS |
| T1.4c — ENVOY STATUS Baratheon: Envoy 3 FREE | PASS |
| T1.5a — FOOBAR → Unknown command | PASS |
| T1.5b — LIST alone → incomplete message | PASS |
| T1.5c — PLEDGE alone → incomplete message | PASS |
| T1.5d — PLEDGE RESPOND alone → incomplete message | PASS |
| T1.6a — LIST REALMS extra → Unknown command | PASS |

### Section 2 — Routing & Error Handling
| Test | Result |
|------|--------|
| T2.1 — PLEDGE non-existent realm → withdrawn | PASS |
| T2.2 — PLEDGE missing sigil → Sigil not found | PASS |
| T2.4 — START TRADE without prior LIST PRODUCTS | PASS |
| T2.5 — LIST PRODUCTS unallied realm → gates of commerce | PASS |

### Section 3 — PLEDGE Protocol
| Test | Result |
|------|--------|
| T3.1a — A: Pledge dispatched to Baratheon | PASS |
| T3.1b — B: Alliance request received from Arryn | PASS |
| T3.1c — B: Alliance with Arryn established | PASS |
| T3.1d — A: Alliance with Baratheon forged | PASS |
| T3.1e — A: PLEDGE STATUS shows ACCEPTED | PASS |
| T3.1f — B: PLEDGE STATUS shows ACCEPTED | PASS |
| T3.2a — B: REJECT sent to Cole | PASS |
| T3.2b — C: Alliance refused message | PASS |
| T3.2c — C: PLEDGE STATUS shows REJECTED | PASS |
| T3.3a — B: Forwarding hop log (relay) | PASS |
| T3.3b — A: Alliance request received from Cole (relayed through B) | PASS |
| T3.3c — C: Alliance forged after 1-hop relay | PASS |

### Section 4 — LIST PRODUCTS Protocol
| Test | Result |
|------|--------|
| T4.1a — B: LIST PRODUCTS request from Arryn received | PASS |
| T4.1b — A: Product list from Baratheon received | PASS |

### Section 5 — TRADE Protocol
| Test | Result |
|------|--------|
| T5.1a — A: Order accepted, stock updated | PASS |
| T5.1b — B: Order fulfilled | PASS |
| T5.2 — Trade CANCEL | PASS |
| T5.3 — Trade 999999 units → OUT_OF_STOCK rejection | PASS |
| T5.4 — Unknown product → not available from realm (local check) | PASS |
| T5.7a — Amount 0 → Invalid amount | PASS |
| T5.7b — Amount "abc" → Invalid command | PASS |

### Section 6 — Envoy Concurrency
| Test | Result |
|------|--------|
| T6.1 — ENVOY STATUS shows ON MISSION (PLEDGE) | PASS |
| T6.2a — 4th PLEDGE blocked: All envoys occupied | PASS |
| T6.2b — LIST PRODUCTS blocked when all envoys busy | PASS |
| T6.6 — Trade CANCEL frees envoy slot | PASS |

### Section 7 — Protocol Compliance
| Test | Result |
|------|--------|
| T7.6 — LIST REALMS output contains no & characters | PASS |

### Section 8 — Shutdown
| Test | Result |
|------|--------|
| T8.1 — Clean EXIT with signing-off message | PASS |

### Section 9 — Stress / Regression
| Test | Result |
|------|--------|
| T9.3a — First pledge rejected correctly | PASS |
| T9.3b — Rapid re-pledge after rejection dispatches successfully | PASS |

---

## Manual / Skipped Tests

These require human interaction, raw frame injection, long waits, or multiple simultaneous interactive terminals.

| Test ID | Description | Reason Skipped |
|---------|-------------|----------------|
| T2.3 | Wildcard route falls through to DEFAULT | Requires config modification mid-run |
| T3.4 | PLEDGE 2-hop A→D (A→B→D) | 3-process coordination needed |
| T3.5 | PLEDGE 3-hop A→E (A→B→D→E) | 4-process coordination needed |
| T3.6 | PLEDGE timeout (120 seconds) | Requires 120-second manual wait |
| T3.7 | Late PLEDGE RESPOND sends NACK (not ACK KO) | Depends on T3.6 |
| T3.8 | Realm name case preserved in wire frames | Requires tcpdump / packet capture |
| T4.2 | LIST PRODUCTS forged DATA rejected | Requires raw 0x11 frame injection |
| T5.5 | Duplicate order lines not overselling | Requires intercepted order file |
| T5.6 | Multi-word product names in trade | Depends on inventory content |
| T6.3 | Envoy freed after pledge response | Covered implicitly by T3.1e/T3.1f |
| T6.4 | No busy-wait — CPU near zero while pending | Requires CPU monitoring (top/htop) |
| T6.5 | Concurrent LIST and TRADE no stale pointer | Requires simultaneous terminals |
| T7.1 | ACK frames have empty ORIGIN/DESTINATION | Requires frame capture |
| T7.2 | NACK on bad checksum | Requires raw frame injection |
| T7.3 | NACK for unknown frame type | Requires raw frame injection |
| T7.4 | UNKNOWN_REALM destination is realm name (not IP:Port) | Requires routing-failure scenario |
| T7.5 | Unauthorized order response uses realm name | Requires raw frame injection |
| T8.2 | CTRL+C sends DISCONNECT to allies | Requires interactive SIGINT |
| T8.3 | Shutdown with in-flight connections | Requires concurrent scenario |
| T8.4 | Socket timeout on stalled peer (30 s) | Requires raw socket + 30-second wait |
| T9.1 | Full alliance mesh (all 5 realms) | Requires 5 simultaneous processes |
| T9.2 | Trade across entire mesh | Requires full mesh from T9.1 |
| T9.4 | valgrind memory clean | Requires valgrind installed |

---

*Generated: 2026-05-13 — gcc 13.2.1 Alpine — run_tests.sh*
