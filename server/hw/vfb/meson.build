srcs = [
    'InitInput.c',
    'InitOutput.c',
    '../../mi/miinitext.c',
    '../../mi/miinitext.h',
]

xvfb_link_depends = []
if build_x12_surface
    xvfb_link_depends += x12_proto_a
endif

xvfb_server = executable('Xvfb',
    srcs,
    include_directories: inc_server,
    dependencies: dep_server_exe,
    link_depends: xvfb_link_depends,
    link_with: [
        libxserver_main,
        libxserver_fb,
        libxserver,
        libxserver_xkb_stubs,
        libxserver_xi_stubs,
        libxserver_glx,
        libglxvnd,
    ],
    install: true,
)