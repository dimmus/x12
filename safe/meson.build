# Memory-safe protocol front-end (Rust) — tests + staticlib for dix (ADR-0012).

dep_x12_proto = disabler()
build_x12_surface = false
x12_proto_a = []

if get_option('build-safe-proto')
  cargo = find_program('cargo', required: false)
  if cargo.found()
    test(
      'safe-proto',
      cargo,
      args: [
        'test',
        '--manifest-path',
        meson.project_source_root() / 'safe' / 'x12-proto' / 'Cargo.toml',
        '--locked',
      ],
      timeout: 120,
      suite: ['safe'],
    )

    cargo_target = meson.current_build_dir() / 'cargo-target'
    x12_proto_sources = files(
      'x12-proto/Cargo.toml',
      'x12-proto/Cargo.lock',
      'x12-proto/src/lib.rs',
      'x12-proto/src/ffi.rs',
      'x12-proto/src/frame.rs',
      'x12-proto/src/level.rs',
      'x12-proto/src/x12_surface.rs',
      'x12-proto/src/generated_wire.rs',
      'x12-proto/include/x12_proto.h',
      'x12-proto/include/x12_proto_generated.h',
      'x12-proto/tools/gen_wire.py',
    )

    x12_proto_a = custom_target(
      'libx12_proto_a',
      input: x12_proto_sources,
      output: 'libx12_proto.a',
      command: [
        find_program('sh'),
        '-c',
        'cargo build --release --locked --manifest-path "$1" --target-dir "$2" && cp "$2/release/libx12_proto.a" "$3"',
        'sh',
        meson.project_source_root() / 'safe' / 'x12-proto' / 'Cargo.toml',
        cargo_target,
        '@OUTPUT@',
      ],
      console: true,
      build_by_default: true,
    )

    # Attach to final server executables via dep_server_exe + link_depends: x12_proto_a.
    # Do NOT add this dependency to static_library() targets (ar would choke on the .a).
    dep_x12_proto = declare_dependency(
      include_directories: include_directories('x12-proto/include'),
      link_args: [
        '-Wl,--whole-archive',
        x12_proto_a.full_path(),
        '-Wl,--no-whole-archive',
        '-ldl',
        '-lpthread',
        '-lm',
      ],
    )
    build_x12_surface = true
  else
    warning('build-safe-proto=true but cargo not found; X12-SURFACE server stub disabled')
  endif
endif
