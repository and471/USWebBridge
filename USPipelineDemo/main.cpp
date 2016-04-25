#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <glib.h>

#include <USPipeline/DNLImageSource.h>
#include <USPipeline/DNLFileImageSource.h>
#include <USPipeline/DNLFrameExchange.h>
#include <USPipeline/DNLImageExtractor.h>
#include <USPipeline/UltrasoundImagePipeline.h>
#include <Modules/USStreamingCommon/DNLImage.h>


int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cout << "Not enough arguments"<<std::endl;
        std::cout << "Usage:"<<std::endl;
        std::cout << argv[0]<<" "<< "<folder>"<<std::endl;
        return -1;
    }

    gst_init(&argc, &argv);
    std::string folder = argv[1];

    GMainLoop* loop = g_main_loop_new(NULL, FALSE);

    DNLImageSource* dnl_image_source = new DNLFileImageSource(folder);
    UltrasoundImagePipeline *pipeline = new UltrasoundImagePipeline(loop);
    pipeline->setDNLImageSource(dnl_image_source);

    pipeline->start();
    g_main_loop_run(loop);

    pipeline->stop();
    g_main_loop_unref(loop);

    return 0;
}