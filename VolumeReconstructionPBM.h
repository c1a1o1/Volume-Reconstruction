#ifndef VOLUMERECONSTRUCTIONPBM_H
#define	VOLUMERECONSTRUCTIONPBM_H

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>

#include <itkCastImageFilter.h>
#include <itkImage.h>

// Other includes 
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

#include <vector>
#include <QString>

class VolumeReconstructionPBM 
{
    
public:
    
	typedef itk::Image<unsigned char, 2> ImageType;    
    typedef itk::Image<float, 2> FloatImageType;
    typedef itk::CastImageFilter<FloatImageType, ImageType> CastFilterType;
    
    /**
     * \brief Constructor
     */
   static VolumeReconstructionPBM *New()
   {
	return new VolumeReconstructionPBM;
   } 
   
    /**
     * \brief Set the transformation for each image used in the reconstruction
     */
    void setTransformStack(std::vector< vnl_matrix<double> >);

   /**
     * \brief Set the size of the volume data
     */
	void setVolumeSize(vnl_vector<double>);

    /**
     * \brief Set the volume data orgin in the 3D scene
     */
	void setVolumeOrigin(vnl_vector<double>);

	/**
     * \brief Set image data stack to generate the volume
     */
    void setVolumeImageStack(std::vector< vtkSmartPointer< vtkImageData> >);

	/**
     * \brief Set the scale of the images
     */
    void setScale(vnl_vector<double>);

	/**
     * \brief Set the resolution of the volume
     */
    void setResolution(int);

   /**
     * \brief Returns the new volume data with the pixel based method
     */
   vtkSmartPointer<vtkImageData> generateVolume();
   
   
private:    

	/** Reconstruction Volume */
    vtkSmartPointer<vtkImageData> volumeData;

	/** Volume with filled voxels */
    vtkSmartPointer<vtkImageData> filledVolume;

	/** Volume with accumulated pixels */
	vtkSmartPointer<vtkImageData> AccDataVolume;

	/** Size of the volume */
	vnl_vector<double> volumeSize;

    /** Where the volume data begins in the 3D scene */
	vnl_vector<double> volumeOrigin;

	/** The stack of images data */
    std::vector< vtkSmartPointer< vtkImageData> > volumeImageStack;

	/** The stack of images data */
    std::vector< vtkSmartPointer< vtkImageData> > gradientImageStack;

	/** Contains the transformation for each image */
    std::vector< vnl_matrix<double> > transformStack;
    
	/** scale of the images */
    vnl_vector<double> scale;

	/** The resolution of the volume*/
    int resolution;

	/** the maximun distance found in the volume */
	double maxDistance;

	std::vector<std::vector<std::vector<std::vector<double> > > > gaussKernelsStack;

    /**
     * Fill the nearest voxel with pixel information
     * @param volume
     * @param imageWidth
     * @param imageHeight
     * @param imageSize
     * @param xScale
     * @param yScale
     * @param zScale
     * @param volumeScale
     */
    void binFillingNN();

	void binFillingGauss();

	void binFillingAdaptiveGauss();

	void holeFillingFixedRegion();

	void holeFillingGrowingRegion();

	void computeGaussKernels(float maxVariance, float step);

	void computeImageGradient();

	/**
     * \brief Computes the maximun distance in the volume
     */
	double calcMaxDistance();

	/**
    * \brief convert a vtk image to an itk image
    * [out] an itk image
    */
    FloatImageType::Pointer convertToITKImage(vtkSmartPointer<vtkImageData> vtkImage);
    
    /**
    * \brief convert an itk image to a vtk image
    * [out] a vtk image
    */
    vtkSmartPointer<vtkImageData> convertToVTKImage(FloatImageType::Pointer itkImage);
    
};


#endif	/* VOLUMERECONSTRUCTIONPBM_H */

