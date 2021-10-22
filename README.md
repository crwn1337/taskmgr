# taskmgr
display videos on the task manager

## how to use
1. build (build release and make sure you have the correct platform set)
2. make a folder called "images" in the same location where the dll is
3. you can convert a video to bmp files using [ffmpeg](https://www.ffmpeg.org/) :
`ffmpeg -i <video> -vf scale=<width>x<height>,fps=<fps> -pix_fmt bgr24 %d.bmp`
replacing <video> with the video you want, <width> and <height> with the ones that fit you and <fps> i would mostly recommend using 3.
4. inject the dll into task manager and go to the cpu tab and change the graph to show logical cores