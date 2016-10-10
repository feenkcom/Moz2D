module.exports = {
    project: {
        name: 'Moz2D',
        version: {
            major: 1,
            minor: 1
        },
        cmake: 3.0,
        installer: 'install'
    },

    build: {
        // relative to this config file, will be bounded to @TOPSRCDIR@
        sources: 'mozilla-central',
        // relative to sources dir (mozilla-central)
        objects: 'obj-mozilla',
        // relative to this config file
        patches: 'patches',
        // binary output folder, relative to this config file
        bin: 'build',
        makefile: 'backend.mk',
        mozilla_config: 'mozilla-config.h',
        url: 'https://hg.mozilla.org/mozilla-central/archive/',
        version: 'FIREFOX_AURORA_50_BASE',
        format: '.tar.gz',
        arch: 'i386', // i386, x86_64
        packages: {
            general: [
                'gfx/2d',
                'gfx/cairo/cairo/src',
                'gfx/cairo/libpixman/src',
                'gfx/config',
                'gfx/gl',
                'gfx/graphite2/src',
                'gfx/harfbuzz/src',
                'gfx/ots/src',
                'gfx/qcms',
                'gfx/skia',
                'gfx/src',
                'gfx/thebes',

                'modules/zlib/src',
                'modules/brotli',
                'modules/woff2',

                'memory/fallible',
                'memory/mozalloc',

                'mfbt',

                'dom/encoding',

                'xpcom/glue',
                'xpcom/string',

                'config/external/nspr/libc',
                'config/external/nspr/pr',
                'config/external/nspr/ds',
                'config/external/icu/common',
                'config/external/icu/i18n',

                'intl/locale',
                'intl/unicharutil/util/internal',

                'mozglue/misc'
            ],
            mac: [
                'intl/locale/mac',
                'config/external/icu/stubdata',
            ],
            linux: [
                'intl/locale/unix',
                'config/external/icu/stubdata'
            ],
            win: [
                'intl/locale/windows'
                //'config/external/icu/data'
            ]
        },
        bindings: {
            general: {
                sources: ['src'],
                packages: [
                    'wrapper',
                    'telemetry',
                    'preferences',
                    'services',
                    'undefined'
                ]
            },
            mac: {
                sources: ['src/undefined/mac'],
                packages: ['undefined_mac']
            },
            linux: {
                sources: ['src/undefined/unix'],
                packages: ['undefined_unix']
            },
			win: {
                sources: ['src/undefined/windows'],
                packages: ['undefined_windows']
            }
        },
        libraries: {
            mac: [
                'CoreFoundation',
                'OpenGL',
                'IOSurface',
                'Cocoa',
                'CoreServices',
                'CoreGraphics',
                'CoreText',
                'QuartzCore',
                'IOKit'
            ],
            linux: [
                'dl',
                'X11',
                'X11-xcb',
                'xcb',
                'xcb-shm',
                'Xt',
                'Xext',
                'Xrender',
                'fontconfig',
                'freetype',
                'gtk-x11-2.0',
                'gdk-x11-2.0',
                'atk-1.0',
                'gdk_pixbuf-2.0',
                'gtk-3',
                'gdk-3',
                'gobject-2.0',
                'pango-1.0',
                'pangocairo-1.0',
                'glib-2.0'
            ],
            win: [
				'winmm.lib',
				'wsock32.lib',
				// GetFileVersionInfoSizeW
				'version.lib',
				// ScriptFreeCache
				'usp10.lib',
				// Required by cairo print surface
				'msimg32.lib'
			]
        },
        flags: {
            c: {
                cross: {
                    general: '',
                    mac: '-mssse3 -msse4.1',
                    // for some strange reason -FI does not work with CMake
                    win: '/FI${MOZ_TOP_OBJ_PATH}/mozilla-config.h',
                    linux: '-mssse3 -msse4.1 -fPIC'
                },
                i386: {
                    linux: ''
                },
                x86_64: {
                    linux: '-m64'
                }
            },
            cxx: {
                cross: {
                    general: '',
                    mac: '-mssse3 -msse4.1 -fexceptions',
                    // for some strange reason -FI does not work with CMake
                    win: '/FI${MOZ_TOP_OBJ_PATH}/mozilla-config.h',
                    linux: '-mssse3 -msse4.1 -fexceptions -fPIC'
                },
                i386: {
                    linux: ''
                },
                x86_64: {
                    linux: '-m64'
                }
            },
            asm: {
                cross: {
                    general: '',
                    mac: '',
                    win: '',
                    linux: ''
                },
                i386: {
                    mac: '-arch i386 -m32',
                    linux: '-m32'
                },
                x86_64: {
                    linux: '-m64'
                }
            },
            linker: {
                cross: {
                    general: '',
                    mac: '',
                    win: '',
                    linux: '' //'-Wl,--no-undefined'
                }
            },
            sources: [{
                source: '${MOZ_TOP_PATH}/xpcom/glue/nsThreadUtils.cpp',
                flags: '-DMOZILLA_INTERNAL_API'
            }]
        },
        defines: {
            general: '-DMOZILLA_EXTERNAL_LINKAGE -DMOZ_DUMP_PAINTING -DXPCOM_GLUE_USE_NSPR',
            linux: ''
        },
        undefines: {
            general: [
                'MOZ_CRASHREPORTER',
                'MOZ_CRASHREPORTER_ENABLE_PERCENT',
                'MOZ_LOGGING',
                'MOZ_MEMORY',
                'MOZ_ENABLE_PROFILER_SPS'
            ],
			win: [
			    // SEH exceptions does not work, disable them
			    // https://blogs.msdn.microsoft.com/zhanli/2010/06/25/structured-exception-handling-seh-and-c-exception-handling/
			    'HAVE_SEH_EXCEPTIONS'
			]
        },
        includes: [
            '${MOZ_TOP_OBJ_PATH}/dist/include',
            '${MOZ_TOP_OBJ_PATH}/dist/include/nspr',
            '${MOZ_TOP_PATH}/intl/locale'
        ],
        excludes: {
            linux: ['nsprpub/lib/ds/plvrsion.c']
        }
    },

    config: {
        status: 'config.status',
        mozconfig: '.mozconfig',
        flags: '',
        options: {
            i386: {
                linux: [
                    '--target=i686-pc-linux-gnu',
                    '--x-libraries=/usr/lib32'
                ]
            },
            x86_64: {
                win: [
                    '--target=x86_64-pc-mingw32',
                    '--host=x86_64-pc-mingw32'
                ]
            }
        },
        exports: {
            cross: {
                mac: [],
                linux: [],
                win: [ 'AS=yasm.EXE' ]
            }
        },
        cross_compile: true,
        modules: {
            general: {
                enabled: [
                    'optimize=-O2'
                ],
                disabled: [
                    'debug',
                    'sandbox',
                    //'printing',
                    'gio',
                    'dbus',
                    'synth-speechd',
                    'websms-backend',
                    'dbm',
                    'accessibility',
                    'webrtc',
                    'webspeech',
                    'webspeechtestbackend',
                    'permissions',
                    'negotiateauth',
                    'pref-extensions',
                    'system-extension-dirs',
                    'gamepad',
                    'crashreporter',
                    'updater',
                    'parental-controls',
                    'content-sandbox',
                    'mozril-geoloc',
                    'necko-wifi',
                    'cookies',
                    'ctypes'
                ]
            },
            linux: {
                enabled: ['tree-freetype']
            },
            win: {
                disabled: []
            }
        }
    },
    // visitor pattern
    accept: function(visitor) {
        return visitor.visitConfig(module.exports);
    }
};