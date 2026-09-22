xgp_headers = files(
  'XAbout.h', 'XApplication.h', 'XFileEntry.h', 'BrowserDlg.h', 'XFileList.h', 'XDate.h',
  'XFileDlg.h', 'XPrintDlg.h', 'XAttrLabel.h', 'XDialog.h', 'XAttrEntry.h', 'XAttribute.h',
  'ConnectDlg.h', 'HTMLViewer.h', 'GTKViewer.h', 'AutoContainer.h', 'MessageDlg.h',
  'LoginDlg.h', 'XValue.h', 'SearchDlg.cpp', 'SearchDlg.h', 'EnumEntry.h', 'XAttrSpin.h',
  'GtkMozViewer.h', 'TraceDlg.h', 'AnimWindow.h', 'WebkitViewer.h',
)

xgp_sources = files(
  'XDialog.cpp', 'XAbout.cpp', 'XApplication.cpp', 'XFileEntry.cpp', 'Version.cpp',
  'XFileList.cpp', 'XDate.cpp', 'XFileDlg.cpp', 'XPrintDlg.cpp', 'BrowserDlg.cpp',
  'ConnectDlg.cpp', 'HTMLViewer.cpp', 'GTKViewer.c', 'AutoContainer.cpp', 'MessageDlg.cpp',
  'LoginDlg.cpp', 'EnumEntry.cpp', 'GtkMozViewer.c', 'TraceDlg.cpp', 'AnimWindow.cpp',
  'WebkitViewer.c', 'SearchDlg.cpp',
)

xgp_deps = [ygp_dep, gtkmm_dep, giomm_dep, dl_dep]
if have_gtk
  xgp_deps += [gtk_dep, gthread_dep]
endif
# The HTML viewers are loaded dynamically; only the headers are needed
foreach d : [gtkhtml_dep, webkit_dep]
  if d.found()
    xgp_deps += d.partial_dependency(compile_args: true, includes: true)
  endif
endforeach

xgp_args = ['-DLOCALEDIR="@0@"'.format(localedir),
            '-DPKGDIR="@0@/"'.format(pkgdatadir_xgp)]

# Named like libtool's "-release" did: libXGP-<version>.so
libxgp = library('XGP-' + version, xgp_sources,
                 c_args: xgp_args,
                 cpp_args: xgp_args + ['-Woverloaded-virtual'],
                 dependencies: xgp_deps,
                 install: true)

if get_option('default_library') != 'static' and shareobj_ext == '.so'
  install_symlink('libXGP.so', pointing_to: 'libXGP-' + version + shareobj_ext,
                  install_dir: get_option('libdir'))
endif

xgp_dep = declare_dependency(link_with: libxgp,
                             include_directories: include_directories('.'),
                             dependencies: [ygp_dep, gtkmm_dep])

install_headers(xgp_headers, subdir: 'XGP')

configure_file(input: 'XGP.pc.in', output: 'XGP.pc', configuration: pc_conf,
               install: true, install_dir: pkgconfigdir)

subdir('Icons')
subdir('Samples')
