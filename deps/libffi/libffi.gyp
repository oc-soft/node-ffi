# This file is used with the GYP meta build system.
# http://code.google.com/p/gyp
# To build try this:
#   svn co http://gyp.googlecode.com/svn/trunk gyp
#   ./gyp/gyp -f make --depth=`pwd` libffi.gyp
#   make
#   ./out/Debug/test
{
  'variables': {
    'target_arch%': 'ia32',
    # it will be set i386 almost
  },
  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      # TODO: hoist these out and put them somewhere common, because
      #       RuntimeLibrary MUST MATCH across the entire project
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 1, # static debug
          },
        },
      },
      'Release': {
        'defines': [ 'NDEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 0, # static release
          },
        },
      }
    },
    'msvs_settings': {
      'VCCLCompilerTool': {
      },
      'VCLibrarianTool': {
      },
      'VCLinkerTool': {
        'GenerateDebugInformation': 'true',
      },
    },
    'conditions': [
      [
        'OS == "win"', {
          'defines': [
            'WIN32',
            'FFI_STATIC_BUILD'
          ],
        }
      ],
      [
        'OS == "mac"', {
          'conditions': [
            ['target_arch=="arm64" or target_arch=="aarch64"',
              {
                'variables': {
                  'target_arch_name': 'aarch64'
                }
              }
            ],
            ['target_arch=="x86_64"', 
              {
                'variables': {
                  'target_arch_name': 'x86-64'
                }
              },
              {
                'variables': {
                  'target_arch_name': '<(target_arch)'
                }
              }
            ]
          ],
          'cflags': [
            '-target', '<(target_arch_name)'
          ],
          'ldflags': [
            '-target', '<(target_arch_name)'
          ]
        }
      ]
    ],
  },

  # Compile .asm files on Windows
  'conditions': [
    ['OS=="win"', {
      'target_defaults': {
        'conditions': [
          ['target_arch=="ia32" or target_arch=="i386"', {
            'variables': { 'ml': ['ml', '/nologo', '/safeseh' ] }
          }, {
            'variables': { 'ml': ['ml64', '/nologo' ] }
          }]
        ],
        'rules': [
          {
            'rule_name': 'assembler',
            'msvs_cygwin_shell': 0,
            'extension': 'asm',
            'inputs': [
            ],
            'outputs': [
              '<(INTERMEDIATE_DIR)/<(RULE_INPUT_ROOT).obj',
            ],
            'action': [
              '<@(ml)', '/c', '/Fo<(INTERMEDIATE_DIR)/<(RULE_INPUT_ROOT).obj', '<(RULE_INPUT_PATH)'
            ],
            'message': 'Building assembly file <(RULE_INPUT_PATH)',
            'process_outputs_as_sources': 1,
          },
        ],
      },
    }],
  ],
  'targets': [
    {
      'target_name': 'ffi',
      'product_prefix': 'lib',
      'type': 'static_library',

      # for CentOS 5 support: https://github.com/rbranson/node-ffi/issues/110
      'standalone_static_library': 1,
      'sources': [
        'src/prep_cif.c',
        'src/types.c',
        'src/raw_api.c',
        'src/java_raw_api.c',
        'src/closures.c',
        'src/tramp.c'
      ],
      'defines': [
        'PIC',
        'FFI_BUILDING',
        'HAVE_CONFIG_H'
      ],
      'include_dirs': [
        'include',
        # platform and arch-specific headers
        'config/<(OS)/<(target_arch)'
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          'include',
          # platform and arch-specific headers
          'config/<(OS)/<(target_arch)'
        ],
      },
      'conditions': [
        ['target_arch=="arm"', {
          'sources': [ 'src/arm/ffi.c' ],
          'conditions': [
            ['OS=="linux"', {
              'sources': [ 'src/arm/sysv.S' ]
            }]
          ]
        }],
        ['target_arch=="aarch64" or target_arch=="arm64"',
          {
            'include_dirs': [
              'src/aarch64'
            ],
            'sources': [ 
              'src/aarch64/ffi.c',
              'src/aarch64/sysv.S'
            ],
          }, { # ia32 or x64
            'sources': [
              'src/x86/ffi.c',
              'src/x86/ffi64.c',
              'src/x86/ffiw64.c',
              'src/x86/sysv.S',
              'src/x86/sysv_intel.S',
              'src/x86/unix64.S',
              'src/x86/win64.S',
              'src/x86/win64_intel.S',
              'src/x86/win64.S',
            ],
            'conditions': [
              ['OS=="win"',
                {
                  # the libffi dlmalloc.c file has a bunch of implicit
                  # conversion warnings, and the main ffi.c file contains one,
                  # so silence them
                  'msvs_disabled_warnings': [ 4267 ],
                  # the ffi64.c file is never compiled on Windows
                  'sources!': [
                    'src/x86/ffi.c',
                    'src/x86/ffi64.c',
                    'src/x86/sysv.S',
                    'src/x86/sysv_intel.S',
                    'src/x86/unix64.S',
                    'src/x86/win64.S',
                    'src/x86/win64_intel.S',
                  ],
                  'conditions': [
                    ['target_arch=="ia32"',
                      {
                        'sources!': [
                          'src/x86/ffi64.c',
                          'src/x86/ffiw64.c',
		                      'src/x86/sysv.S',
                          'src/x86/sysv_intel.S',
                          'src/x86/unix64.S',
                          'src/x86/win64.S',
                          'src/x86/win64_intel.S',
                          'src/x86/win64.S',
                         ]
                      },
                      { # target_arch=="x64"
                        'sources': [
                          'src/x86/win64_intel.asm'
                        ]
                      }
                    ]
                  ]
                }
              ],
              ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris" or OS=="mac"', 
                {
                  'conditions': [
                    ['target_arch=="ia32"',
                      {
                        'sources!': [
                          'src/x86/ffi64.c',
                          'src/x86/ffiw64.c',
                          'src/x86/unix64.S',
                          'src/x86/win64.S',
                          'src/x86/win64_intel.S',
                         ]
                      }
                    ],
                    ['target_arch=="x64"',
                      {
                        'sources!': [
                          'src/x86/ffi.c',
                          'src/x86/sysv.S',
                          'src/x86/sysv_intel.S',
                          'src/x86/win64_intel.S',
                        ]
                      }
                    ]
                  ]
                }
              ]
            ]
          }
        ],
        [
          'target_arch=="arm64"', 
          {
            'include_dirs': [
              'config/<(OS)/aarch64',
            ],
            'direct_dependent_settings': {
              'include_dirs': [
                # platform and arch-specific headers
                'config/<(OS)/aarch64'
              ],
            },
          }
        ]
      ]
    },
    {
      'target_name': 'test',
      'type': 'executable',
      'dependencies': [ 'ffi' ],
      'sources': [ 'test.c' ]
    },
    {
      'target_name': 'closure-test',
      'type': 'executable',
      'dependencies': [ 'ffi' ],
      'sources': [ 'closure.c' ]
    }
  ]
}
