# RTSP NVENC NVIDIA JETSON STREAM


The following repository showcases using hardware accelerated video encode on Nvidia Jetson using Gstreamer. The camera parameters are **very** application specific and must be changed if the setup is different.



## How to use

- **ONLY** If you have Nvidia Jetson with Gstreamer 1.14.5 and a RealSense D435

- [INSTALL MY PLUGIN AT](https://github.com/JaredHane98/D435-Y8I-Gstreamer-Plugin)

- clone the repository

- navigate to the build directory

- cmake && make

- launch with ./RTSP_SERVER_EXAMPLE 

- on another system with VLC media player open the Network with the stream URL

- EG: rtsp://192.168.68.61:8554/test




https://github.com/user-attachments/assets/a432ba64-dbbd-4b52-a0dc-fc0d057bf301

