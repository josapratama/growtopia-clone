git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

cmake ..
cmake --build . --config Release

cmake .. -DCMAKE_TOOLCHAIN_FILE="D:/coding/bu-aini/growtopia/backend/vcpkg/scripts/buildsystems/vcpkg.cmake"

cmake --build . --config Debug