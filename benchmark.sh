#!/bin/bash

DATA_SIZE="125"
DATA_DIR="/Users/lxs/workspace/MonitorPlacement/3rdParty/Random3Dcity/data-"${DATA_SIZE}"/"
WORK_DIR="/Users/lxs/workspace/MonitorPlacement/"
CAMERA_COUNT="40"

#rm -rf ${DATA_DIR}
#mkdir ${DATA_DIR}
#python /Users/lxs/workspace/MonitorPlacement/3rdParty/Random3Dcity/randomiseCity.py -o ${DATA_DIR}"file.xml" -n $DATA_SIZE -s 1
#python /Users/lxs/workspace/MonitorPlacement/3rdParty/Random3Dcity/generateCityGML.py -i ${DATA_DIR}"file.xml" -o ${DATA_DIR} -s 1
#
#cp ${DATA_DIR}"LOD3_1.gml" ${WORK_DIR}"data/"
#cp ${DATA_DIR}"Road-LOD0.gml" ${WORK_DIR}"data/"

echo ${DATA_SIZE}"-(${CAMERA_COUNT}): " >> ${WORK_DIR}"coverage-rate"

for (( i = 0; i < 10; i++ )); do
	./unittest ${CAMERA_COUNT}
done
