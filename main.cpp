
#include"main.h"

//some paras
const string haarModePath1 = "model/tank_medium/near.xml";
const string haarModePath2 = "model/tank_medium/medium.xml";
const string haarModePath3 = "model/tank_medium/far.xml";
CvHaarClassifierCascade *cascade1 = (CvHaarClassifierCascade*)cvLoad((char*)haarModePath1.data(), 0, 0, 0);
CvHaarClassifierCascade *cascade2 = (CvHaarClassifierCascade*)cvLoad((char*)haarModePath2.data(), 0, 0, 0);
CvHaarClassifierCascade *cascade3 = (CvHaarClassifierCascade*)cvLoad((char*)haarModePath3.data(), 0, 0, 0);
const string model_file   = "model/tank_medium/deploy.prototxt"; 
const string trained_file = "model/tank_medium/jiangNet_iter_150000.caffemodel";  
const string mean_file    = "model/tank_medium/mean.binaryproto";  
const string label_file   = "model/label.txt";  
//start_time1 = clock();  
Classifier caffeObject(model_file, trained_file, mean_file, label_file); 

/*
0 Olny display source image
1 Display every cutting part
2 Display Haar detection in every part
3 Display Haar detection and caffe classification in every part
4 Show the effect of Haar detection integration in source image
5 Show the effect of Haar detection and caffe classification integration in source image ----- Default mode
6 Show 2 and 4 effects at the same time
7 Show 3 and 5 effects at the same time
8 Effect is same with 5  ----- Extend
 */
int toolNum = 5;

//read paras
string* readParas(){
	ifstream in("paras.txt"); //read paras.txt
	if(!in.is_open()){
		printf("open file fail!\n");
	}
	
	char buf[1024]; //Each line of string in XML
	string *parasValue = new string[7]; //get paras
	
	//paras value
	int lineNum = 0;
   	while(getline(in,parasValue[lineNum]))
    {	
		//printf("%s\n",parasValue[lineNum].c_str());
		lineNum++;
    }
    in.close();
	return parasValue;
}

/*
cut image n*n parts
offset:give up edge beside in photo
over:a
srcEdge:IW
n:cut image n*n parts
*/
CutImageParts cutImageParts(int offset,int over,int srcWidth,int srcHight,int n){
	//init parts array
	CutImageParts imageParts;
	imageParts.partsNum = n;

	//offset detect
	if(offset<0||over<0||offset>=srcWidth||over>=srcWidth||n<1) 
	{
		printf("paras errors!\n");
		return imageParts;
	}

	//begin cutting
	int FW = (srcWidth-offset*2+(n-1)*over)/n;//edgeLength is core:FW*FH
	int FH = (srcHight-offset*2+(n-1)*over)/n;
	int stepW = FW-over;//stepW
	int stepH = FH-over;//stepH
	int rectNum = 0;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			//left = offset + j*step，j is width
			int left = offset+j*stepW;
			//top = offset + i*step,j is hight
			int top = offset +i*stepH;
			int right = left+FW;
			int bottom = top+FH;
			imageParts.GdiArray[rectNum].left = left;
			imageParts.GdiArray[rectNum].top = top;
			imageParts.GdiArray[rectNum].right = right;
			imageParts.GdiArray[rectNum].bottom = bottom;
			rectNum++;
		}
	}
	
	return imageParts;
}

//cut image from srcImage by coord
IplImage* getSampleImage(IplImage * srcImage, GdiCRect rect){
	int height = rect.bottom - rect.top;
	int width = rect.right - rect.left;
	int nChannels = srcImage->nChannels;
	IplImage *temp = cvCreateImage(cvSize(width, height), 8, nChannels);

	//coord start from srcImage rect
	for (int i = rect.top; i < rect.bottom; i++)
	{
		//sample image start y point
		int it = i - rect.top;
		for (int j = rect.left; j < rect.right; j++)
		{
			//srcImage location
			int loc = i*srcImage->widthStep + j*nChannels;
			//sample image start x point
			int jt = j - rect.left;
			//sample image location
			int sampleImageLoc = it*temp->widthStep + jt*nChannels;
			//begin copy image rect pixel to sample docker
			temp->imageData[sampleImageLoc + 0] = srcImage->imageData[loc + 0];
			temp->imageData[sampleImageLoc + 1] = srcImage->imageData[loc + 1];
			temp->imageData[sampleImageLoc + 2] = srcImage->imageData[loc + 2];
		}
	}
	//if resize
	/*
	 IplImage * resizeImage = cvCreateImage(cvSize(20, 20), 8, 1);
	 cvResize(temp, resizeImage);*/
	return temp;
}

