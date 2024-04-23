set(Test_Labels fapi p5)
set(_fapi_p5_messages "param_request")

foreach (fapi_p5_message IN LISTS _fapi_p5_messages)
    add_executable(nr_fapi_${fapi_p5_message}_test nr_fapi_${fapi_p5_message}_test.c)
    target_link_libraries(nr_fapi_${fapi_p5_message}_test PUBLIC nr_fapi_p5)
    target_link_libraries(nr_fapi_${fapi_p5_message}_test PRIVATE pthread UTIL ${T_LIB} minimal_lib)
    add_dependencies(tests nr_fapi_${fapi_p5_message}_test)

    add_test(nr_fapi_${fapi_p5_message}_test nr_fapi_${fapi_p5_message}_test)
    set_tests_properties(nr_fapi_${fapi_p5_message}_test PROPERTIES LABELS "${Test_Labels}")
endforeach ()
