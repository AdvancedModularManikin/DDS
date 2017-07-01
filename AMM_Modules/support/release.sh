# This sets some environment variables needed for compiling.
# Make sure you update to your BioGears root and OpenSplice release.com file.
#
# `source release.sh`
#

if [ "${SPLICE_ORB:=}" = "" ]; then
    SPLICE_ORB=DDS_OpenFusion_2
    export SPLICE_ORB
fi

if [ "${SPLICE_JDK:=}" = "" ]; then
    SPLICE_JDK=jdk
    export SPLICE_JDK
fi

BIOGEARS_ROOT=/usr/local/physiology
export BIOGEARS_ROOT

source /usr/local/opensplice/install/HDE/armv6l.linux/release.com




