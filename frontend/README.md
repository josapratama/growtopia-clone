# Growtopia Desktop Client (C++)

## Prasyarat
1. Install vcpkg dependencies:
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install glfw3 glad imgui[opengl3-binding,glfw-binding] nlohmann-json cpprestsdk sfml
```

2. Build Game Client:
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="./vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
```

3. Jalankan `build/Release/GrowtopiaApp.exe` (atau jalankan Web Client di folder `web/`).