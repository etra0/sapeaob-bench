#ifdef _WIN32
#pragma comment(lib, "Shlwapi.lib")
#ifdef _DEBUG
#pragma comment(lib, "benchmarkd.lib")
#else
#pragma comment(lib, "benchmark.lib")
#endif
#endif

#ifndef PROJECT_BASEDIR
#error "Project basedir is not defined"
#else
#define DATA_DIR PROJECT_BASEDIR "/data/output.bin"
#endif

#include <Patterns.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <otis.h>
#include <sapeaob/sapeaob.hpp>
#include <vector>

#define CHECK(result, value)                                                   \
  do {                                                                         \
    if (result == 0)                                                           \
      throw std::exception("Result is 0");                                     \
    std::uint8_t v = *reinterpret_cast<std::uint8_t *>(result);                \
    if (v != value)                                                            \
      throw std::exception("Value is incorrect");                              \
  } while (false)

static void BM_sapeaob_small(benchmark::State &state) {
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});
  sapeaob::pattern<0xC0, 0xFF, 0xEE> p{};
  std::uintptr_t result = 0;
  auto start = values.begin();
  auto size = values.size();

  for (auto _ : state) {
    result = p.scan_match(start, size);
  }

  CHECK(result, 0xC0);
}

static void BM_sapeaob_small_wildcard(benchmark::State &state) {
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});
  sapeaob::pattern<0xDE, sapeaob::ANY, 0xBE, sapeaob::ANY, 0xC0, 0xFF, 0xEE>
      p{};
  std::uintptr_t result = 0;
  auto start = values.begin();
  auto size = values.size();

  for (auto _ : state) {
    result = p.scan_match(start, size);
  }

  CHECK(result, 0xDE);
}

static void BM_otis_small(benchmark::State &state) {
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});
  ScanPattern p("C0 FF EE");
  p.createAOBPatternFromStringPattern();
  LPBYTE result = 0;
  LPBYTE ptr = reinterpret_cast<LPBYTE>(&*(values.begin()));

  for (auto _ : state) {
    result = findAOBPattern(ptr, values.size(), p);
  }

  CHECK(result, 0xC0);
}

static void BM_otis_small_wildcard(benchmark::State &state) {
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});
  ScanPattern p("DE ?? BE ?? C0 FF EE");
  p.createAOBPatternFromStringPattern();
  LPBYTE result = 0;
  LPBYTE ptr = reinterpret_cast<LPBYTE>(&*(values.begin()));

  for (auto _ : state) {
    result = findAOBPattern(ptr, values.size(), p);
  }

  CHECK(result, 0xDE);
}

static void BM_modutils_small(benchmark::State &state) {
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});

  std::uintptr_t start = (std::uintptr_t) & *(values.begin());
  std::uintptr_t end = start + values.size();
  auto pattern = hook::make_range_pattern(start, end, "C0 FF EE");
  std::uintptr_t result = 0;

  for (auto _ : state) {
    result = (std::uintptr_t)pattern.get_first();
    state.PauseTiming();
    pattern.clear();
    state.ResumeTiming();
  }

  CHECK(result, 0xC0);
}

static void BM_modutils_small_wildcard(benchmark::State &state) {
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});
  std::uintptr_t start = (std::uintptr_t) & *(values.begin());
  std::uintptr_t end = start + values.size();
  auto pattern =
      hook::txn::make_range_pattern(start, end, "DE ? BE ? C0 FF EE");
  std::uintptr_t result = 0;

  for (auto _ : state) {
    result = (std::uintptr_t)pattern.get_first();
    state.PauseTiming();
    pattern.clear();
    state.ResumeTiming();
  }

  CHECK(result, 0xDE);
}

static void BM_sapeaob_long(benchmark::State &state) {
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});
  sapeaob::pattern<0xDE, 0xAD, 0xBE, 0xEF, 0xC0, 0xFF, 0xEE, 0xAA, 0xBB, 0xCC,
                   0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                   0x77, 0x88, 0x99, 0xAA>
      p{};
  std::uintptr_t result = 0;
  auto start = values.begin();
  auto size = values.size();

  for (auto _ : state) {
    result = p.scan_match(start, size);
  }

  CHECK(result, 0xDE);
}

