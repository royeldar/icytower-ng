project('icytower', 'c')

app_id = 'io.github.royeldar.icytower'

cc = meson.get_compiler('c')

if host_machine.system() == 'linux'
  liballegro5_deps = [
    dependency('allegro-5'),
    dependency('allegro_primitives-5'),
    dependency('allegro_image-5'),
    dependency('allegro_font-5'),
    dependency('allegro_audio-5'),
    dependency('allegro_acodec-5'),
    dependency('allegro_ttf-5'),
    dependency('allegro_color-5'),
    dependency('allegro_memfile-5'),
    dependency('allegro_physfs-5'),
    dependency('allegro_dialog-5'),
    dependency('allegro_video-5'),
    dependency('allegro_main-5'),
    cc.find_library('m'),
  ]
elif host_machine.system() == 'windows'
  lib_paths = [
    meson.current_source_dir() / 'allegro/lib',
    meson.current_source_dir() / 'allegro_deps/lib',
  ]
  include_dirs = include_directories(
    'allegro/include',
    'allegro_deps/include',
    is_system: true
  )
  libraries = [
    cc.find_library('allegro_monolith-static', dirs: lib_paths),
    cc.find_library('dumb', dirs: lib_paths),
    cc.find_library('FLAC', dirs: lib_paths),
    cc.find_library('freetype', dirs: lib_paths),
    cc.find_library('jpeg', dirs: lib_paths),
    cc.find_library('ogg', dirs: lib_paths),
    cc.find_library('openmpt', dirs: lib_paths),
    cc.find_library('opus', dirs: lib_paths),
    cc.find_library('opusfile', dirs: lib_paths),
    cc.find_library('physfs', dirs: lib_paths),
    cc.find_library('png16', dirs: lib_paths),
    cc.find_library('sharpyuv', dirs: lib_paths),
    cc.find_library('theoradec', dirs: lib_paths),
    cc.find_library('vorbis', dirs: lib_paths),
    cc.find_library('vorbisfile', dirs: lib_paths),
    cc.find_library('webp', dirs: lib_paths),
    cc.find_library('webpdecoder', dirs: lib_paths),
    cc.find_library('webpdemux', dirs: lib_paths),
    cc.find_library('zlib', dirs: lib_paths),
  ]
  libraries += [
    cc.find_library('ssp', static: true),
    cc.find_library('stdc++', static: true),
    cc.find_library('winpthread', static: true),
    cc.find_library('dsound'),
    cc.find_library('opengl32'),
    cc.find_library('shlwapi'),
    cc.find_library('winmm'),
  ]
  liballegro5_deps = declare_dependency(
    compile_args: ['-DALLEGRO_STATICLINK'],
    dependencies: libraries,
    include_directories: include_dirs,
    link_args: ['-static-libgcc', '-static-libstdc++'],
  )
endif

sources = files(
  'main.c',
  'background.c',
  'character.c',
  'config.c',
  'controls.c',
  'events.c',
  'exit.c',
  'flatpak.c',
  'floor.c',
  'fonts.c',
  'fullscreen.c',
  'game.c',
  'gameplay.c',
  'gfx.c',
  'hashmap.c',
  'instructions.c',
  'keyboard.c',
  'keymap.c',
  'level.c',
  'menu.c',
  'music.c',
  'music_volume.c',
  'random.c',
  'rebind.c',
  'reward.c',
  'render.c',
  'resources.c',
  'scene.c',
  'screenshake.c',
  'sfx.c',
  'shared_state.c',
  'sound.c',
  'sound_volume.c',
  'synchronization.c',
  'triple_buffer.c',
  'eye_candy.c',
)

if host_machine.system() == 'linux'
  assets_dir = get_option('datadir') / app_id
else
  assets_dir = get_option('bindir')
endif

executable(
  'icytower',
  sources: sources,
  dependencies: liballegro5_deps,
  win_subsystem: 'windows',
  install: true,
)

install_subdir(
  'misc/gfx',
  exclude_files: ['.gitattributes', '.gitignore'],
  install_dir: assets_dir,
)
install_subdir(
  'misc/sfx',
  exclude_files: ['.gitattributes', '.gitignore'],
  install_dir: assets_dir,
)

if host_machine.system() == 'linux'
  install_data(
    'misc/icytower.desktop',
    rename: app_id + '.desktop',
    install_dir: get_option('datadir') / 'applications',
  )
  install_data(
    'misc/icytower.svg',
    rename: app_id + '.svg',
    install_dir: get_option('datadir') / 'icons/hicolor/scalable/apps',
  )
endif
