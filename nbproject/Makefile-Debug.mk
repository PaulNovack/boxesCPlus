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
	${OBJECTDIR}/Box.o \
	${OBJECTDIR}/Item.o \
	${OBJECTDIR}/main.o


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
LDLIBSOPTIONS=-lpthread /usr/lib/libmysqlcppconn8.so /usr/lib/libserved.so

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/boxescplus
	${CP} /usr/lib/libmysqlcppconn8.so ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${CP} /usr/lib/libserved.so ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/boxescplus: /usr/lib/libmysqlcppconn8.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/boxescplus: /usr/lib/libserved.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/boxescplus: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/boxescplus ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/Box.o: Box.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include/served -I/usr/local/include/mysql-connector-c++-8.0.29-src/include/ -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Box.o Box.cpp

${OBJECTDIR}/Item.o: Item.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include/served -I/usr/local/include/mysql-connector-c++-8.0.29-src/include/ -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Item.o Item.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include/served -I/usr/local/include/mysql-connector-c++-8.0.29-src/include/ -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} -r ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmysqlcppconn8.so ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libserved.so
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/boxescplus

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
