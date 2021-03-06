{
  'includes': [
    '../build/common.gypi',
  ],

  'targets': [
    {
      'target_name': 'shinobu',
      'type': 'executable',

      'include_dirs': [
        '.',
        '../third_party/wtl/Include',
        '../third_party/wil/include',
      ],
      'libraries': [
        'dbghelp.lib',
        'pdh.lib',
        'powrprof.lib',
        'version.lib',
        'winmm.lib',
        'ws2_32.lib',
      ],
      'msbuild_settings': {
        'Midl': {
          'OutputDirectory': 'gen',
          'HeaderFileName': '%(Filename).h',
        },
        'PostBuildEvent': {
          'Command': 'signtool sign /fd SHA256 "$(TargetPath)"',
        },
      },

      'sources': [
        'app/constants.cpp',
        'app/constants.h',
        'app/shinobu.cpp',
        'app/shinobu.h',
        'common/policy_config.idl',
        'gen/policy_config.h',
        'module/application.h',
        'module/application_impl.cpp',
        'module/application_impl.h',
        'module/clipbrd/clipboard.cpp',
        'module/clipbrd/clipboard.h',
        'module/clipbrd/clipboard_monitor.cpp',
        'module/clipbrd/clipboard_monitor.h',
        'module/clipbrd/clipbrd_constants.cpp',
        'module/clipbrd/clipbrd_constants.h',
        'module/clipbrd/monitor_clipboard_page.cpp',
        'module/clipbrd/monitor_clipboard_page.h',
        'module/clipbrd/ring_buffer.h',
        'module/module.h',
        'module/perfmon/perfmon_constants.cpp',
        'module/perfmon/perfmon_constants.h',
        'module/perfmon/performance_counter.cpp',
        'module/perfmon/performance_counter.h',
        'module/perfmon/performance_monitor.cpp',
        'module/perfmon/performance_monitor.h',
        'module/perfmon/performance_monitor_page.cpp',
        'module/perfmon/performance_monitor_page.h',
        'module/preferences.h',
        'module/profman/profile_manager.cpp',
        'module/profman/profile_manager.h',
        'module/property_dialog.h',
        'module/registry_preferences.cpp',
        'module/registry_preferences.h',
        'module/snap/manage_windows_page.cpp',
        'module/snap/manage_windows_page.h',
        'module/snap/snap_constants.cpp',
        'module/snap/snap_constants.h',
        'module/snap/window_manager.cpp',
        'module/snap/window_manager.h',
        'module/wol/wakeup_manager.cpp',
        'module/wol/wakeup_manager.h',
        'module/wol/wakeup_targets_page.cpp',
        'module/wol/wakeup_targets_page.h',
        'res/compatibility.manifest',
        'res/resource.h',
        'res/resource.rc',
        'res/shinobu.ico',
        'ui/hot_key_ctrl.cpp',
        'ui/hot_key_ctrl.h',
        'ui/main_frame.cpp',
        'ui/main_frame.h',
        'ui/property_dialog_impl.h',
      ],
    },
  ],
}
