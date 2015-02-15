#!/bin/bash
function usage {
    >&2 echo "./post-build.bash TARBALL BUILD_TYPE"
    >&2 echo "build types: linux osx"
}

function main {
    if [[ $# -ne 2 ]]; then
        usage
        exit 1
    fi
    local tarball="$1"; shift
    local buildType="$1"; shift
    local remoteDir=/usr/share/nginx/html/downloads/"$buildType"/$(uname -m)
    scp "$tarball" root@lightningmusic.org:"$remoteDir"
}
main "$@"
