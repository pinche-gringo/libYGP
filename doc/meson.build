# Documentation (needs doxygen): "meson compile -C <builddir> doc"
# The HTML-files are created in the doc-subdirectory of the sources
doxygen = find_program('doxygen', required: false)

doc_conf = configuration_data()
doc_conf.set('VERSION', version)
doc_conf.set('TOP_SRCDIR', meson.project_source_root())
doc_conf.set('DOCDIR', meson.current_source_dir())

doxyfiles = []
foreach lib : ['YGP', 'XGP']
  doxyfiles += configure_file(input: lib + '.doxy.in', output: lib + '.doxy',
                              configuration: doc_conf)
endforeach

if doxygen.found()
  run_target('doc', command: [find_program('sh'), '-c',
                              'doxygen "$1" && doxygen "$2"', 'doxygen'] + doxyfiles)

  run_target('sf-install',
             command: [find_program('sh'), '-c',
                       'cd "$1" && scp -r index.html XGP YGP g17m0,libymp@web.sourceforge.net:htdocs/doc',
                       'scp', meson.current_source_dir()])
endif
