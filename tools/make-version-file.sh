#!/bin/bash
[[ -z "$1" ]] && exit 1
git_tag=$(git describe --tags --abbrev=0)
git_describe=$(git describe --tags)

if [[ $git_tag =~ ^v(0|[1-9][0-9]*)[.](0|[1-9][0-9]*)[.](0|[1-9][0-9]*)(-[.0-9A-Za-z-]+)?([+][.0-9A-Za-z-]+)?$ ]]; then
	VERSION_MAJOR=${BASH_REMATCH[1]}
	VERSION_MINOR=${BASH_REMATCH[2]}
	VERSION_PATCH=${BASH_REMATCH[3]}
	IDENTIFIERS=${BASH_REMATCH[4]:1}	#strip off leading '-'
	METADATA=${BASH_REMATCH[5]:1}		#strip off leading '+'
	if [[ $git_tag != "$git_describe" ]]; then
		if [[ $git_describe =~ (g[0-9a-f]+)$ ]]; then
			GIT_HASH=${BASH_REMATCH[1]}
			METADATA=${METADATA}${GIT_HASH}
		fi
	fi
	VERSION=${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
else
	echo "Unable to get git version string, using fallback version" 1>&2
	# Fallback hardcoded version
	VERSION=0.1.0
fi

# Construct Semantic Version
SEMVER=${VERSION}
if [[ -n $IDENTIFIERS ]]; then
	SEMVER=${SEMVER}-${IDENTIFIERS}
fi
if [[ -n $METADATA ]]; then
	SEMVER=${SEMVER}+${METADATA}
fi

cat << EOF > "$1"
#ifndef VERSION_HPP
#define VERSION_HPP
#define MIST_VERSION "${SEMVER}"
#endif
EOF
