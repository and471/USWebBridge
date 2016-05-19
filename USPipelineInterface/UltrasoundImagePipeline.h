#ifndef ULTRASOUNDIMAGEPIPELINE_H
#define ULTRASOUNDIMAGEPIPELINE_H

#include "UltrasoundController.h"
#include "PatientMetadata.h"
#include "FrameSource.h"
#include <functional>

class UltrasoundController; // forward declaration

class UltrasoundImagePipeline
{
public:
    virtual ~UltrasoundImagePipeline() {};

    virtual void start()=0;
    virtual void stop()=0;

    virtual void onSetSlice(int slice)=0;
    virtual void setOnNewPatientMetadataCallback(std::function<void(PatientMetadata)> cb) =0;
    virtual void onNewPatientMetadata(PatientMetadata patient) =0;

    virtual void setQuantizer(int quantizer) =0;
    virtual void setFPS(double fps) =0;
    virtual double getFPS() =0;
    virtual void setFrameSource(FrameSource* frame_source) =0;

    virtual int getPort() =0;

protected:
    UltrasoundController* controller;
    FrameSource* frame_source;

    std::function<void(PatientMetadata)> onNewPatientMetadataCallback;

    virtual void onFrame(Frame* frame) =0;
    virtual void onNSlicesChanged(int nSlices) =0;
};

#endif // ULTRASOUNDIMAGEPIPELINE_H
