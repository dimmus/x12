# Memory-safe protocol front-end (Rust). Does not block the C server build.
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
      ],
      timeout: 120,
      suite: ['safe'],
    )
  else
    warning('build-safe-proto=true but cargo not found; skipping safe-proto meson test')
  endif
endif
