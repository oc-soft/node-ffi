{
  'variables': {
    'target_arch%': 'ia32',
  },
  'targets': [
    {
      'target_name': 'ffi_bindings',
      'sources': [
        'src/node-module.cc',
        'src/call.cc',
        'src/prep.cc',
        'src/constants.cc',
        'src/callback.cc',
        'src/closure.cc',
        'src/code-object.cc',
        'src/async-call.cc',
        'src/async-handle.cc',
        'src/objc-object-wrap.cc',
        'src/wrap-pointer.cc',
        'src/dl.cc',
        'src/errno.cc',
        'src/types.cc',
        'src/ffi-config.cc'
      ],
      'include_dirs': [
        '<!(node -e "require(\'nan\')")',
        'include'
      ],
      'dependencies': [
        'deps/libffi/libffi.gyp:ffi'
      ],

      'conditions': [
        [
          'OS=="win"',
          {
            'sources': [
                'src/win32-dlfcn.cc',
            ],
            'defines': [
                'FFI_BUILDING',
		'FFI_STATIC_BUILD'
            ],
            'msvs_settings': {
              'VCCLCompilerTool': {
                'AdditionalOptions': [ '/std:c++20' ],
		'PreprocessorDefinitions' : [ 'NOMINMAX' ]
              },
              'VCLibrarianTool': {
              },
              'VCLinkerTool': {
                'AdditionalOptions': [
                  '/MAP'
                ] 
              }
            }
          }
        ],
        [
          'OS=="mac"',
          {
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
            ],
            'libraries': [
              '-lobjc'
            ], 
            'xcode_settings': {
              'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
              'OTHER_CFLAGS': [
                  '-ObjC++'
              ]
            },
          },
        ]
      ]
    }
  ]
}
