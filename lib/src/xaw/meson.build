# X12 unified Athena widget set: lib/src/xaw built as libXaw (ADR-0020).

h_check += ['wctype.h', 'wchar.h', 'widec.h']
f_check += 'iswspace'

conf.set('XAW_ARROW_SCROLLBARS', true)
conf.set('XAW_MULTIPLANE_PIXMAPS', true)
conf.set('XAW_INTERNATIONALIZATION', true)
conf.set('XAW_GRAY_BLKWHT_STIPPLES', true)

inc_xaw = [inc_lib, include_directories('.')]

dep_int_xaw = [libx11, xproto, libxext, libxt, libxmu, libxpm, libxft]
dep_ext_xaw = [dep_freetype2]

src = files(
  'AllWidgets.c',
  'AsciiSink.c',
  'AsciiSrc.c',
  'AsciiText.c',
  'Box.c',
  'Command.c',
  'Dialog.c',
  'Form.c',
  'Grip.c',
  'Label.c',
  'Layout.c',
  'List.c',
  'MenuButton.c',
  'Paned.c',
  'Panner.c',
  'Porthole.c',
  'Repeater.c',
  'Scrollbar.c',
  'Simple.c',
  'SimpleMenu.c',
  'SmeBSB.c',
  'Sme.c',
  'SmeLine.c',
  'SmeThreeD.c',
  'StripChart.c',
  'TextAction.c',
  'Text.c',
  'TextPop.c',
  'TextSink.c',
  'TextSrc.c',
  'TextTr.c',
  'ThreeD.c',
  'Tip.c',
  'Toggle.c',
  'Tree.c',
  'Vendor.c',
  'Viewport.c',
  'Xaw3dP.c',
  'Xaw3dXft.c',
  'XawInit.c',
  # internationalization
  'MultiSrc.c',
  'MultiSink.c',
  'XawIm.c',
  'XawI18n.c',
)

lgen = generator(
  exe_flex,
  output: '@PLAINNAME@.yy.c',
  arguments: ['-i', '-o', '@OUTPUT@', '@INPUT@']
)
src += lgen.process('laylex.l')

pgen = generator(
  exe_bison,
  output: ['@BASENAME@.c', '@BASENAME@.h'],
  arguments: ['@INPUT@', '--defines=@OUTPUT1@', '--output=@OUTPUT0@']
)
src += pgen.process('laygram.y')

lib = library('Xaw', src,
  include_directories: inc_xaw,
  dependencies: [dep_int_xaw, dep_ext_xaw],
  link_with: lib_i18n,
  version: '8.5.0',
  install: true,
)

libxaw = declare_dependency(
  link_with: lib,
  include_directories: inc_lib,
)

pkg.generate(lib,
  description: 'X Athena Widgets Library (unified Xaw / Xft)',
  version: ver_xaw,
  filebase: 'xaw7',
  requires: ['xproto', 'xmu', 'xt', 'xft', 'fontconfig'],
  requires_private: ['x11', 'xext', 'xpm'],
)
