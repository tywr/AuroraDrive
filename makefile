BUILD_DIR = builds
PROJECT_NAME = AuroraDrive

run:
	cmake --build ${BUILD_DIR} --config Debug
	killall ${PROJECT_NAME} || true
	open ${BUILD_DIR}/src/${PROJECT_NAME}_artefacts/Standalone/${PROJECT_NAME}.app

