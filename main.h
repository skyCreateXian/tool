#include"caffe.h"
#include"shareMemory.h"
#include<fstream>
#include<time.h>
#include<iostream>
using namespace std;
using namespace cv;
/*
date:2019/9/20
author:huanglei
fun:Missile Flight Control Algorithm
*/

inline int set_cpu(int i);

//cut image part paras
struct CutImageParts{
	int partsNum; //partsNum*partsNum parts
	GdiCRect GdiArray[MIN_NUM_RENDER]; //Matrix coordinate set
};
