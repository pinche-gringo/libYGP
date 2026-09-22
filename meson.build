project('libYGP', ['c', 'cpp'],
        version: '0.9.50',
        license: 'GPL-3.0-or-later',
        meson_version: '>= 1.1',
        default_options: ['cpp_std=c++23',
                          'warning_level=2',
                          'buildtype=debugoptimized'])

version = meson.project_version()
version_parts = version.split('.')
release = '@0@.@1@'.format(version_parts[0], version_parts[1])  # 0.9.50 -> 0.9

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')
fs = import('fs')

prefix = get_option('prefix')
localedir = prefix / get_option('localedir')
pkgdatadir_xgp = prefix / get_option('datadir') / 'XGP'
pkgconfigdir = get_option('libdir') / 'pkgconfig'

if host_machine.system() == 'windows' or host_machine.system() == 'cygwin'
  shareobj_ext = '.dll'
elif host_machine.system() == 'darwin'
  shareobj_ext = '.dylib'
else
  shareobj_ext = '.so'
endif

conf = configuration_data()
conf.set_quoted('PACKAGE_NAME', meson.project_name())
conf.set_quoted('PACKAGE_VERSION', version)
conf.set_quoted('LIB_RELEASE', release)
conf.set_quoted('LIBYGP_NAME', meson.project_name())
conf.set_quoted('SHAREOBJ_EXT', shareobj_ext)

# Results of feature checks; the found ones are defined to 1 in ygp-cfg.h
have = {}

# Trace- and check-levels (formerly --enable-debug / --enable-checks)
if get_option('trace_level') >= 0
  add_project_arguments('-DTRACELEVEL=@0@'.format(get_option('trace_level')), language: ['c', 'cpp'])
endif
if get_option('check_level') >= 0
  add_project_arguments('-DCHECK=@0@'.format(get_option('check_level')), language: ['c', 'cpp'])
endif

#-----------------------------------------------------------------------------
# Libraries
#-----------------------------------------------------------------------------
dl_dep = dependency('dl', required: false)
have += {'HAVE_LIBDL': dl_dep.found()}

threads_dep = dependency('threads', required: false)
have += {'HAVE_LIBPTHREAD': threads_dep.found()}

socket_dep = cc.find_library('socket', required: false)
have += {'HAVE_LIBSOCKET': socket_dep.found()}
ws2_dep = cc.find_library('ws2_32', required: false)
have += {'HAVE_LIBWS2_32': ws2_dep.found()}

boost_dep = dependency('boost', modules: ['filesystem'], required: false)
if not boost_dep.found()
  error('''Required library boost_filesystem not found!
Either it can't be found in your default link-path or install it using your
package-manager or from its website at http://www.boost.org''')
endif
have += {'HAVE_LIBBOOST_FILESYSTEM': true}
foreach h : ['boost/filesystem/path.hpp', 'boost/shared_ptr.hpp', 'boost/tokenizer.hpp',
             'boost/scoped_ptr.hpp', 'boost/spirit/include/classic_core.hpp']
  if not cxx.has_header(h, dependencies: boost_dep)
    error('Required header @0@ not found'.format(h))
  endif
  have += {'HAVE_' + h.underscorify().to_upper(): true}
endforeach

gmp_dep = dependency('', required: false)
if get_option('gmp').allowed()
  gmp_dep = dependency('gmp', required: false)
  if not gmp_dep.found()
    gmp_dep = cc.find_library('gmp', required: get_option('gmp'))
  endif
endif
have += {'HAVE_LIBGMP': gmp_dep.found()}
lib_gmp = gmp_dep.found() ? '-lgmp' : ''

intl_dep = dependency('intl', required: get_option('nls'))
msgfmt = find_program('msgfmt', required: get_option('nls'))
enable_nls = intl_dep.found() and msgfmt.found()
have += {'ENABLE_NLS': enable_nls}
have += {'HAVE_GETTEXT': enable_nls}
have += {'HAVE_DCGETTEXT': enable_nls}

#-----------------------------------------------------------------------------
# Headers, functions, types
#-----------------------------------------------------------------------------
foreach h : ['regex.h', 'syslog.h', 'sys/socket.h', 'winsock2.h', 'windows.h', 'sys/wait.h',
             'dlfcn.h']
  have += {'HAVE_' + h.underscorify().to_upper(): cc.has_header(h)}
endforeach

# Like AC_HEADER_DIRENT: The first header defining DIR
foreach h : ['dirent.h', 'sys/ndir.h', 'sys/dir.h', 'ndir.h']
  if cc.has_header_symbol(h, 'DIR', prefix: '#include <sys/types.h>')
    have += {'HAVE_' + h.underscorify().to_upper(): true}
    break
  endif
endforeach

foreach f : ['timegm', 'strfmon', 'beginthread', '_beginthread', 'pipe', '_pipe', 'fork',
             'spawnvp', 'sleep', '_sleep', 'getlogin', 'strftime', 'strptime', 'backtrace',
             'getpid', '_getpid', 'wcscoll', 'mbtowc', 'tzset', '_tzset']
  have += {'HAVE_' + f.to_upper(): cc.has_function(f, dependencies: threads_dep)}
