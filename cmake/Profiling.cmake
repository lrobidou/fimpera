find_program(VALGRIND_BIN NAMES "valgrind" DOC "Path to valgrind executable")

macro(add_profile_executable_target target_name deps runner arguments output_directory)
  if (VALGRIND_BIN)
    message(STATUS "${BoldGreen}[profiling-exec]${CEnd}: add target '${target_name}'")
    add_custom_target(${target_name}
      COMMAND valgrind --tool=callgrind --callgrind-out-file=${output_directory}/${target_name}.out --log-file=${output_directory}/${target_name}.log --dump-instr=yes --simulate-cache=yes --collect-jumps=yes ${runner} ${arguments}
    )
    add_dependencies(${target_name} ${deps})
  endif()
endmacro()

macro(add_profile_target target_name deps runner output_directory)
  if (VALGRIND_BIN)
    message(STATUS "${BoldGreen}[profiling]${CEnd}: add target '${target_name}'")
    add_custom_target(${target_name}
      COMMAND valgrind --tool=callgrind --callgrind-out-file=${output_directory}/${target_name}.out --log-file=${output_directory}/${target_name}.log  --dump-instr=yes --simulate-cache=yes --collect-jumps=yes --collect-atstart=no --instr-atstart=no ${runner}
    )
    add_dependencies(${target_name} ${deps})
  endif()
endmacro()