add_executable(Benchmark main.cpp)

# Compiler optimization flags
if (MSVC)
    target_compile_options(Benchmark PRIVATE /O2 /Ot)
else()
    target_compile_options(Benchmark PRIVATE -O3 -march=native -flto)
endif()

target_link_libraries(Benchmark PRIVATE FECS)
