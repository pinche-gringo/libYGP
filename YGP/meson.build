ygp_sources = files(
  'Path.cpp', 'IVIOAppl.cpp', 'DirSrch.cpp', 'ANumeric.cpp', 'AssParse.cpp', 'Parse.cpp',
  'FileRExp.cpp', 'Version.cpp', 'ATime.cpp', 'XStrBuf.cpp', 'AYear.cpp', 'ADate.cpp',
  'PathDirSrch.cpp', 'ATStamp.cpp', 'INIFile.cpp', 'CRegExp.cpp', 'StackTrc.cpp', 'Socket.cpp',
  'File.cpp', 'AttrParse.cpp', 'IDirSrch.cpp', 'Entity.cpp', 'Thread.cpp', 'XDirSrch.cpp',
  'Check.cpp', 'Process.cpp', 'ConnMgr.cpp', 'StatusObj.cpp', 'Relation.cpp', 'TableWriter.cpp',
  'MetaEnum.cpp', 'DynTrace.cpp', 'Module.cpp', 'Attribute.cpp', 'FileTypeChk.cpp', 'Utility.cpp',
)

ygp_headers = files(
  'Path.h', 'ANumeric.h', 'DirSrch.h', 'IVIOAppl.h', 'AttrVal.h', 'FileRExp.h', 'XStrBuf.h',
  'XStream.h', 'Parse.h', 'Trace.h', 'Check.h', 'PathDirSrch.h', 'AYear.h', 'ADate.h',
  'ATStamp.h', 'ATime.h', 'CRegExp.h', 'RegExp.h', 'Log.h', 'StackTrc.h', 'Socket.h',
  'IDirSrch.h', 'File.h', 'AssParse.h', 'Attribute.h', 'AttrParse.h', 'Entity.h', 'Thread.h',
  'Mutex.h', 'INIFile.h', 'XDirSrch.h', 'ConnMgr.h', 'Process.h', 'StatusObj.h', 'Relation.h',
  'TableWriter.h', 'MetaEnum.h', 'SortDirSrch.h', 'DynTrace.h', 'Module.h', 'Exception.h',
  'FileTypeChk.h', 'Utility.h',
)

if get_option('network_traversing')
  ygp_sources += files('RemoteFile.cpp', 'RDirSrch.cpp', 'RDirSrchSrv.cpp')
  ygp_headers += files('RemoteFile.h', 'RDirSrch.h', 'RDirSrchSrv.h')
endif

ygp_deps = [boost_dep, dl_dep, threads_dep, socket_dep, ws2_dep, intl_dep, gmp_dep]
if have_gtk
  # GTK is loaded dynamically (see Check.cpp); only the headers are needed
  ygp_deps += gtk_dep.partial_dependency(compile_args: true, includes: true)
endif

# The library is named like libtool's "-release" did: libYGP-<version>.so
libygp = library('YGP-' + version, ygp_sources,
                 include_directories: top_inc,
                 cpp_args: ['-DLOCALEDIR="@0@"'.format(localedir),
                            '-Woverloaded-virtual', '-Weffc++'],
                 dependencies: ygp_deps,
                 install: true)

if get_option('default_library') != 'static' and shareobj_ext == '.so'
  install_symlink('libYGP.so', pointing_to: 'libYGP-' + version + shareobj_ext,
                  install_dir: get_option('libdir'))
endif

ygp_dep = declare_dependency(link_with: libygp,
                             include_directories: top_inc,
                             dependencies: [intl_dep, gmp_dep])

install_headers(ygp_headers, subdir: 'YGP')

configure_file(input: 'YGP.pc.in', output: 'YGP.pc', configuration: pc_conf,
               install: true, install_dir: pkgconfigdir)

subdir('Tests')
