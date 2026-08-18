BUILD_DEBUG_DIR = builds/debug-ninja
BUILD_RELEASE_DIR = builds/release
PROJECT_NAME = orbital-bass-engine
DEBUG_TARGET = $(PROJECT_NAME)_Standalone

init-debug:
	cmake -S . -B ${BUILD_DEBUG_DIR} -G Ninja -D CMAKE_BUILD_TYPE=Debug

init-release:
	cmake -S . -B builds/release -D CMAKE_BUILD_TYPE=Release

build-debug: init-debug
	cmake --build ${BUILD_DEBUG_DIR} --config Debug --target ${DEBUG_TARGET} --parallel

build-release:
	cmake --build ${BUILD_RELEASE_DIR} --config Release --parallel

run: build-release
	killall ${PROJECT_NAME} || true
	open ${BUILD_RELEASE_DIR}/src/${PROJECT_NAME}_artefacts/Release/Standalone/${PROJECT_NAME}.app

run-debug: build-debug
	killall ${PROJECT_NAME} || true
	open ${BUILD_DEBUG_DIR}/src/${PROJECT_NAME}_artefacts/Debug/Standalone/${PROJECT_NAME}.app

debug: build-debug
	lldb ${BUILD_DEBUG_DIR}/src/${PROJECT_NAME}_artefacts/Debug/Standalone/${PROJECT_NAME}.app/Contents/MacOS/${PROJECT_NAME}

build-impulses:
	bin/BinaryBuilder impulses src/assets/ ImpulseResponseBinary
	python3 bin/extract_impulses.py src/include/ImpulseResponses.h
