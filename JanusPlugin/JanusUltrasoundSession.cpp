#include <cstdio>
#include <functional>
#include <jansson.h>

#include <json.hpp>
using json = nlohmann::json;

#include <USPipelineInterface/FrameSource.h>
#include <USPipelineInterface/UltrasoundImagePipeline.h>
#include <USPipelineInterface/UltrasoundController.h>

#include "JanusUltrasoundSession.h"

using namespace std::placeholders;


void JanusUltrasoundSession::setPipeline(UltrasoundImagePipeline *pipeline) {
    this->pipeline = pipeline;
    pipeline->setOnNewPatientMetadataCallback(
        std::bind(&UltrasoundController::onNewPatientMetadata, this, std::placeholders::_1)
    );
}

void JanusUltrasoundSession::start() {
    if (!pipeline_started) {
        pipeline->start();
        pipeline_started = true;
    }
}

void JanusUltrasoundSession::stop() {
    pipeline->stop();
}

void JanusUltrasoundSession::onNewPatientMetadata(PatientMetadata patient) {
    json obj;
    obj["name"] = patient.name;

    sendMethod(obj, METHOD_NEW_PATIENT_METADATA);
}

void JanusUltrasoundSession::onNSlicesChanged(int nSlices) {
    json obj;
    obj["nSlices"] = nSlices;

    this->sendMethod(obj, METHOD_N_SLICES_CHANGED);
}

void JanusUltrasoundSession::sendMethod(json data, std::string method) {
    json obj;
    obj["method"] = method;
    obj["data"] = data;
    sendData(obj);
}

void JanusUltrasoundSession::sendData(json obj) {
    std::string str = obj.dump();
    char* s = str.c_str();
    gateway->relay_data(handle, s, strlen(s));
}


void JanusUltrasoundSession::onDataReceived(char* msg) {
    json obj = json::parse(msg);

    if (obj["method"] == "SET_SLICE") {
        int slice = obj["data"]["slice"];
        onSetSlice(slice);
    }

    if (obj["method"] == "QUANTIZER") {
        int quantizer = obj["data"]["quantizer"];
        pipeline->setQuantizer(quantizer);
    }

    if (obj["method"] == "FRAMERATE") {
        double fps = obj["data"]["fps"];
        pipeline->setFPS(fps);
    }
}

int JanusUltrasoundSession::getPort() {
    return pipeline->getPort();
}

void JanusUltrasoundSession::onSetSlice(int slice) {
    onSetSliceCallback(slice);
}

void JanusUltrasoundSession::setOnSetSliceCallback(std::function<void(int)> cb) {
    this->onSetSliceCallback = cb;
}

void JanusUltrasoundSession::tearDownPeerConnection() {
    json event;
    event["ultrasound"] = "event";
    event["result"]["status"] = "stopped";

    std::string event_str = event.dump();
    gateway->push_event(handle, &janus_ultrasound_plugin, NULL, event_str.c_str(), NULL, NULL);
    gateway->close_pc(handle);
}
