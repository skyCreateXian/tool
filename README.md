Base:
  Opencv 3.2 
  Ubuntu 16.04
  Caffe
  CMake
  
include:
  pthread
  sys/shm.h
  sys/types.h

Describe:
  This project is based on opencv3.2 and Caffe v1.0.

  Inspection process:

  1. We try to cut the image equally. T * T uses multi process and shared memory mechanism for parallel detection.

  2. The detection method uses Hal feature to extract image features.

  3. Classify the two features to find the true features.



Example:

  1. You need to use cmake to compile the project.

  2. Enter the project directory.

  3. Command:

    ./bin/tool number

  The following explains number in the command:

    0 Olny display source image.

    1 Display every cutting part.

    2 Display Haar detection in every part.

    3 Display Haar detection and caffe classification in every part.

    4 Show the effect of Haar detection integration in source image.

    5 Show the effect of Haar detection and caffe classification integration in source image ----- Default mode.

    6 Show 2 and 4 effects at the same time.

    7 Show 3 and 5 effects at the same time.

    8 Effect is same with 5  ----- Extend.
    
    Other numbers are treated as default

