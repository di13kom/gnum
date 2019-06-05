gnum_sources = [
  'main.c',
  'gnum-app-window.c',
  'gnum-app.c',
  'gnum-dialog-win.c'
]

gnum_deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk+-3.0', version: '>= 3.22'),
]

gnome = import('gnome')

gnum_sources += gnome.compile_resources('gnum-resources',
  'gnum.gresource.xml',
  c_name: 'gnum'
)

executable('gnum', gnum_sources,
  dependencies: gnum_deps,
  install: true,
)
