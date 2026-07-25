inc_lib += [
    inc_config,
    include_directories(
        '.',
        'bitmaps',
        'dri',
        'extensions',
        'fonts',
        'ICE',
        'SM',
        'Xaw',
        'Xcursor',
        'Xft',
        'Xmu',
        'Xtrans',
    )
]

subdir('dri')
subdir('extensions')
subdir('fonts')
# Unified Athena headers (ADR-0020).
subdir('Xaw')

install_headers(
    'ap_keysym.h',
    'DECkeysym.h',
    'HPkeysym.h',
    'keysymdef.h',
    'keysym.h',
    'Sunkeysym.h',
    'Xalloca.h',
    'Xarch.h',
    'Xatom.h',
    'Xdefs.h',
    'XF86keysym.h',
    'Xfuncs.h',
    'Xfuncproto.h',
    'X.h',
    'Xmd.h',
    'Xosdefs.h',
    'Xos.h',
    'Xos_r.h',
    'Xproto.h',
    'Xprotostr.h',
    'Xthreads.h',
    'Xw32defs.h',
    'XWDFile.h',
    'Xwindows.h',
    'Xwinsock.h',
    subdir: 'X11'
)

# x11
install_headers(
    'XKBlib.h',
    'Xcms.h',
    'Xlib.h',
    'XlibConf.h',
    'Xlibint.h',
    'Xlocale.h',
    'Xresource.h',
    'Xutil.h',
    'cursorfont.h',
    'Xregion.h',
    'ImUtil.h',
    'Xlib-xcb.h',
    subdir: 'X11',
)

# xauth
install_headers(
    'Xauth.h',
    subdir: 'X11',
)

# xdmcp
install_headers(
    'Xdmcp.h',
    subdir: 'X11',
)

# xpm
install_headers(
    'xpm.h',
    subdir: 'X11',
)

# xt
install_headers(
    'Composite.h',
    'CompositeP.h',
    'ConstrainP.h',
    'Constraint.h',
    'Core.h',
    'CoreP.h',
    'Intrinsic.h',
    'IntrinsicP.h',
    'Object.h',
    'ObjectP.h',
    'RectObj.h',
    'RectObjP.h',
    'ResConfigP.h',
    'Shell.h',
    'ShellP.h',
    'StringDefs.h',
    'Vendor.h',
    'VendorP.h',
    'CallbackI.h',
    'ConvertI.h',
    'CreateI.h',
    'EventI.h',
    'HookObjI.h',
    'InitialI.h',
    'IntrinsicI.h',
    'PassivGraI.h',
    'ResourceI.h',
    'SelectionI.h',
    'ShellI.h',
    'ThreadsI.h',
    'TranslateI.h',
    'VarargsI.h',
    'Xtos.h',
    subdir: 'X11',
)

# xshmfence
install_headers(
    'xshmfence.h',
    subdir: 'X11',
)

install_headers(
    'ICE/ICE.h',
    'ICE/ICEconn.h',
    'ICE/ICElib.h',
    'ICE/ICEmsg.h',
    'ICE/ICEproto.h',
    'ICE/ICEutil.h',
    subdir: 'X11/ICE',
)

install_headers(
    'SM/SM.h',
    'SM/SMlib.h',
    'SM/SMproto.h',
    subdir: 'X11/SM',
)

install_headers(
    'Xcursor/Xcursor.h',
    subdir: 'X11/Xcursor',
)

install_headers(
    'Xft/XftCompat.h',
    'Xft/Xft.h',
    subdir: 'X11/Xft',
)

install_headers(
    'Xmu/Xmu.h',
	'Xmu/Atoms.h',
	'Xmu/CharSet.h',
	'Xmu/CloseHook.h',
	'Xmu/Converters.h',
	'Xmu/CurUtil.h',
	'Xmu/CvtCache.h',
	'Xmu/DisplayQue.h',
	'Xmu/Drawing.h',
	'Xmu/Editres.h',
	'Xmu/EditresP.h',
	'Xmu/Error.h',
	'Xmu/ExtAgent.h',
	'Xmu/Initer.h',
	'Xmu/Lookup.h',
	'Xmu/Misc.h',
	'Xmu/StdSel.h',
	'Xmu/StdCmap.h',
	'Xmu/SysUtil.h',
	'Xmu/WidgetNode.h',
	'Xmu/WinUtil.h',
	'Xmu/Xct.h',
	'Xmu/WhitePoint.h',
    subdir: 'X11/Xmu',
)

install_headers(
    'Xtrans/Xtrans.h',
	'Xtrans/Xtrans.c',
	'Xtrans/Xtransint.h',
	'Xtrans/Xtranslcl.c',
	'Xtrans/Xtranssock.c',
	'Xtrans/Xtransutil.c',
	'Xtrans/transport.c',
    subdir: 'X11/Xtrans',
)

# Xpoll.h
fd_set_maybe_headers = [
    'sys/param.h',
    'sys/types.h',
    'sys/time.h',
    'sys/select.h'
]
fd_set_headers = ''
foreach h: fd_set_maybe_headers
    if cc.has_header(h)
        fd_set_headers += '#include <' + h + '>\n'
    endif
endforeach

# generated headers. try not to make more of these, please.
if cc.has_member('fd_set', 'fds_bits', prefix: fd_set_headers)
    fds_bits = 'fds_bits'
elif cc.has_member('fd_set', '__fds_bits', prefix: fd_set_headers)
    fds_bits = '__fds_bits'
else
    error('Your fd_set is too weird.')
endif
conf.set('USE_FDS_BITS', fds_bits)

subdir('bitmaps')