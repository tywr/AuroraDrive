BUILD_DIR = builds
PROJECT_NAME = AuroraDrive

build:
	cmake --build ${BUILD_DIR} --config Debug

run: build
	killall ${PROJECT_NAME} || true
	open ${BUILD_DIR}/src/${PROJECT_NAME}_artefacts/Debug/Standalone/${PROJECT_NAME}.app

debug: build
	lldb builds/src/${PROJECT_NAME}_artefacts/Debug/Standalone/${PROJECT_NAME}.app/Contents/MacOS/${PROJECT_NAME}
