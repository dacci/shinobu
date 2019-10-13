{
  'includes': [
    'build/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'shinobu',
      'type': 'none',
      'dependencies': [
        'shinobu/shinobu.gyp:shinobu',
      ],
    },
  ],
}
