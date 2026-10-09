# XValue and XAttribute don't need a display; the other tests create widgets
# and are skipped (exit-code 77) if GTK can't be initialised
xgp_tests = ['XValue', 'XAttribute', 'XAttrLabel', 'XAttrEntry', 'XAttrSpin', 'EnumEntry']

test_env = environment()
test_env.prepend('LD_LIBRARY_PATH', meson.project_build_root() / 'YGP',
                 meson.project_build_root() / 'XGP')

foreach t : xgp_tests
  exe = executable(t, t + '.cpp',
                   cpp_args: ['-DLOCALEDIR="@0@"'.format(localedir)],
                   include_directories: include_directories('../../YGP/Tests'),
                   dependencies: xgp_dep)
  test(t, exe,
       suite: 'XGP',
       workdir: meson.current_source_dir(),
       env: test_env)
endforeach
