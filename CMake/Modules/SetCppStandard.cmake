#
# Module: SetCppStandard
#
# Description:
# Sets the C++ standard for the target and disables
# vendor (LLVM, GNU, Microsoft, etc.) extensions.
#
# Author:
# マギルゥーベルベット (magiruuvelvet)
#

macro (SetCppStandard TargetName CppVer)
    set_property(TARGET ${TargetName} PROPERTY CXX_STANDARD ${CppVer})
    set_property(TARGET ${TargetName} PROPERTY CXX_STANDARD_REQUIRED ON)
    set_property(TARGET ${TargetName} PROPERTY CXX_EXTENSIONS OFF)
endmacro()