//detectPlane
RealRect haarDetect(IplImage* partImage,CvHaarClassifierCascade *cascade)
{
	RealRect haarRect;
	clock_t beginTime = clock();
	printf("begin detect time %d us\n",beginTime);
	haarRect.num = 0;

	//apply rect for return
	CvMemStorage* storage = 0;
	IplImage *frame, *frame_copy = 0;
	if (!cascade)
	{
		return haarRect;
	}
	storage = cvCreateMemStorage(0);

	int nImgWidth = 0;
	int nImgHeight = 0;
	if (!partImage)    // 
	{
		return haarRect;
	}// end if


	if (partImage)
	{
		nImgWidth = partImage->width;
		nImgHeight = partImage->height;

		// detect_and_draw( image );
		double scale = 1.1;
		IplImage* gray = cvCreateImage(cvSize(partImage->width, partImage->height), 8, 1);
		IplImage* small_img = cvCreateImage(cvSize(cvRound(partImage->width / scale),
			cvRound(partImage->height / scale)),
			8, 1
			);
		cvCvtColor(partImage, gray, CV_BGR2GRAY);
		cvResize(gray, small_img, CV_INTER_LINEAR);
		cvEqualizeHist(small_img, small_img);
		cvClearMemStorage(storage);

		int i;

		if (cascade)
		{
			CvSeq* faces = cvHaarDetectObjects(gray, cascade, storage,
				scale, 1, CV_HAAR_DO_CANNY_PRUNING,
				cvSize(32, 32)
				);
			for (i = 0; i < (faces ? faces->total : 0); i++)
			{
				CvRect* r = (CvRect*)cvGetSeqElem(faces, i);
				int radius;
				haarRect.GdiArray[haarRect.num].left = r->x;
				haarRect.GdiArray[haarRect.num].top =  r->y;
				haarRect.GdiArray[haarRect.num].right = (r->x + r->width);
				haarRect.GdiArray[haarRect.num].bottom = (r->y + r->height);
				haarRect.num++;
			}
		}
		}
		return haarRect;
}

RealRect caffeClassification(IplImage* image,RealRect haarRect){
	clock_t beginTime = clock();
	RealRect rect;
	rect.num = 0;
	for (int i = 0; i < haarRect.num; i++)
		{
			GdiCRect aimRect = haarRect.GdiArray[i];
			IplImage *sampleImage = getSampleImage(image,aimRect);
			IplImage *resizeImage = cvCreateImage(cvSize(30, 30), 8, 3);
	 		cvResize(sampleImage, resizeImage);
			//cv::Mat img(resizeImage);
			cv::Mat img = cv::cvarrToMat(resizeImage);
			if(img.empty())
				printf("image is empty!");
			std::vector<Prediction> predictions = caffeObject.Classify(img);
			if (predictions[0].first=="1")
			{
				rect.GdiArray[rect.num] = aimRect;
				rect.num++;
			}
		}
		return rect;
	}

inline int set_cpu(int i)  
{  
    cpu_set_t mask;  
    CPU_ZERO(&mask);  
  
    CPU_SET(i,&mask);  
  
    //printf("thread %u, i = %d\n", pthread_self(), i);  
    if(-1 == pthread_setaffinity_np(pthread_self() ,sizeof(mask),&mask))  
    {  
        return -1;  
    }  
    return 0;  
} 

//process function
/*
cut image four parts
create four thread to due the four parts
*/
void firstStep(CutImageParts parts,IplImage* srcImage,CvHaarClassifierCascade* haarModel,int* shmidArray){
	//cut t*t parts
	int partsNum = parts.partsNum*parts.partsNum;
	clock_t beginTime = clock();
	pid_t pid;
	pid_t wid;
	int i;
	for (i = 0; i < partsNum; i++)
	{
		pid = fork();
		if(pid == 0){
			IplImage* partImage = getSampleImage(srcImage,parts.GdiArray[i]);
			int cpuNum;//=i%4;
			if(i%2==0) cpuNum = 2;
			else cpuNum =3;
			if(set_cpu(cpuNum))  
    		{  
        		printf("set cpu erro\n");  
    		}
			RealRect haarRect = haarDetect(partImage,haarModel);
			RealRect aimRect = caffeClassification(partImage,haarRect); 
			if(toolNum==2||toolNum==4||toolNum==6)
				shareMemoryWrite(shmidArray[i],haarRect);
			if(toolNum==3||toolNum==5||toolNum==7||toolNum==8)
				shareMemoryWrite(shmidArray[i],aimRect);
			break;
		}
		if(pid<0){
			printf("process create fail!\n");
			return;
		}
	}
	//in son process
	if(i>=0&&i<partsNum) _exit(0);

	//process recovery
	//in farther process
	else
	while (i>0)
	{
		wid = waitpid(-1,NULL,WNOHANG);
		if(wid >0) 
		{
			//recovery one process
			i--;
		}
		usleep(10*1000);//wait 10ms
	}

	return;
}

