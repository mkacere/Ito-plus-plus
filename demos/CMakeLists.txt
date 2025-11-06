# Simple demo (repeate patern for others added!!!!)
add_executable(ito_demo main.cpp "main.cpp")
target_link_libraries(ito_demo PRIVATE ito_engine)

# math demo
add_executable(ito_demo_math math_demo.cpp "math_demo.cpp")
target_link_libraries(ito_demo_math PRIVATE ito_engine)

# Black Scholes demo
add_executable(ito_demo_black_scholes black_scholes_demo.cpp "black_scholes_demo.cpp")
target_link_libraries(ito_demo_black_scholes PRIVATE ito_engine)
