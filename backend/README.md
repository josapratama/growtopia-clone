# Growtopia Server Backend

## Prasyarat
1. vcpkg:
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install libpqxx nlohmann-json
```

2. Build Server:
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="./vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
```

3. Jalankan Database PostgreSQL via Docker:
```bash
docker compose up -d
```