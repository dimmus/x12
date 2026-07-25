# X12 reference WM (xwm): twm-class session face over the in-tree twm engine.

xwm_conf = configuration_data()
xwm_conf.set('BINDIR', dir_bin)
xwm_conf.set('DATADIR', dir_data)

configure_file(
  input: 'xwm.in',
  output: 'xwm',
  configuration: xwm_conf,
  install: true,
  install_dir: dir_bin,
  install_mode: 'rwxr-xr-x',
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
