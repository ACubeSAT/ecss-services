########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(etl_COMPONENT_NAMES "")
set(etl_FIND_DEPENDENCY_NAMES "")

########### VARIABLES #######################################################################
#############################################################################################
set(etl_PACKAGE_FOLDER_RELEASE "/home/athanasios/.conan2/p/etl1e17537dbc11f/p")
set(etl_BUILD_MODULES_PATHS_RELEASE )


set(etl_INCLUDE_DIRS_RELEASE "${etl_PACKAGE_FOLDER_RELEASE}/include")
set(etl_RES_DIRS_RELEASE )
set(etl_DEFINITIONS_RELEASE )
set(etl_SHARED_LINK_FLAGS_RELEASE )
set(etl_EXE_LINK_FLAGS_RELEASE )
set(etl_OBJECTS_RELEASE )
set(etl_COMPILE_DEFINITIONS_RELEASE )
set(etl_COMPILE_OPTIONS_C_RELEASE )
set(etl_COMPILE_OPTIONS_CXX_RELEASE )
set(etl_LIB_DIRS_RELEASE )
set(etl_BIN_DIRS_RELEASE )
set(etl_LIBRARY_TYPE_RELEASE UNKNOWN)
set(etl_IS_HOST_WINDOWS_RELEASE 0)
set(etl_LIBS_RELEASE )
set(etl_SYSTEM_LIBS_RELEASE )
set(etl_FRAMEWORK_DIRS_RELEASE )
set(etl_FRAMEWORKS_RELEASE )
set(etl_BUILD_DIRS_RELEASE "${etl_PACKAGE_FOLDER_RELEASE}/lib/cmake")
set(etl_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(etl_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${etl_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${etl_COMPILE_OPTIONS_C_RELEASE}>")
set(etl_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${etl_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${etl_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${etl_EXE_LINK_FLAGS_RELEASE}>")


set(etl_COMPONENTS_RELEASE )