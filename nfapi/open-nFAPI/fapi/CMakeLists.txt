add_library(nr_fapi_p5
        src/nr_fapi_p5.c
        src/nr_fapi_p5_utils.c
)
target_include_directories(nr_fapi_p5 PUBLIC inc)
# Note: Dependencies with NFAPI_LIB NFAPI_USER_LIB only necessary until last FAPI function is moved into new library, to be removed at a later stage
target_link_libraries(nr_fapi_p5 PUBLIC nfapi_common NFAPI_LIB NFAPI_USER_LIB)
