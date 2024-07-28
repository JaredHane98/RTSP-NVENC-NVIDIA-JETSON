#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <gst/controller/gstinterpolationcontrolsource.h>
#include <gst/controller/gstdirectcontrolbinding.h>
#include <gst/gstpad.h>
#include <gio/gio.h>

#include <string.h>
#include <string>
#include <iostream>
#include <cassert>




G_DECLARE_FINAL_TYPE (RTSPFactory, rtsp_factory, RTSP, FACTORY, GstRTSPMediaFactory);

struct _RTSPFactory
{
  GstRTSPOnvifMediaFactory parent;
};

G_DEFINE_TYPE (RTSPFactory, rtsp_factory, GST_TYPE_RTSP_MEDIA_FACTORY);


static void rtsp_factory_init (RTSPFactory * factory)
{}


GstElement* CreateElement(const std::string& element, const std::string& element_name, GstElement* pipe)
{
    GstElement* output;

    if (G_UNLIKELY (!(output = (gst_element_factory_make (element.c_str(), element_name.c_str()))))) 
        return NULL;
    if (G_UNLIKELY (!gst_bin_add (GST_BIN_CAST (pipe), output))) 
    {
        gst_object_unref(G_OBJECT(output));
        return NULL;
    } 
    return output;
}


static GstElement* rtsp_factory_create_element(GstRTSPMediaFactory * factory, const GstRTSPUrl * url)
{
    GstElement *src, *rsdeinterlace, *videoconvert, *nvvidconv, *nvv4l2h264enc, *h264parse, *rtph264pay, *basetransform;
    GstCaps *src_caps, *videoconvert_caps,  *nvvidconv_caps, *transform_caps;

    GstElement *ret  = gst_bin_new (NULL);
    GstElement *pbin = gst_pipeline_new ("JETSON_PIPELINE");


    src = CreateElement("v4l2src", "v4l2src0", pbin);
    rsdeinterlace = CreateElement("rsdeinterlace", "rsdeinterlace0", pbin);
    videoconvert = CreateElement("videoconvert", "videoconvert0", pbin);
    nvvidconv = CreateElement("nvvidconv", "nvvidconv0", pbin);
    nvv4l2h264enc = CreateElement("nvv4l2h264enc", "nvv4l2h264enc0", pbin);
    h264parse = CreateElement("h264parse", "h264parse0", pbin);
    rtph264pay = CreateElement("rtph264pay", "pay0", pbin);


    assert(src && rsdeinterlace && videoconvert && nvvidconv && nvv4l2h264enc && h264parse && rtph264pay);

    // platform and implementation specific implementation specific details

    const int frame_rate = 30;
    const int bitrate = 8000000;

    g_object_set(G_OBJECT(src), "device", "/dev/video1", NULL);
    
    g_object_set(G_OBJECT(nvv4l2h264enc), "bitrate", bitrate, 
                                          "control-rate", 0,
                                          "preset-level", 3,
                                          "profile", 2,
                                          "insert-vui", true,
                                          "maxperf-enable", true, NULL);

    g_object_set(G_OBJECT(rtph264pay), "pt", 96, NULL);

    src_caps = gst_caps_new_simple ("video/x-raw", 
                                    "format", G_TYPE_STRING, "GRAY8", 
                                    "width", G_TYPE_INT, 1280,
                                    "height", G_TYPE_INT, 720,
                                    "interlace-mode", G_TYPE_STRING, "progressive",
                                    "framerate", GST_TYPE_FRACTION, frame_rate, 1, NULL);
    
    videoconvert_caps = gst_caps_from_string("video/x-raw, format=(string)I420, width=(int)2560, height=(int)720, framerate=(fraction)30/1");

    nvvidconv_caps = gst_caps_from_string("video/x-raw(memory:NVMM), format=(string)I420, width=(int)2560, height=(int)720, framerate=(fraction)30/1");

    // link the elements
    gst_element_link_filtered(src, rsdeinterlace, src_caps);
    gst_element_link(rsdeinterlace, videoconvert);
    gst_element_link_filtered(videoconvert, nvvidconv, videoconvert_caps);
    gst_element_link_filtered(nvvidconv, nvv4l2h264enc, nvvidconv_caps);
    gst_element_link(nvvidconv, nvv4l2h264enc);
    gst_element_link(nvv4l2h264enc, h264parse);
    gst_element_link(h264parse, rtph264pay);

    gst_caps_unref(src_caps);
    gst_caps_unref(videoconvert_caps);
    gst_caps_unref(nvvidconv_caps);

    gst_bin_add (GST_BIN (ret), pbin);

    return ret;
}


static void rtsp_factory_class_init (RTSPFactoryClass * klass)
{
    GstRTSPMediaFactoryClass *mf_class = GST_RTSP_MEDIA_FACTORY_CLASS (klass);
    mf_class->create_element = rtsp_factory_create_element;
}

static GstRTSPMediaFactory *rtsp_factory_new (void)
{
    GstRTSPMediaFactory *result = GST_RTSP_MEDIA_FACTORY (g_object_new (rtsp_factory_get_type (), NULL));
    return result;
}
static gboolean timeout (GstRTSPServer * server)
{
    GstRTSPSessionPool *pool;

    pool = gst_rtsp_server_get_session_pool (server);
    gst_rtsp_session_pool_cleanup (pool);
    g_object_unref (pool);

    return TRUE;
}



typedef struct
{
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;
}RTSP_Server;



static void create_rtsp_server()
{
    RTSP_Server rtsp_server;

    gst_init(NULL, NULL);

    rtsp_server.loop = g_main_loop_new(NULL, FALSE);
    rtsp_server.server = gst_rtsp_onvif_server_new (); // creating server instance
    rtsp_server.mounts = gst_rtsp_server_get_mount_points(rtsp_server.server); // Get the mount points         
    rtsp_server.factory = rtsp_factory_new (); // attach the factory 

    gst_rtsp_media_factory_set_media_gtype (rtsp_server.factory, GST_TYPE_RTSP_ONVIF_MEDIA); // media type 
    gst_rtsp_media_factory_set_shared (rtsp_server.factory, TRUE); 
    gst_rtsp_media_factory_set_protocols(rtsp_server.factory, GST_RTSP_LOWER_TRANS_UDP); // transmission protocol


    gst_rtsp_mount_points_add_factory (rtsp_server.mounts, "/test", rtsp_server.factory);    // attach the test factory to the /test url 

    g_object_unref (rtsp_server.mounts);

    assert(gst_rtsp_server_attach (rtsp_server.server, NULL) != 0);

    g_timeout_add_seconds (2, (GSourceFunc) timeout, rtsp_server.server);
    g_print ("stream ready at rtsp://IP_ADDRESS:8554/test\n");
    g_main_loop_run (rtsp_server.loop); 
}


int main()
{
    create_rtsp_server();
    return 0;
}

