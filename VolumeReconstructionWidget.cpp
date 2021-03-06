#include "VolumeReconstructionWidget.h"
#include "ui_VolumeReconstructionWidget.h"
#include "VolumeReconstruction.h"
#include "VolumeReconstructionPBM.h"
#include "vtkMetaImageWriter.h"

#include <QString>

VolumeReconstructionWidget::VolumeReconstructionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumeReconstructionWidget)
{

	ui->setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);
        ui->resolution->show();
        ui->resolution->setTickInterval(1);
        ui->resolution->setRange(1, 10);

}

VolumeReconstructionWidget::~VolumeReconstructionWidget()
{
    delete ui;
}

void VolumeReconstructionWidget::setMainWindow(MainWindow* mainwindow)
{
    this->mainWindow = mainwindow;
}


void VolumeReconstructionWidget::generate()
{

	volumeData = vtkSmartPointer<vtkImageData>::New();

	if(ui->pixelMethod->isChecked()){
		
		calcImageBounds();
		calcVolumeSize(false);

		VolumeReconstructionPBM * reconstructor = VolumeReconstructionPBM::New();

		reconstructor->setScale(scale);
		reconstructor->setVolumeImageStack(volumeImageStack);
		reconstructor->setTransformStack(transformStack);
		reconstructor->setVolumeOrigin(volumeOrigin);
		reconstructor->setVolumeSize(volumeSize);
        reconstructor->setResolution(res);
		
		volumeData = reconstructor->generateVolume();


	}else if(ui->voxelMethod->isChecked()){
		
		calcImageBounds();
		calcPointCloud();
		calcVolumeSize(false);

		VolumeReconstruction * reconstructor = VolumeReconstruction::New();

		reconstructor->setImageBoundsStack(imageBoundsXStack, imageBoundsYStack, imageBoundsZStack);
		reconstructor->setScale(scale);
		reconstructor->setTransformStack(transformStack);
		reconstructor->setVolumeImageStack(volumeImageStack);
		reconstructor->setVolumeOrigin(volumeOrigin);
		reconstructor->setVolumeSize(volumeSize);
        reconstructor->setResolution(res);
		reconstructor->setPointCloud(pointCloudX,pointCloudY,pointCloudZ,pointCloudGray);
		
		volumeData = reconstructor->generateVolume();

	}

    mainWindow->getDisplayWidget()->setAndDisplayVolume(volumeData);
	mainWindow->getDisplayWidget()->setVolumeOrigin(volumeOrigin);
}

void VolumeReconstructionWidget::setTransformStack(std::vector< vnl_matrix<double> > transformStack)
{
    this->transformStack = transformStack;
}

void VolumeReconstructionWidget::setVolumeImageStack(std::vector< vtkSmartPointer<vtkImageData> > volumeImageStack)
{
    this->volumeImageStack = volumeImageStack;
}

void VolumeReconstructionWidget::calcImageBounds()
{
	std::cout<<"Calculating images bounds"<<std::endl;

	for(int i=0; i<volumeImageStack.size(); i++){	

		int * imageSize = volumeImageStack.at(i)->GetDimensions();		
		vnl_vector<double> point;
		vnl_vector<double> transformedPoint;
		vnl_vector<double> imageBoundsX;
		vnl_vector<double> imageBoundsY;
		vnl_vector<double> imageBoundsZ;

		imageBoundsX.set_size(4);
		imageBoundsY.set_size(4);
		imageBoundsZ.set_size(4);

		point.set_size(4);
		transformedPoint.set_size(4);

		point[2] = 0;
		point[3] = 1;

		scale = mainWindow->getDisplayWidget()->getTransformScale();

		point[0] = scale[0]*0;
		point[1] = scale[1]*0;
		transformedPoint = transformStack.at(i)*point;
		imageBoundsX[0] = transformedPoint[0]; 
		imageBoundsY[0] = transformedPoint[1];
		imageBoundsZ[0] = transformedPoint[2];
		
		point[0] = scale[0]*imageSize[0];
		point[1] = scale[1]*0;
		transformedPoint = transformStack.at(i)*point;
		imageBoundsX[1] = transformedPoint[0]; 
		imageBoundsY[1] = transformedPoint[1];
		imageBoundsZ[1] = transformedPoint[2];

		point[0] = scale[0]*0;
		point[1] = scale[1]*imageSize[1];
		transformedPoint = transformStack.at(i)*point;
		imageBoundsX[2] = transformedPoint[0]; 
		imageBoundsY[2] = transformedPoint[1];
		imageBoundsZ[2] = transformedPoint[2];

		point[0] = scale[0]*imageSize[0];
		point[1] = scale[1]*imageSize[1];
		transformedPoint = transformStack.at(i)*point;
		imageBoundsX[3] = transformedPoint[0]; 
		imageBoundsY[3] = transformedPoint[1];
		imageBoundsZ[3] = transformedPoint[2];

		imageBoundsXStack.push_back(imageBoundsX);
		imageBoundsYStack.push_back(imageBoundsY);
		imageBoundsZStack.push_back(imageBoundsZ);

	}

}


