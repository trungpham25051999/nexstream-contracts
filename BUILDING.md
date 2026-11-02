# Building nexstream-contracts

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

Sanitizer configuration:

```sh
cmake -S . -B build-sanitize -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build-sanitize --parallel 2
ctest --test-dir build-sanitize --output-on-failure
```

If CMake cannot find Protobuf on Debian/Ubuntu, install `protobuf-compiler` and
`libprotobuf-dev`. Generated sources are intentionally produced in the build tree.
