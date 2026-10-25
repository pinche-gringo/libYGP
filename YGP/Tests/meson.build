ygp_tests = [
  'IVIOAppl', 'ANumeric', 'AYear', 'ADate', 'ATime', 'ATStamp', 'XStrBuf', 'XStream', 'Parse',
  'FileRExp', 'DirSrch', 'PathDirSrch', 'Attribute', 'AssParse', 'AttrParse',
  'INIFile', 'Relation', 'Process', 'MetaEnum', 'Entity', 'SortDirSrch',
  'FileTypeChk', 'ConnMgr', 'Utility',
]

test_args = ['-DLOCALEDIR="@0@"'.format(localedir)]

# The tests expect to be run from within their source directory

foreach t : ygp_tests
  exe = executable(t, t + '.cpp',
                   cpp_args: test_args,
                   dependencies: ygp_dep)
  test(t, exe,
       workdir: meson.current_source_dir(),
       is_parallel: false)
endforeach
