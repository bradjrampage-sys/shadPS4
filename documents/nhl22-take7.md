# NHL 22 Take 7: start-screen compatibility investigation

Base: `nhl22-test6-modern` at `99cd0d807475cd0354a7f95d2154e84647f73bca`.
This branch retains all three Test 6 CMASK/tile patches. The reported Test 6 run
uses NHL 22 CUSA26280, app 01.30, on Windows 11 with an RTX 4070 Ti. It reaches
the background/flag start screen reproducibly, but never displays Press Start.

## Evidence from the latest Test 6 log

- Around line 15175 the game calls `sceNpSetNpTitleId(CUSA26280_00)` twice and
  registers an NP state callback. ShadNet is disabled; the calls by themselves
  do not prove a missing callback blocks the front end.
- Around line 15182 it probes `/temp0/nhl/ui/features/bootflow/vvms/bootflowlogo/bootflowlogo.lreq`.
  Around line 15342 it probes the corresponding `pressstart/pressstart.lreq`.
  Both loose paths return ENOENT. Frostbite also reads packed CAS files, so the
  loose-file misses are clues, not proof that the assets are absent.
- After the press-start probe, shader pipelines continue compiling and the game
  reads `nhl_installpackage_02/cas_04.cas`. It does not crash at that probe.
- Thousands of repetitive DirtySDK epoll create/control lines obscure the later
  signal. Test 7 moves normal epoll activity to debug level; errors remain visible.
- The log ends after a long run without a fatal device-loss report. There is no
  evidence yet that the GPU, a missing UI asset, NP, or the game thread is the
  sole blocker.

## Public code comparison

- Kravickas `test-nhl` currently points to `38c12b9f`, the same ancestor already
  incorporated in Test 6. The separate `NHL19-Build` branch is older and is not
  a suitable base for Take 7.
- The Battlefield 4 fork's upstream RFC documents a readback overrun, a deferred
  capture bug, depth format fallback, and Windows guest red-zone corruption.
  This modern Test 6 tree has a different staging pool and already captures its
  deferred image readback by value, so copying the old readback patches would
  be unsafe. The depth mapping and Windows fault mitigation can be ported.
- The NHL 24 compatibility report for an older 0.15.0 build describes a black
  screen and exit. It does not reproduce NHL 22's stable start-screen stall.

## Take 7 changes

1. Map Z16 plus stencil to D32S8 consistently through the shared format table,
   avoiding a Vulkan depth/stencil attachment format missing on some drivers.
2. On Windows, invalidate a mapped 64 KiB span for a guest write fault, falling
   back to the precise range if the span is not entirely GPU mapped. This
   reduces repeated exception delivery on the guest SysV red zone.
3. Log the host resolution of missing `.lreq` UI requests, count one video flip
   heartbeat per 600 submits, and quiet routine DirtySDK epoll traffic. The
   next run can separate active rendering from a stalled front end and check
   whether the `/temp0` mapping is unexpected.

Sources: https://github.com/shadps4-emu/shadPS4/issues/4816 ;
https://github.com/rieucdamien-boop/shadPS4 ;
https://github.com/shadps4-compatibility/shadps4-game-compatibility/issues/2500 .

## Next run

Use the Windows build from this branch with the same CUSA26280 01.30 install,
settings, and controller used in Test 6. Save the full log after reaching the
start screen and waiting at least a few minutes. Record whether the background
animates, whether the flip heartbeat continues, and whether pressing Start/X
changes the screen. Compare the `.lreq` host paths and later CAS reads. A
remaining stall needs a runtime trace from the actual game; the build alone
cannot establish that the menu is reached.