endforeach
have += {'HAVE_FNMATCH': cc.has_function('fnmatch', prefix: '#include <fnmatch.h>')}

have += {'HAVE_SSIZE_T': cc.has_type('ssize_t', prefix: '#include <sys/types.h>')}
have += {'HAVE_SOCKLEN_T': cc.has_type('socklen_t', prefix: '#include <sys/socket.h>')}
foreach t : ['short', 'int', 'long']
  conf.set('SIZEOF_' + t.to_upper(), cc.sizeof(t))
endforeach
have += {'WORDS_BIGENDIAN': host_machine.endian() == 'big'}

have += {'HAVE_SIGBUS': cc.compiles('''#include <signal.h>
int main () { signal (SIGBUS, SIG_DFL); return 0; }''', name: 'signal.h defines SIGBUS')}

if meson.can_run_host_binaries()
  strftime_len = cxx.run('''#include <cstdio>
#include <ctime>
int main () {
   time_t time (time (NULL));
   return strftime (NULL, 10, "%d", gmtime (&time)) > 0 ? 0 : 1; }''',
                         name: 'strftime returns the needed length')
  have += {'STRFTIME_RETURNS_LENGTH': strftime_len.compiled() and strftime_len.returncode() == 0}
else
  warning('Disabling dynamic calculation of buffersize for strftime')
endif

have += {'HAVE_NETWORK_TRAVERSING': get_option('network_traversing')}

#-----------------------------------------------------------------------------
# GUI libraries
#-----------------------------------------------------------------------------
gtk_dep = dependency('gtk4', version: '>= 4.0.0', required: false)
gthread_dep = dependency('gthread-2.0', version: '> 2.2.0', required: false)
have_gtk = gtk_dep.found() and gthread_dep.found()
have += {'HAVE_GTK': have_gtk}
if not have_gtk
  message('No GTK+: Checks will be only textual')
endif

gtkmm_dep = dependency('gtkmm-4.0', version: '>= 4.0.0', required: get_option('xgp'))
build_xgp = gtkmm_dep.found()
if not build_xgp
  warning('Subdir XGP will not be made')
endif

giomm_dep = dependency('giomm-2.4', version: '>= 2.4.0', required: false)
have += {'HAVE_GIOMM': giomm_dep.found()}
if not giomm_dep.found()
  message('Using own icon-handling')
endif

gtkhtml_dep = dependency('', required: false)
foreach v : ['3.14', '3.8', '3.5', '3.1']
  gtkhtml_dep = dependency('libgtkhtml-' + v, version: '>= ' + v + '.0', required: false)
  if gtkhtml_dep.found()
    conf.set_quoted('HAVE_GTKHTML', v)
    break
  endif
endforeach

webkit_dep = dependency('webkit-1.0', version: '>= 1.0.0', required: false)
have += {'HAVE_WEBKIT': webkit_dep.found()}

#-----------------------------------------------------------------------------
# Programs
#-----------------------------------------------------------------------------
if not find_program('perl', required: false).found()
  warning('Program \'perl\' not found - bin/mgeni.pl will not work!')
endif

foreach b : [['epiphany', 'EPIPHANY'], ['firefox', 'FIREFOX'], ['galeon', 'GALEON'],
             ['seamonkey', 'SEAMONKEY'], ['mozilla', 'MOZILLA'], ['netscape', 'NETSCAPE'],
             ['konqueror', 'KONQUEROR'], ['opera', 'OPERA'], ['dillo', 'DILLO'],
             ['iexplore', 'IEXPLORER']]
  conf.set('HAVE_' + b[1], find_program(b[0], required: false).found() ? 1 : 0)
endforeach

#-----------------------------------------------------------------------------
# Output
#-----------------------------------------------------------------------------
foreach name, found : have
  if found
    conf.set(name, 1)
  endif
endforeach
configure_file(input: 'ygp-cfg.h.in', output: 'ygp-cfg.h', configuration: conf,
               install: true, install_dir: get_option('includedir'))
install_headers('config.h')

top_inc = include_directories('.')

# Values for the pkg-config files
pc_conf = configuration_data()
pc_conf.set('prefix', prefix)
pc_conf.set('exec_prefix', '${prefix}')
pc_conf.set('libdir', '${exec_prefix}' / get_option('libdir'))
pc_conf.set('includedir', '${prefix}' / get_option('includedir'))
pc_conf.set('PACKAGE_VERSION', version)
pc_conf.set('LIB_GMP', lib_gmp)

subdir('YGP')
if build_xgp
  subdir('XGP')
endif
subdir('bin')
if enable_nls
  subdir('po')
endif
subdir('doc')

summary({'XGP': build_xgp,
         'NLS': enable_nls,
         'GMP': gmp_dep.found(),
         'Network traversing': get_option('network_traversing'),
         'Trace level': get_option('trace_level'),
         'Check level': get_option('check_level')},
        bool_yn: true)
