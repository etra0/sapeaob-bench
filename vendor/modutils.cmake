cmake_minimum_required(VERSION 3.8)

add_library(
	ModUtils 
	STATIC
	ModUtils/Patterns.cpp
)

target_include_directories(ModUtils INTERFACE ModUtils/)