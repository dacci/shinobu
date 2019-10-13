# Copyright 2009 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'variables': {
      'clang%': 0,
      'conditions': [
        ['OS=="mac"', {
          'clang%': 1,
        }],
      ],
    },
    'clang%': '<(clang)',

    'mac_sdk%': '',
    'mac_deployment_target%': '',

    'pkg-config%': 'pkg-config',

    'vc_runtime%': 'dynamic',

    'target_arch%': 'x64',
  },

  'target_defaults': {
    'includes': [
      'filename_rules.gypi',
    ],
    'conditions': [

      ['OS=="mac"', {
        'xcode_settings': {
          'ALWAYS_SEARCH_USER_PATHS': 'NO',
          'GCC_C_LANGUAGE_STANDARD': 'c17',  # -std=c17
          'GCC_CW_ASM_SYNTAX': 'NO',  # No -fasm-blocks
          'GCC_DYNAMIC_NO_PIC': 'NO',  # No -mdynamic-no-pic
          'GCC_ENABLE_CPP_EXCEPTIONS': 'NO',  # -fno-exceptions
          'GCC_ENABLE_CPP_RTTI': 'NO',  # -fno-rtti
          'GCC_ENABLE_PASCAL_STRINGS': 'NO',  # No -mpascal-strings

          # GCC_INLINES_ARE_PRIVATE_EXTERN maps to -fvisibility-inlines-hidden
          'GCC_INLINES_ARE_PRIVATE_EXTERN': 'YES',

          'GCC_OBJC_CALL_CXX_CDTORS': 'YES',  # -fobjc-call-cxx-cdtors
          'GCC_PRECOMPILE_PREFIX_HEADER': 'NO',
          'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',  # -fvisibility=hidden
          'GCC_TREAT_WARNINGS_AS_ERRORS': 'YES',  # -Werror
          'GCC_WARN_ABOUT_MISSING_NEWLINE': 'YES',  # -Wnewline-eof
          'OTHER_CFLAGS': [
            '-fno-strict-aliasing',  # See http://crbug.com/32204
            '-fstack-protector-all',  # Implies -fstack-protector
          ],
          'USE_HEADERMAP': 'NO',
          'WARNING_CFLAGS': [
            '-Wall',
            '-Wendif-labels',
            '-Wextra',

            # Don't warn about unused function parameters.
            '-Wno-unused-parameter',

            # Don't warn about the "struct foo f = {0};" initialization
            # pattern.
            '-Wno-missing-field-initializers',
          ],

          'conditions': [
            ['clang!=0', {
              'CLANG_CXX_LANGUAGE_STANDARD': 'c++17',  # -std=c++17

              # Don't link in libarclite_macosx.a, see http://crbug.com/156530.
              'CLANG_LINK_OBJC_RUNTIME': 'NO',  # No -fobjc-link-runtime

              # CLANG_WARN_OBJC_MISSING_PROPERTY_SYNTHESIS maps to
              # -Wobjc-missing-property-synthesis
              'CLANG_WARN_OBJC_MISSING_PROPERTY_SYNTHESIS': 'YES',

              'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
              'WARNING_CFLAGS': [
                '-Wexit-time-destructors',
                '-Wextra-semi',
                '-Wheader-hygiene',
                '-Wimplicit-fallthrough',
                '-Wno-selector-type-mismatch',
                '-Wsign-compare',
                '-Wstring-conversion',
              ],
            }, {  # else: clang==0
              'GCC_VERSION': '4.2',
            }],

            ['mac_sdk!=""', {
              'SDKROOT': 'macosx<(mac_sdk)',  # -isysroot
            }],

            ['mac_deployment_target!=""', {
              # MACOSX_DEPLOYMENT_TARGET maps to -mmacosx-version-min
              'MACOSX_DEPLOYMENT_TARGET': '<(mac_deployment_target)',
            }],

            ['target_arch=="ia32"', {
              'ARCHS': [
                'i386',
              ],
            }],
            ['target_arch=="x64"', {
              'ARCHS': [
                'x86_64',
              ],
            }],
          ],

          'target_conditions': [
            ['_type=="executable"', {
              'OTHER_LDFLAGS': [
                '-Wl,-pie',  # Position-independent executable (MH_PIE)
              ],
            }],
          ],
        },
      }],

      ['OS=="linux"', {
        'cflags': [
          '-fPIC',
          '-fno-exceptions',
          '-fno-strict-aliasing',  # See http://crbug.com/32204
          '-fstack-protector-all',  # Implies -fstack-protector
          '-fvisibility=hidden',
          '-g',
          '-pipe',
          '-pthread',
          '-Wall',
          '-Werror',
          '-Wextra',
          '-Wno-unused-parameter',
          '-Wno-missing-field-initializers',
        ],
        'cflags_c': [
          '-std=c17',
        ],
        'cflags_cc': [
          '-fno-rtti',
          '-fvisibility-inlines-hidden',
          '-std=c++17',
        ],
        'defines': [
          '_FILE_OFFSET_BITS=64',
        ],
        'ldflags': [
          '-fPIC',
          '-pthread',
          '-Wl,--as-needed',
          '-Wl,-z,noexecstack',
        ],

        'conditions': [
          ['clang!=0', {
            'cflags': [
              '-Wexit-time-destructors',
              '-Wextra-semi',
              '-Wheader-hygiene',
              '-Wimplicit-fallthrough',
              '-Wsign-compare',
              '-Wstring-conversion',
            ],
          }, {  # else: clang==0
            'conditions': [
              ['target_arch=="ia32"', {
                'cflags': [
                  '-msse2',
                  '-mfpmath=sse',
                ],
              }],
            ],
          }],

          ['target_arch=="ia32"', {
            'cflags': [
              '-m32',
            ],
            'ldflags': [
              '-m32',
            ],
          }],
          ['target_arch=="x64"', {
            'cflags': [
              '-m64',
            ],
            'ldflags': [
              '-m64',
            ],
          }],
        ],

        'target_conditions': [
          ['_type=="executable"', {
            'ldflags': [
              '-pie',
            ],
          }],
        ],
      }],

      ['OS=="win"', {
        'msbuild_configuration_attributes': {
          'CharacterSet': 'Unicode',
        },
        'msbuild_settings': {
          'ClCompile': {
            'WarningLevel': 'Level4',
            'TreatWarningAsError': 'true',
            'SDLCheck': 'true',
            'ExceptionHandling': 'false',
            'RuntimeTypeInfo': 'false',
            'LanguageStandard': 'stdcpp17',
            'AdditionalOptions': '/Qspectre',
            'ForcedIncludeFiles': [
              'sdkddkver.h',
            ],
          },
          'Link': {
            'TreatLinkerWarningAsErrors': 'true',
          },
          'Lib': {
            'TreatLibWarningAsErrors': 'true',
          },
          'ResourceCompile': {
            'Culture': '0x0411',
          },
          'Midl': {
            'WarnAsError': 'true',
            'ValidateAllParameters': 'true',
          },
        },
        'defines': [
          '_HAS_EXCEPTIONS=0',
          'WIN32_LEAN_AND_MEAN',
          'NOMINMAX',
          '_ATL_NO_HOSTING',
          '_ATL_CSTRING_EXPLICIT_CONSTRUCTORS',
          '_WTL_NO_CSTRING',
          '_CSTRING_NS_=ATL',
        ],

        'conditions': [
          ['target_arch=="ia32"', {
            'msbuild_settings': {
              'ClCompile': {
                'CreateHotpatchableImage': 'true',
              },
              'Link': {
                'CreateHotPatchableImage': 'Enabled',
                'AdditionalManifestDependencies': [
                  'type=\'win32\' name=\'Microsoft.Windows.Common-Controls\' version=\'6.0.0.0\' processorArchitecture=\'x86\' publicKeyToken=\'6595b64144ccf1df\' language=\'*\'',
                ],
              },
            },
          }],
          ['target_arch=="x64"', {
            'msvs_configuration_platform': 'x64',
            'msbuild_settings': {
              'Link': {
                'AdditionalManifestDependencies': [
                  'type=\'win32\' name=\'Microsoft.Windows.Common-Controls\' version=\'6.0.0.0\' processorArchitecture=\'amd64\' publicKeyToken=\'6595b64144ccf1df\' language=\'*\'',
                ],
              },
            },
          }],
        ],
      }],

    ],

    'default_configuration': 'Debug',
    'configurations': {
      'Release': {
        'defines': [
          'NDEBUG',
        ],
        'conditions': [

          ['OS=="mac"', {
            'xcode_settings': {
              'DEAD_CODE_STRIPPING': 'YES',  # -Wl,-dead_strip
              'DEBUG_INFORMATION_FORMAT': 'dwarf-with-dsym',
              'GCC_OPTIMIZATION_LEVEL': '3',

              'target_conditions': [
                ['_type=="executable" or _type=="shared_library" or \
                  _type=="loadable_module"', {
                  'DEPLOYMENT_POSTPROCESSING': 'YES',
                  'STRIP_INSTALLED_PRODUCT': 'YES',
                }],
                ['_type=="shared_library" or _type=="loadable_module"', {
                  'STRIPFLAGS': '-x',
                }],
              ],
            },
          }],

          ['OS=="linux"', {
            'cflags': [
              '-O3',
              '-fdata-sections',
              '-ffunction-sections',
            ],
            'ldflags': [
              '-Wl,-O1',
              '-Wl,--gc-sections',
            ],

            'conditions': [
              ['clang==0', {
                'cflags': [
                  '-fno-ident',
                ],
              }],
            ],
          }],

          ['OS=="win"', {
            'msbuild_configuration_attributes': {
              'LinkIncremental': 'false',
            },
            'msbuild_settings': {
              'ClCompile': {
                'Optimization': 'MaxSpeed',
                'IntrinsicFunctions': 'true',
                'ControlFlowGuard': 'Guard',
                'FunctionLevelLinking': 'true',
                'conditions': [
                  ['vc_runtime=="dynamic"', {
                    'RuntimeLibrary': 'MultiThreadedDLL',
                  }],
                  ['vc_runtime=="static"', {
                    'RuntimeLibrary': 'MultiThreaded',
                  }],
                ],
              },
              'Link': {
                'OptimizeReferences': 'true',
                'EnableCOMDATFolding': 'true',
              },
            },
          }],

        ],
      },
      'Debug': {
        'conditions': [

          ['OS=="mac"', {
            'xcode_settings': {
              'COPY_PHASE_STRIP': 'NO',
              'DEBUG_INFORMATION_FORMAT': 'dwarf',
              'GCC_OPTIMIZATION_LEVEL': '0',
            },
          }],

          ['OS=="linux"', {
            'cflags': [
              '-O0',
              '-ggdb',
            ],
          }],

          ['OS=="win"', {
            'defines': [
              '_DEBUG',
            ],
            'msbuild_configuration_attributes': {
              'LinkIncremental': 'true',
            },
            'msbuild_settings': {
              'ClCompile': {
                'Optimization': 'Disabled',
                'conditions': [
                  ['vc_runtime=="dynamic"', {
                    'RuntimeLibrary': 'MultiThreadedDebugDLL',
                  }],
                  ['vc_runtime=="static"', {
                    'RuntimeLibrary': 'MultiThreadedDebug',
                  }],
                ],
              },
            },
          }],

        ],
      },
    },
  },

  'conditions': [
    ['OS=="mac"', {
      'xcode_settings': {
        'SYMROOT': '<(DEPTH)/xcodebuild',
      },
    }],
  ],
}
