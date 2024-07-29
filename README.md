# RTSP NVENC NVIDIA JETSON STREAM


The following repository showcases hardware accelerated video encode on Nvidia Jetson using Gstreamer. The camera parameters are **very** application specific must be changed. 



## How to use

- **ONLY for Nvidia Jetson with Gstreamer 1.14.5 and Realsense D435**
- [INSTALL MY PLUGIN](https://github.com/JaredHane98/D435-Y8I-Gstreamer-Plugin)
- clone
- navigate to the build directory
- cmake && make
- launch with ./RTSP_SERVER_EXAMPLE 
- use VLC to display network stream.
- EG: rtsp://192.168.68.61:8554/test


https://github.com/user-attachments/assets/a432ba64-dbbd-4b52-a0dc-fc0d057bf301

