# Sample application; not built by default (build it with "meson compile -C <builddir> X-Appl")
executable('X-Appl', 'X-Appl.cpp', 'Dialog.cpp', 'AnimWindow.cpp',
           cpp_args: ['-DLOCALEDIR="@0@"'.format(localedir), '-Woverloaded-virtual'],
           include_directories: include_directories('../../YGP'),
           dependencies: xgp_dep,
           build_by_default: false)
