#!/bin/bash

APP_FOLDER=/storage/emulated/0/Android/data/com.disertatie.vlad.datacapture/files/DCIM
ANDROID_PLATFORM_TOOLS=/home/vlad/Android/Sdk/platform-tools
DOWNLOAD_FOLDER=/home/vlad/AndroidStudioProjects/DataCapture/fileRetrivalScript/CapturedData

echo "Running pull command ...";

${ANDROID_PLATFORM_TOOLS}/adb pull ${APP_FOLDER} ${DOWNLOAD_FOLDER}/*.
