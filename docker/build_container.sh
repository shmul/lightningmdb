#!/bin/bash

#
# Usage: docker/build_container.sh lua5.2
#

platform=$1
if [ -z "$platform" ] ; then
	echo "Usage: $0 <platform_name> [docker build params]"
	echo ""
	echo "Example: $0 lua5.2"
	exit 1
fi

shift
# Docker repo name
echo $platform
repo="lightningmdb.$platform"

projectrootrelative="$(dirname $0)/.."
projectroot=$(cd $projectrootrelative && pwd) # Get the absolute path
dockerfile="$projectroot/docker/Dockerfile-$platform"
if [ ! -f "$dockerfile" ] ; then
	echo "Missing docker file: $dockerfile"
	exit 2
fi

docker build --file=$dockerfile --tag=$repo:$platform $@ $projectroot