static void BM_otis_long(benchmark::State &state) {
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});
  ScanPattern p("DE AD BE EF C0 FF EE AA BB CC DD EE FF 00 11 22 33 44 55 66 "
                "77 88 99 AA");
  p.createAOBPatternFromStringPattern();
  LPBYTE result = 0;
  LPBYTE ptr = reinterpret_cast<LPBYTE>(&*(values.begin()));

  for (auto _ : state) {
    result = findAOBPattern(ptr, values.size(), p);
  }
  CHECK(result, 0xDE);
}

static void BM_modutils_long(benchmark::State &state) {
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});
  std::uintptr_t start = (std::uintptr_t) & *(values.begin());
  std::uintptr_t end = start + values.size();
  auto pattern = hook::txn::make_range_pattern(
      start, end,
      "DE AD BE EF C0 FF EE AA BB CC DD EE FF 00 11 22 33 44 55 66 "
      "77 88 99 AA");
  std::uintptr_t result = 0;

  for (auto _ : state) {
    result = (std::uintptr_t)pattern.get_first();
    state.PauseTiming();
    pattern.clear();
    state.ResumeTiming();
  }

  CHECK(result, 0xDE);
}

static void BM_sapeaob_long_wildcard(benchmark::State &state) {
  using namespace sapeaob;
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});
  pattern<0xDE, ANY, 0xBE, ANY, 0xC0, 0xFF, ANY, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
          ANY, 0x00, 0x11, 0x22, 0x33, ANY, 0x55, 0x66, 0x77, 0x88>
      p{};
  std::uintptr_t result = 0;
  auto start = values.begin();
  auto size = values.size();

  for (auto _ : state) {
    result = p.scan_match(start, size);
  }

  CHECK(result, 0xDE);
}

static void BM_otis_long_wildcard(benchmark::State &state) {
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});
  ScanPattern p(
      "DE ?? BE ?? C0 FF ?? AA BB CC DD EE ?? 00 11 22 33 ?? 55 66 77 88");
  p.createAOBPatternFromStringPattern();
  LPBYTE result = 0;
  LPBYTE ptr = reinterpret_cast<LPBYTE>(&*(values.begin()));

  for (auto _ : state) {
    result = findAOBPattern(ptr, values.size(), p);
  }

  CHECK(result, 0xDE);
}

static void BM_modutils_long_wildcard(benchmark::State &state) {
  std::ifstream data(DATA_DIR, std::ios::binary);
  std::vector<std::uint8_t> values(std::istreambuf_iterator<char>(data), {});
  std::uintptr_t start = (std::uintptr_t) & *(values.begin());
  std::uintptr_t end = start + values.size();
  auto pattern = hook::txn::make_range_pattern(
      start, end,
      "DE ? BE ? C0 FF ? AA BB CC DD EE ? 00 11 22 33 ? 55 66 77 88");
  std::uintptr_t result = 0;

  for (auto _ : state) {
    result = (std::uintptr_t)pattern.get_first();
    state.PauseTiming();
    pattern.clear();
    state.ResumeTiming();
  }

  CHECK(result, 0xDE);
}

BENCHMARK(BM_otis_small);
BENCHMARK(BM_sapeaob_small);
BENCHMARK(BM_modutils_small);

BENCHMARK(BM_otis_small_wildcard);
BENCHMARK(BM_sapeaob_small_wildcard);
BENCHMARK(BM_modutils_small_wildcard);

BENCHMARK(BM_otis_long);
BENCHMARK(BM_sapeaob_long);
BENCHMARK(BM_modutils_long);

BENCHMARK(BM_otis_long_wildcard);
BENCHMARK(BM_sapeaob_long_wildcard);
BENCHMARK(BM_modutils_long_wildcard);

BENCHMARK_MAIN();