void VolumeReconstructionWidget::calcVolumeSize(bool usePixelMethod)
{
	vnl_vector<double> xMin;
	vnl_vector<double> xMax;
	vnl_vector<double> yMin;
	vnl_vector<double> yMax;
	vnl_vector<double> zMin;
	vnl_vector<double> zMax;

	xMin.set_size(volumeImageStack.size());
	xMax.set_size(volumeImageStack.size());
	yMin.set_size(volumeImageStack.size());
	yMax.set_size(volumeImageStack.size());
	zMin.set_size(volumeImageStack.size());
	zMax.set_size(volumeImageStack.size());

	std::cout<<std::endl;

	if(usePixelMethod){

		for(int i=0; i<volumeImageStack.size(); i++){
		
			xMin.put(i,imageCoordsXStack.at(i).min_value());
			xMax.put(i,imageCoordsXStack.at(i).max_value());

			yMin.put(i,imageCoordsYStack.at(i).min_value());
			yMax.put(i,imageCoordsYStack.at(i).max_value());

			zMin.put(i,imageCoordsZStack.at(i).min_value());
			zMax.put(i,imageCoordsZStack.at(i).max_value());
		
		}
	}else{

		for(int i=0; i<volumeImageStack.size(); i++){
		
			xMin.put(i,imageBoundsXStack.at(i).min_value());
			xMax.put(i,imageBoundsXStack.at(i).max_value());

			yMin.put(i,imageBoundsYStack.at(i).min_value());
			yMax.put(i,imageBoundsYStack.at(i).max_value());

			zMin.put(i,imageBoundsZStack.at(i).min_value());
			zMax.put(i,imageBoundsZStack.at(i).max_value());
		
		}
	}

	volumeOrigin.set_size(3);
	volumeOrigin[0] = xMin.min_value();
	volumeOrigin[1] = yMin.min_value();
	volumeOrigin[2] = zMin.min_value();
	std::cout<<std::endl<<"Volume origin coords: "<<volumeOrigin[0]<<","<<volumeOrigin[1]<<","<<volumeOrigin[2]<<std::endl;
	
	volumeFinal.set_size(3);
	volumeFinal[0] = xMax.max_value();
	volumeFinal[1] = yMax.max_value();
	volumeFinal[2] = zMax.max_value();
	std::cout<<"Volume final coords: "<<volumeFinal[0]<<","<<volumeFinal[1]<<","<<volumeFinal[2]<<std::endl;

	volumeSize.set_size(3);
	volumeSize[0] = vtkMath::Ceil((volumeFinal[0] - volumeOrigin[0])/(scale[0]*res));
	volumeSize[1] = vtkMath::Ceil((volumeFinal[1] - volumeOrigin[1])/(scale[1]*res));
	volumeSize[2] = vtkMath::Ceil((volumeFinal[2] - volumeOrigin[2])/(scale[1]*res));
	std::cout<<"Volume size: "<<volumeSize[0]<<","<<volumeSize[1]<<","<<volumeSize[2]<<std::endl<<std::endl;
}


void VolumeReconstructionWidget::save()
{
	vtkSmartPointer<vtkMetaImageWriter> writer = vtkSmartPointer<vtkMetaImageWriter>::New();
	
	QString saveDirectory = QFileDialog::getSaveFileName(
                this, tr("Choose File to Save Volume"), QDir::currentPath());

	QString saveMhdDirectory = saveDirectory;
	QString saveRawDirectory = saveDirectory;

    QString mhdFilename = ".mhd";
	QString rawFilename = ".raw";

    QString qtSaveMhdFile = saveMhdDirectory.append(mhdFilename);
	QString qtSaveRawFile = saveRawDirectory.append(rawFilename);

	std::string str1 = std::string(qtSaveMhdFile.toAscii().data());
	const char * saveMhdFile = str1.c_str();

	std::string str2 = std::string(qtSaveRawFile.toAscii().data());
	const char * saveRawFile = str2.c_str();

	std::cout<<"Saving Volume in files:"<<std::endl<<std::endl;
	std::cout<<saveMhdFile<<std::endl<<std::endl;
	std::cout<<saveRawFile<<std::endl;

	writer->SetFileName(saveMhdFile);
	writer->SetRAWFileName(saveRawFile);
	writer->SetInputConnection(volumeData->GetProducerPort());

	try{
	writer->Write();
	}catch( exception& e){
		std::cout<<e.what()<<std::endl;
	}
}

void VolumeReconstructionWidget::setResolution(int idx)
{
    res = idx;
}

void VolumeReconstructionWidget::calcPointCloud(){

	std::cout<<"Calculating Point Cloud"<<std::endl;

	vnl_vector<double> point;
	point.set_size(4);
	point[0]=0;
	point[1]=0;
	point[2]=0;
	point[3]=1;

	for(int i=0; i<volumeImageStack.size(); i++){	

		 int * imageSize = volumeImageStack.at(i)->GetDimensions();

	        for (int x = 0; x<imageSize[0]; x++){
				for (int y = 0; y<imageSize[1]; y++){
                
					point[0]=scale[0]*x;
					point[1]=scale[1]*y;
	
					vnl_vector<double> transformedPoint = transformStack.at(i)*point;
					unsigned char * imagePixel = static_cast<unsigned char *> (
				                volumeImageStack.at(i)->GetScalarPointer(x, y, 0));

					pointCloudX.push_back(transformedPoint[0]);
					pointCloudY.push_back(transformedPoint[1]);
					pointCloudZ.push_back(transformedPoint[2]);	
					pointCloudGray.push_back(imagePixel[0]);
				}
			}
	}

}