int main(int argc, char* argv[]){
	if(argc>=2)
		toolNum = atoi(argv[1]);

	if(toolNum<0||toolNum>8)
	{
		printf("Parameter error!\n");
		return 0;
	}

	int partNum = 2;
	int shmidArray[partNum*partNum];
	for (int i = 0; i < partNum*partNum; i++)
	{
		shmidArray[i] = createShareMemory(i);
		printf("%d \n",shmidArray[i]);
	}
	//read video test
	VideoCapture cap;
	cap.open(0);//auto detect camera ID
	cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
	//cap.set(CV_CAP_PROP_FPS,10);
	if (!cap.isOpened())
	{
		printf("video open error!\n");
	}
	Mat frame;
	cap >> frame;
	//begin time
	clock_t start = clock();
	CvHaarClassifierCascade* haarModel;
	//this vedio`s photo is 480*640
	while(cap.isOpened()){
		cap >> frame;
		haarModel = cascade3;
		CutImageParts parts = cutImageParts(0,100,frame.cols,frame.rows,partNum);
		IplImage iplTemp = (IplImage)frame;
		IplImage* iplframe = &iplTemp;
		firstStep(parts,iplframe,haarModel,shmidArray);

		//fuse aim
		IplImage* cloneImageByCut = cvCloneImage(iplframe);
		for (int i = 0; i < partNum*partNum; i++)
		{
			IplImage* partImage = getSampleImage(cloneImageByCut,parts.GdiArray[i]);
			ShareMemory sha = shareMemoryRead(shmidArray[i]);
			if(sha.num>0)
			{
				int widthOffset = parts.GdiArray[i].left;
				int heightOffset = parts.GdiArray[i].top;
				for (int arrayNum = 0; arrayNum < sha.num; arrayNum++)
				{
					if(toolNum==2||toolNum==3||toolNum==6||toolNum==7)
						cvRectangle(partImage,
						cvPoint(sha.GdiArray[arrayNum].left,sha.GdiArray[arrayNum].top),
						cvPoint(sha.GdiArray[arrayNum].right,sha.GdiArray[arrayNum].bottom),cvScalar(0,0,255),4,8,0);
					if(toolNum==4||toolNum==5||toolNum==6||toolNum==7||toolNum==8)
					{
						sha.GdiArray[arrayNum] = {
							sha.GdiArray[arrayNum].left+widthOffset,
							sha.GdiArray[arrayNum].top+heightOffset,
							sha.GdiArray[arrayNum].right+widthOffset,
							sha.GdiArray[arrayNum].bottom+heightOffset
						};
						if(i==3) printf("%d %d %d %d \n",sha.GdiArray[arrayNum].left,sha.GdiArray[arrayNum].top,sha.GdiArray[arrayNum].right,sha.GdiArray[arrayNum].bottom);
						cvRectangle(iplframe,
						cvPoint(sha.GdiArray[arrayNum].left,sha.GdiArray[arrayNum].top),
						cvPoint(sha.GdiArray[arrayNum].right,sha.GdiArray[arrayNum].bottom),cvScalar(0,0,255),4,8,0);
					}
				}
			}
			if(toolNum==1||toolNum==2||toolNum==3||toolNum==6||toolNum==7)
				imshow("part"+to_string(i),cvarrToMat(partImage));
		}
		
		if(toolNum==0||toolNum==4||toolNum==5||toolNum==6||toolNum==7||toolNum==8)
			imshow("cap",frame);
		char key = static_cast<char>(cvWaitKey(30));
		if(key == 27) break;
	}
	//VideoCaption release
	cap.release();

	for (int i = 0; i < partNum; i++)
	{
		destoryShareMemory(shmidArray[i]);
	}
}
