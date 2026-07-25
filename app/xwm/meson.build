# X12 reference WM — from-scratch ICCCM engine (ADR-0019).

xwm_conf = configuration_data()
xwm_conf.set('BINDIR', dir_bin)
xwm_conf.set('DATADIR', dir_data)

xwm_src = files(
  'main.c',
  'atoms.c',
  'client.c',
  'config.c',
  'events.c',
  'log.c',
  'menu.c',
)

executable(
  'xwm',
  xwm_src,
  dependencies: [libx11, libxext, xproto],
  c_args: ['-DDATADIR="@0@"'.format(dir_data)],
  install: true,
)

install_data('system.xwmrc', install_dir: dir_data / 'X11' / 'xwm')

configure_file(
  input: 'x12-xinitrc.in',
  output: 'x12-xinitrc',
  configuration: xwm_conf,
  install: true,
  install_dir: dir_data / 'X11' / 'xwm',
  install_mode: 'rwxr-xr-x',
)
