#!/bin/bash

#
# Usage: docker/run_tests_in_container.sh lua5.2
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
repo=lightningmdb

projectrootrelative="$(dirname $0)/.."
projectroot=$(cd $projectrootrelative && pwd) # Get the absolute path
dockerfile="$projectroot/docker/Dockerfile-$platform"
if [ ! -f "$dockerfile" ] ; then
	echo "Missing docker file: $dockerfile"
	exit 2
fi

containername="lightningmdb_test_$platform"
docker run --name=$containername $repo:$platform
retval=$?
docker rm $containername

exit $retval
