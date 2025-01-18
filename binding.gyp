{
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
                'FFI_BUILDING'
            ],
            'msvs_settings': {
              'VCCLCompilerTool': {
                'LanguageStandard': 'stdcpp20'
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
            'xcode_settings': {
              'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
              'OTHER_CFLAGS': [
                  '-ObjC++'
              ]
            },
            'libraries': [
                '-lobjc'
            ],
          }
        ]
      ]
    }
  ]
}
