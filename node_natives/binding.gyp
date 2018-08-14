{
  "targets": [
    {
      "target_name": "addon",
      'sources' : [  "<!@(node -p \"require('fs').readdirSync('../code/engine/gkNodeAPI/').map(f=>'../code/engine/gkNodeAPI/'+f).join(' ')\")" ],
      "include_dirs" : ["<!(node -e \"require('nan')\")","../code/engine/gkCommon","../code/engine/gkCommon","../code/engine/gkCommon/serializer","../code/engine/gkCommon/Mathlib","../code/thirdparty","../code/thirdparty/rapidxml"],
      'conditions': [
        [
          'OS=="win"', {   
              "libraries": [ "winmm.lib" ],
              'msvs_settings': {
                'VCCLCompilerTool': {
                  'EnableEnhancedInstructionSet': 2,
                  'FloatingPointModel': 2,
                },
                'VCLinkerTool': {
                  'LinkTimeCodeGeneration': 1,
                  'OptimizeReferences': 2,
                  'EnableCOMDATFolding': 2,
                  'LinkIncremental': 1,       
                }
              }          
            },
        'OS=="mac"', {
          'xcode_settings': {
        'OTHER_CFLAGS': [
              '-msse2',
              '-Ofast'
            ],
          },

        }
        ]
      ]
    }
  ]
}
