git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg install glfw3 glad imgui[opengl3-binding,glfw-binding]

cmake .. -DCMAKE_TOOLCHAIN_FILE=D:/coding/bu-aini/growtopia/frontend/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake --build . --config Release

.\vcpkg install nlohmann-json cpprestsdk sfml