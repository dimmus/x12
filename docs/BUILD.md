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

## Legacy gate

```sh
./tests/legacy/run.sh
```

Requires built `Xvfb`, `xauth`, `xterm`, `x11perf`, `xcmstest`.

## Known workaround

Xvfb is started with `-extension XFree86-Bigfont` in the legacy harness: QueryFont via Bigfont currently trips `xcb_xlib_too_much_data_requested` in in-tree Xlib/Xaw. Tracked for a protocol-side fix.
