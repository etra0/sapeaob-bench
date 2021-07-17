# CMakeList.txt : CMake project for sapeaob-benchmark, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (
	sapeaob-benchmark
	"sapeaob-benchmark.cpp"
)

target_link_libraries(
	sapeaob-benchmark
	PUBLIC
	benchmark::benchmark
	sapeaob
	otis
	ModUtils
)

# TODO: Add tests and install targets if needed.
