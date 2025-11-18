# Helper function to add demo executables
function(add_ito_demo DEMO_NAME)
    # Create executable from <name>_demo.cpp
    add_executable(ito_demo_${DEMO_NAME} ${DEMO_NAME}_demo.cpp)
    
    # Link to the header-only library
    target_link_libraries(ito_demo_${DEMO_NAME} PRIVATE ito_engine)
    
    # Optional: Set output directory for organized binaries
    set_target_properties(ito_demo_${DEMO_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endfunction()

# Add all demos (one line each!)
add_ito_demo(main)              # main_demo.cpp
add_ito_demo(math)              # math_demo.cpp
add_ito_demo(black_scholes)     # black_scholes_demo.cpp
add_ito_demo(gbm_sim)           # gbm_sim_demo.cpp
add_ito_demo(montecarlo)        # montecarlo_demo.cpp
add_ito_demo(montecarlo_BM)