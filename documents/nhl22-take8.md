<!-- SPDX-FileCopyrightText: 2026 shadPS4 Emulator Project -->
<!-- SPDX-License-Identifier: GPL-2.0-or-later -->

# NHL 22 Take 8: front-end transition trace

Base: `nhl22-take7` at `ddece869398cee9a50023d52e5a386430cfec3fb`.

Take 7 showed the Press X prompt for the first time. Pressing X removes it,
but the menu never appears. This proves the controller event reaches the game;
it does not prove that GPU work, front-end assets, or the next guest event
completes. The Test 7 log contains no fatal GPU error and ends shortly after
the press-start loose-file probe and shader compilation.

The log contains many missing `/temp0/nhl/ui/*.lreq` loose paths, including
`pressstart.lreq` and `feroot.lreq`, and a missing `start_alpha.gnf`. It also
loads packed CAS data and successfully renders the prompt. Treat those path
misses as probes until a pack lookup or a failed front-end load is demonstrated.

## New trace points

- Log Cross and Options button edges delivered by `scePadRead`.
- Log the first and every 60th submitted and completed video flip.
- Log the first and every 60th GfxEopQueue wait and its completion.
- Log the first and every 60th queued and signaled EOP fence.
- Log game-requested HDR mode and the resulting swapchain mode.

These low-volume markers distinguish four failure modes after X: guest event
wait, GPU fence starvation, video presentation stall, or a front end that keeps
rendering without reaching the menu. They also show whether NHL requests PQ HDR
while the host exposes it.

## Next experiments after a Take 8 log

1. If EOP queued rises but signaled stops, isolate scheduler priority operations
   and GPU fence completion. Try a guarded diagnostic fallback in a separate
   build, never silently fake completion in the baseline.
2. If flips and EOP continue, trace the CAS/UI load and any guest front-end
   thread wait. Compare a separate real occlusion-query build against the
   invented pixel counter; the Battlefield fork demonstrated this renderer gap.
3. If submitted flips rise but completed flips stop, capture Vulkan device-fault
   data and staging/copy bounds from the Battlefield work.
4. For the oversaturated NVIDIA overlay and dim game, compare an SDR run with
   the same settings except HDR disabled. Keep the menu investigation separate
   from the color-space result. The trace records the game colorimetry and
   whether the swapchain switches to Rec.2020 PQ.

User observation: Take 7 Press X vanishes after the Cross press; the menu stays
on the same screen. HDR colors are also visibly wrong.
