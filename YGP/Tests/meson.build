ygp_tests = [
  'IVIOAppl', 'ANumeric', 'AYear', 'ADate', 'ATime', 'ATStamp', 'XStrBuf', 'XStream', 'Parse',
  'FileRExp', 'CRegExp', 'DirSrch', 'PathDirSrch', 'Attribute', 'AssParse', 'AttrParse',
  'INIFile', 'Thread', 'Relation', 'Process', 'MetaEnum', 'Entity', 'SortDirSrch', 'Module',
  'FileTypeChk',
]

test_args = ['-DLOCALEDIR="@0@"'.format(localedir),
             '-DLIBRELEASE="@0@"'.format(version)]

# The tests expect to be run from within their source directory; the Module
# test needs to find libYGP via dlopen
test_env = environment()
test_env.prepend('LD_LIBRARY_PATH', meson.project_build_root() / 'YGP')

foreach t : ygp_tests
  exe = executable(t, t + '.cpp',
                   cpp_args: test_args,
                   dependencies: ygp_dep)
  test(t, exe,
       workdir: meson.current_source_dir(),
       env: test_env,
       is_parallel: false)
endforeach
