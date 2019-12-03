#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/MIp2-agelumi.o \
	${OBJECTDIR}/MIp2-dnsC.o \
	${OBJECTDIR}/MIp2-lumiC.o \
	${OBJECTDIR}/MIp2-lumiS.o \
	${OBJECTDIR}/MIp2-mi.o \
	${OBJECTDIR}/MIp2-nodelumi.o \
	${OBJECTDIR}/MIp2-p2p.o \
	${OBJECTDIR}/MIp2-t.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/p2_xarxes

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/p2_xarxes: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/p2_xarxes ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/MIp2-agelumi.o: MIp2-agelumi.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MIp2-agelumi.o MIp2-agelumi.c

${OBJECTDIR}/MIp2-dnsC.o: MIp2-dnsC.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MIp2-dnsC.o MIp2-dnsC.c

${OBJECTDIR}/MIp2-lumiC.o: MIp2-lumiC.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MIp2-lumiC.o MIp2-lumiC.c

${OBJECTDIR}/MIp2-lumiS.o: MIp2-lumiS.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MIp2-lumiS.o MIp2-lumiS.c

${OBJECTDIR}/MIp2-mi.o: MIp2-mi.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MIp2-mi.o MIp2-mi.c

${OBJECTDIR}/MIp2-nodelumi.o: MIp2-nodelumi.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MIp2-nodelumi.o MIp2-nodelumi.c

${OBJECTDIR}/MIp2-p2p.o: MIp2-p2p.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MIp2-p2p.o MIp2-p2p.c

${OBJECTDIR}/MIp2-t.o: MIp2-t.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MIp2-t.o MIp2-t.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
