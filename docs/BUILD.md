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
| `build-test` / `build-demo` / `demo-xcmstest` | `true` (legacy corpus) |
| `build-safe-proto` | `true` (registers `meson test safe-proto` when `cargo` exists) |

## Legacy gate

```sh
./tests/legacy/run.sh
```

Requires built `Xvfb`, `xauth`, `xterm`, `x11perf`, `xcmstest`. Also runs `tests/security/run_deny_keylog.sh`.

## Safe protocol front-end

```sh
./tests/safe_proto/run.sh
```

Rust crate `safe/x12-proto` (ADR-0011). Needs `cargo` on `PATH`.

## Hierarchical levels (XACE)

| Flag | Meaning |
|---|---|
| `-client-level sandbox\|user\|full` | Default level for new clients (ADR-0003 default: **full**) |
| `-sandbox-clients 2,3` | Force sandbox for those client indices (G1 testing) |

Sandbox cannot `QueryKeymap` / keyboard grab, and cannot receive events on foreign windows.

## Known workaround

Xvfb is started with `-extension XFree86-Bigfont` in the legacy harness: QueryFont via Bigfont currently trips `xcb_xlib_too_much_data_requested` in in-tree Xlib/Xaw. Tracked for a protocol-side fix.
