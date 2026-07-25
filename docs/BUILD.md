# Building X12

## Dependencies (Ubuntu/Debian)

```sh
sudo apt-get install -y \
  build-essential meson ninja-build pkgconf zlib1g-dev libbz2-dev bzip2 \
  libfreetype-dev libfontconfig-dev libgbm-dev libunwind-dev libjpeg-dev \
  flex bison libinput-dev libssl-dev libncurses-dev \
  mesa-common-dev libgl1-mesa-dri libegl1-mesa-dev libxatracker-dev \
  libpixman-1-dev libsystemd-dev libdbus-1-dev cmake gettext \
  libcairo2-dev libbsd-dev libepoxy-dev libdrm-dev libpciaccess-dev \
  uuid-dev libpng-dev xkb-data x11-xkb-utils xfonts-base
```

If `pkg-config --exists bzip2` fails, add a `bzip2.pc` (see `util/install_deps.sh`).

## Configure & build

Options live in **`meson.options`** (Meson ≥ 1.1; formerly `meson_options.txt`).

```sh
meson setup build -Dprefix=$PWD/prefix
meson compile -C build
```

Important defaults for uninstalled/CI runs:

| Option | Default behavior |
|---|---|
| `xkb_data_dir` | `/usr/share/X11/xkb` when present |
| `xkb_bin_dir` | `$builddir/app/xkbcomp` (avoids system xkbcomp + in-tree libX11 mismatch) |
| `xkb_output_dir` | `/tmp/x12-xkb` |
| `build-debug` | `false` (verbose DEBUG/XNL_DEBUG breaks Xt clients) |
| `build-test` / `build-demo` / `demo-x11perf` / `demo-xcmstest` | `true` (B2 corpus) |
| `xvfb` | `true` (G1 server); `xnest` / `xephyr` default **off** |
| toy demos / `build-font-server` / `build-video-intel` | default **off** (ADR-0014) |
| `xprint` / `xevie` / `build-xproto-legacy` | default **off** |
| `build-safe-proto` | `true` (registers `meson test safe-proto` when `cargo` exists) |

## Legacy gate + X12 smokes (G1 verification)

```sh
./tests/legacy/run.sh   # B2 corpus, then tests/x12/run.sh — G1 gate
./tests/x12/run.sh      # deny_keylog + deny_matrix + QueryVersion + vk_present
./tests/safe_proto/run.sh  # codegen + cargo test + drift + fuzz
```

Requires built `Xvfb`, `xauth`, `xterm`, `x11perf`, `xcmstest`.  
Milestone evidence: [`docs/G1.md`](G1.md) (tag `g1`). Post-G1: [ADR-0016](adr/0016-best-of-best-post-g1.md).

### Xephyr (non-Xvfb) smoke

```sh
meson configure build -Dxephyr=true && meson compile -C build
./tests/x12/run_xephyr.sh
```

## Safe protocol front-end / X12-SURFACE

```sh
./tests/safe_proto/run.sh              # cargo test + IDL drift gate
./tests/surface/run_query_version.sh   # live QueryVersion against Xvfb
./tests/surface/run_vk_present.sh      # lavapipe → Present (needs mesa-vulkan-drivers)
```

Rust crate `safe/x12-proto` (ADR-0011/0012). Needs `cargo` on `PATH` so meson can
build `libx12_proto.a` and enable `X12SURFACE` in the server. Vulkan demo uses
`VK_ICD_FILENAMES` pointing at lavapipe (`lvp_icd.json`).

## Hierarchical levels (XACE)

| Flag | Meaning |
|---|---|
| `-client-level sandbox\|user\|full` | Default level for new clients (ADR-0003 default: **full**) |
| `-sandbox-clients 2,3` | Force sandbox for those client indices (G1 testing) |

Sandbox cannot `QueryKeymap` / keyboard grab, and cannot receive events on foreign windows.

## Known workaround

Xvfb is started with `-extension XFree86-Bigfont` in the legacy harness: QueryFont via Bigfont currently trips `xcb_xlib_too_much_data_requested` in in-tree Xlib/Xaw. Tracked for a protocol-side fix.
