# Readme
Set of Labview-api to spead up parse PCM video.

## deinterleaving
API to Stairs-like shifting bit arrays

## Emphasis
Digital filter to support emphasis PCM mode

## FFmpegVideoReader
Set of functions to read and write media using FFMPEG

To build this, you need download dev kit and shared libraries set from 
[there](https://ffmpeg.zeranoe.com/builds/) and extract it to folder ffmpeg-dist. 
```
ffmpeg-dist
├───bin
├───doc
├───examples
├───include
│   ├───libavcodec
│   ├───libavdevice
│   ├───libavfilter
|   ├───libavformat
│   ├───libavutil
│   ├───libpostproc
│   ├───libswresample
|   └───libswscale
├───lib
└───presets
```

## pcm_utils
Set of parsing functions and P/Q calculations

## MaxParcer
Alternative PCM parser, writen from scratch by [@Fagear](https://www.youtube.com/channel/UCuPCSiMralpydef0BYTmKLQ)

## PlayerWriter
Audio player for labview, based on [portaudio](http://www.portaudio.com/)

## bitmap
Library to read .bmp files, used in development

## tests
Some tests, used in development process
