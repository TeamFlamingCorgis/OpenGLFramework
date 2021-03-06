# 
# Internal file for GetGitRevisionDescription.cmake
#
# Requires CMake 2.6 or newer (uses the 'function' command)
#
# Original Author:
# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryand.net>
# http://academic.cleardefinition.com
# Iowa State University HCI Graduate Program/VRAC
#
# Copyright Iowa State University 2009-2010.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(HEAD_HASH)

file(READ "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/cmake-build-debug/external/glbinding-2.1.1/CMakeFiles/git-data/HEAD" HEAD_CONTENTS LIMIT 1024)

string(STRIP "${HEAD_CONTENTS}" HEAD_CONTENTS)
if(HEAD_CONTENTS MATCHES "ref")
    # named branch
    string(REPLACE "ref: " "" HEAD_REF "${HEAD_CONTENTS}")
    if(EXISTS "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/.git/${HEAD_REF}")
        configure_file("/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/.git/${HEAD_REF}" "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/cmake-build-debug/external/glbinding-2.1.1/CMakeFiles/git-data/head-ref" COPYONLY)
    elseif(EXISTS "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/.git/logs/${HEAD_REF}")
        configure_file("/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/.git/logs/${HEAD_REF}" "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/cmake-build-debug/external/glbinding-2.1.1/CMakeFiles/git-data/head-ref" COPYONLY)
        set(HEAD_HASH "${HEAD_REF}")
    endif()
else()
    # detached HEAD
    configure_file("/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/.git/HEAD" "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/cmake-build-debug/external/glbinding-2.1.1/CMakeFiles/git-data/head-ref" COPYONLY)
endif()

if(NOT HEAD_HASH)
    file(READ "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/cmake-build-debug/external/glbinding-2.1.1/CMakeFiles/git-data/head-ref" HEAD_HASH LIMIT 1024)
    string(STRIP "${HEAD_HASH}" HEAD_HASH)
endif()
