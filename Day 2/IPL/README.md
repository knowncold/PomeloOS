# IPL

为了以后的开发，不再通过nask来做整个磁盘映像，nask只用来做512字节的启动区输出`ipl.bin`（同时生成的列表文件`ipl.lst`只是一个文本文件），然后通过`edimg.exe`这个工具，先读入一个空白的磁盘映像然后在开头写入`ipl.bin`，生成目标的`os.img`。
