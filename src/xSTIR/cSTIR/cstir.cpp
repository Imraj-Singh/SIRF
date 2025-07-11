/*
SyneRBI Synergistic Image Reconstruction Framework (SIRF)
Copyright 2017 - 2024 Rutherford Appleton Laboratory STFC
Copyright 2018 - 2024 University College London.

This is software developed for the Collaborative Computational
Project in Synergistic Reconstruction for Biomedical Imaging (formerly CCP PETMR)
(http://www.ccpsynerbi.ac.uk/).

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include <vector>

#include "sirf/common/iequals.h"
#include "sirf/STIR/stir_types.h"
#include "sirf/iUtilities/DataHandle.h"
#include "sirf/STIR/cstir_p.h"
#include "stir/find_STIR_config.h"
#include "sirf/STIR/stir_x.h"
#include "stir/config.h"
#include "stir/ImagingModality.h"
#include "stir/Scanner.h"
#include "stir/Verbosity.h"
#include "stir/num_threads.h"

using namespace stir;
using namespace sirf;

#define NEW_OBJECT_HANDLE(T) new ObjectHandle<T >(std::shared_ptr<T >(new T))
#define SPTR_FROM_HANDLE(Object, X, H) \
  std::shared_ptr<Object> X; getObjectSptrFromHandle<Object>(H, X);
#define HANDLE_FROM_SPTR(Object, X, H) \
  setHandleObjectSptr<Object>(H, X);

static void*
unknownObject(const char* obj, const char* name, const char* file, int line)
{
	DataHandle* handle = new DataHandle;
	std::string error = "unknown ";
	error += obj;
	error += " '";
	error += name;
	error += "'";
	ExecutionStatus status(error.c_str(), file, line);
	handle->set(0, &status);
	return (void*)handle;
}

extern "C"
void*
cSTIR_STIR_version_string()
{
#if defined(STIR_VERSION_STRING)
	return charDataHandleFromCharData(STIR_VERSION_STRING);
#else
	return charDataHandleFromCharData("unknown");
#endif
}

extern "C"
void*
cSTIR_get_STIR_doc_dir()
{
	return charDataHandleFromCharData(get_STIR_doc_dir().c_str());
}

extern "C"
void*
cSTIR_get_STIR_examples_dir()
{
	return charDataHandleFromCharData(get_STIR_examples_dir().c_str());
}

template<class Method>
void*
cSTIR_newReconstructionMethod(const char* par_file)
{
	try {
		if (strlen(par_file) > 0) {
                        std::shared_ptr<Reconstruction<Image3DF> > sptr(new Method(par_file));
			return newObjectHandle(sptr);
		}
		else {
                        std::shared_ptr<Reconstruction<Image3DF> > sptr(new Method);
			return newObjectHandle(sptr);
		}
	}
	CATCH;
}

extern "C"
void* cSTIR_setVerbosity(const int verbosity)
{
    stir::Verbosity::set(verbosity);
    return new DataHandle;
}

extern "C"
void* cSTIR_getVerbosity(const int verbosity)
{
    return dataHandle<int>(stir::Verbosity::get());
}

extern "C"
void* cSTIR_setOMPThreads(const int threads)
{
	stir::set_num_threads(threads);
	return new DataHandle;
}

extern "C"
void* cSTIR_getOMPThreads()
{
	return dataHandle<int>(stir::get_max_num_threads());
}

extern "C"
void* cSTIR_useDefaultOMPThreads()
{
	stir::set_default_num_threads();
	return new DataHandle;
}
extern "C"
void* cSTIR_getDefaultOMPThreads()
{
	return dataHandle<int>(stir::get_default_num_threads());
}

extern "C"
void* cSTIR_scannerNames()
{
	try {
		std::string scanners = Scanner::list_all_names();
		return charDataHandleFromCharData(scanners.c_str());
	}
	CATCH;
}

extern "C"
void* cSTIR_newObject(const char* name)
{
	try {
		if (sirf::iequals(name, "FBP2D"))
			return NEW_OBJECT_HANDLE(xSTIR_FBP2DReconstruction);
		if (sirf::iequals(name, "ListmodeToSinograms"))
			return NEW_OBJECT_HANDLE(ListmodeToSinograms);
		if (sirf::iequals(name,
			"PoissonLogLikelihoodWithLinearModelForMeanAndProjData"))
			return NEW_OBJECT_HANDLE
			(xSTIR_PoissonLogLikelihoodWithLinearModelForMeanAndProjData3DF);
		if (sirf::iequals(name,
			"PoissonLogLikelihoodWithLinearModelForMeanAndListModeDataWithProjMatrixByBin"))
			return NEW_OBJECT_HANDLE
			(xSTIR_PoissonLLhLinModMeanListDataProjMatBin3DF);
		if (sirf::iequals(name, "AcqModUsingMatrix"))
			return NEW_OBJECT_HANDLE(AcqModUsingMatrix3DF);
#ifdef STIR_WITH_NiftyPET_PROJECTOR
        if (sirf::iequals(name, "AcqModUsingNiftyPET"))
            return NEW_OBJECT_HANDLE(AcqModUsingNiftyPET3DF);
#endif
#ifdef STIR_WITH_Parallelproj_PROJECTOR
		if (sirf::iequals(name, "AcqModUsingParallelproj"))
			return NEW_OBJECT_HANDLE(AcqModUsingParallelproj);
#endif
		if (sirf::iequals(name, "RayTracingMatrix"))
			return NEW_OBJECT_HANDLE(RayTracingMatrix);
		if (sirf::iequals(name, "SPECTUBMatrix"))
			return NEW_OBJECT_HANDLE(SPECTUBMatrix);
		if (sirf::iequals(name, "PinholeSPECTUBMatrix"))
			return NEW_OBJECT_HANDLE(PinholeSPECTUBMatrix);
		if (sirf::iequals(name, "QuadraticPrior"))
			return NEW_OBJECT_HANDLE(QuadPrior3DF);
		if (sirf::iequals(name, "LogcoshPrior"))
			return NEW_OBJECT_HANDLE(LogPrior3DF);
		if (sirf::iequals(name, "RelativeDifferencePrior"))
			return NEW_OBJECT_HANDLE(RDPrior3DF);
#ifdef STIR_WITH_CUDA
		if (sirf::iequals(name, "CudaRelativeDifferencePrior"))
			return NEW_OBJECT_HANDLE(CudaRDPrior3DF);
#endif
		if (sirf::iequals(name, "PLSPrior"))
			return NEW_OBJECT_HANDLE(PLSPrior3DF);
		if (sirf::iequals(name, "TruncateToCylindricalFOVImageProcessor"))
			return NEW_OBJECT_HANDLE(CylindricFilter3DF);
		if (sirf::iequals(name, "Box3D"))
			return NEW_OBJECT_HANDLE(xSTIR_Box3D);
		if (sirf::iequals(name, "Ellipsoid"))
			return NEW_OBJECT_HANDLE(Ellipsoid);
		if (sirf::iequals(name, "EllipsoidalCylinder"))
			return NEW_OBJECT_HANDLE(EllipsoidalCylinder);
		if (sirf::iequals(name, "PETSingleScatterSimulator"))
                  return NEW_OBJECT_HANDLE(PETSingleScatterSimulator);
                if (sirf::iequals(name, "PETScatterEstimator"))
                  return NEW_OBJECT_HANDLE(PETScatterEstimator);
		if (sirf::iequals(name, "SeparableGaussianImageFilter"))
			return NEW_OBJECT_HANDLE(xSTIR_SeparableGaussianImageFilter);
                if (sirf::iequals(name, "PoissonNoiseGenerator"))
                  return NEW_OBJECT_HANDLE(PoissonNoiseGenerator);
		return unknownObject("object", name, __FILE__, __LINE__);
	}
	CATCH;
}

extern "C"
void* cSTIR_setParameter
(void* ptr_s, const char* obj, const char* name, const void* ptr_v)
{
	try {
		CAST_PTR(DataHandle, hs, ptr_s);
		CAST_PTR(DataHandle, hv, ptr_v);
		if (sirf::iequals(obj, "ImageData"))
			return cSTIR_setImageDataParameter(ptr_s, name, ptr_v);
		else if (sirf::iequals(obj, "ListmodeToSinograms"))
			return cSTIR_setListmodeToSinogramsParameter(ptr_s, name, ptr_v);
		else if (sirf::iequals(obj, "SeparableGaussianImageFilter"))
			return cSTIR_setSeparableGaussianImageFilterParameter(ptr_s, name, ptr_v);
		else if (sirf::iequals(obj, "Shape"))
			return cSTIR_setShapeParameter(ptr_s, name, ptr_v);
		else if (sirf::iequals(obj, "Box3D"))
			return cSTIR_setBox3DParameter(hs, name, hv);
		else if (sirf::iequals(obj, "Ellipsoid"))
			return cSTIR_setEllipsoidParameter(hs, name, hv);
		else if (sirf::iequals(obj, "EllipsoidalCylinder"))
			return cSTIR_setEllipsoidalCylinderParameter(hs, name, hv);
		else if (sirf::iequals(obj, "TruncateToCylindricalFOVImageProcessor"))
			return cSTIR_setTruncateToCylindricalFOVImageProcessorParameter
			(hs, name, hv);
		else if (sirf::iequals(obj, "AcquisitionModel"))
			return cSTIR_setAcquisitionModelParameter(hs, name, hv);
		else if (sirf::iequals(obj, "AcqModUsingMatrix"))
			return cSTIR_setAcqModUsingMatrixParameter(hs, name, hv);
#ifdef STIR_WITH_NiftyPET_PROJECTOR
        else if (sirf::iequals(obj, "AcqModUsingNiftyPET"))
            return cSTIR_setAcqModUsingNiftyPETParameter(hs, name, hv);
#endif
		else if (sirf::iequals(obj, "RayTracingMatrix"))
			return cSTIR_setRayTracingMatrixParameter(hs, name, hv);
		else if (sirf::iequals(obj, "SPECTUBMatrix"))
			return cSTIR_setSPECTUBMatrixParameter(hs, name, hv);
		else if (sirf::iequals(obj, "PinholeSPECTUBMatrix"))
			return cSTIR_setPinholeSPECTUBMatrixParameter(hs, name, hv);
		else if (sirf::iequals(obj, "GeneralisedPrior"))
			return cSTIR_setGeneralisedPriorParameter(hs, name, hv);
		else if (sirf::iequals(obj, "QuadraticPrior"))
			return cSTIR_setQuadraticPriorParameter(hs, name, hv);
		else if (sirf::iequals(obj, "LogcoshPrior"))
			return cSTIR_setLogcoshPriorParameter(hs, name, hv);
		else if (sirf::iequals(obj, "RelativeDifferencePrior"))
			return cSTIR_setRelativeDifferencePriorParameter(hs, name, hv);
		else if (sirf::iequals(obj, "PLSPrior"))
			return cSTIR_setPLSPriorParameter(hs, name, hv);
		else if (sirf::iequals(obj, "GeneralisedObjectiveFunction"))
			return cSTIR_setGeneralisedObjectiveFunctionParameter(hs, name, hv);
		else if (sirf::iequals(obj, "PoissonLogLikelihoodWithLinearModelForMean"))
			return cSTIR_setPoissonLogLikelihoodWithLinearModelForMeanParameter
			(hs, name, hv);
		else if (sirf::iequals(obj,
			"PoissonLogLikelihoodWithLinearModelForMeanAndProjData"))
			return
			cSTIR_setPoissonLogLikelihoodWithLinearModelForMeanAndProjDataParameter
			(hs, name, hv);
        else if (sirf::iequals(obj,
            "PoissonLogLikelihoodWithLinearModelForMeanAndListModeDataWithProjMatrixByBin"))
            return
            cSTIR_setPoissonLogLikelihoodWithLinearModelForMeanAndListModeDataWithProjMatrixByBinParameter
            (hs, name, hv);
		else if (sirf::iequals(obj, "Reconstruction"))
			return cSTIR_setReconstructionParameter(hs, name, hv);
		else if (sirf::iequals(obj, "IterativeReconstruction"))
			return cSTIR_setIterativeReconstructionParameter(hs, name, hv);
		else if (sirf::iequals(obj, "OSMAPOSL"))
			return cSTIR_setOSMAPOSLParameter(hs, name, hv);
#ifdef USE_HKEM
		else if (sirf::iequals(obj, "KOSMAPOSL"))
			return cSTIR_setKOSMAPOSLParameter(hs, name, hv);
#endif
		else if (sirf::iequals(obj, "OSSPS"))
			return cSTIR_setOSSPSParameter(hs, name, hv);
		else if (sirf::iequals(obj, "FBP2D"))
			return cSTIR_setFBP2DParameter(hs, name, hv);
                else if(sirf::iequals(obj, "PETSingleScatterSimulator"))
                        return cSTIR_setScatterSimulatorParameter(hs, name, hv);
                else if(sirf::iequals(obj, "PETScatterEstimator"))
                        return cSTIR_setScatterEstimatorParameter(hs, name, hv);
                else if(sirf::iequals(obj, "PoissonNoiseGenerator"))
                        return cSTIR_setPoissonNoiseGeneratorParameter(hs, name, hv);
		else
			return unknownObject("object", obj, __FILE__, __LINE__);
	}
	CATCH;
}

extern "C"
void* cSTIR_parameter(const void* ptr, const char* obj, const char* name) 
{
	try {
		CAST_PTR(DataHandle, handle, ptr);
		if (sirf::iequals(obj, "Shape"))
			return cSTIR_shapeParameter(handle, name);
		else if (sirf::iequals(obj, "Box3D"))
			return cSTIR_Box3DParameter(handle, name);
		else if (sirf::iequals(obj, "Ellipsoid"))
			return cSTIR_ellipsoidParameter(handle, name);
		else if (sirf::iequals(obj, "EllipsoidalCylinder"))
			return cSTIR_ellipsoidalCylinderParameter(handle, name);
		else if (sirf::iequals(obj, "TruncateToCylindricalFOVImageProcessor"))
			return cSTIR_truncateToCylindricalFOVImageProcessorParameter
			(handle, name);
		else if (sirf::iequals(obj, "AcquisitionData"))
			return cSTIR_AcquisitionDataParameter(handle, name);
		else if (sirf::iequals(obj, "ImageData"))
			return cSTIR_ImageDataParameter(handle, name);
		else if (sirf::iequals(obj, "RayTracingMatrix"))
			return cSTIR_rayTracingMatrixParameter(handle, name);
		else if (sirf::iequals(obj, "SPECTUBMatrix"))
			return cSTIR_SPECTUBMatrixParameter(handle, name);
		else if (sirf::iequals(obj, "PinholeSPECTUBMatrix"))
			return cSTIR_PinholeSPECTUBMatrixParameter(handle, name);
		else if (sirf::iequals(obj, "AcquisitionModel"))
			return cSTIR_AcquisitionModelParameter(handle, name);
		else if (sirf::iequals(obj, "AcqModUsingMatrix"))
			return cSTIR_acqModUsingMatrixParameter(handle, name);
		else if (sirf::iequals(obj, "GeneralisedPrior"))
			return cSTIR_generalisedPriorParameter(handle, name);
		else if (sirf::iequals(obj, "PLSPrior"))
			return cSTIR_PLSPriorParameter(handle, name);
		else if (sirf::iequals(obj, "QuadraticPrior"))
			return cSTIR_QuadraticPriorParameter(handle, name);
		else if (sirf::iequals(obj, "LogcoshPrior"))
			return cSTIR_LogcoshPriorParameter(handle, name);
		else if (sirf::iequals(obj, "RelativeDifferencePrior"))
			return cSTIR_RelativeDifferencePriorParameter(handle, name);
		else if (sirf::iequals(obj, "GeneralisedObjectiveFunction"))
			return cSTIR_generalisedObjectiveFunctionParameter(handle, name);
        else if (sirf::iequals(obj,
            "PoissonLogLikelihoodWithLinearModelForMeanAndListModeDataWithProjMatrixByBin"))
            return
            cSTIR_PoissonLogLikelihoodWithLinearModelForMeanAndListModeDataWithProjMatrixByBinParameter
            (handle, name);
        else if (sirf::iequals(obj,
            "PoissonLogLikelihoodWithLinearModelForMeanAndProjData"))
            return
            cSTIR_PoissonLogLikelihoodWithLinearModelForMeanAndProjDataParameter
            (handle, name);
		else if (sirf::iequals(obj, "IterativeReconstruction"))
			return cSTIR_iterativeReconstructionParameter(handle, name);
		else if (sirf::iequals(obj, "OSMAPOSL"))
			return cSTIR_OSMAPOSLParameter(handle, name);
		else if (sirf::iequals(obj, "KOSMAPOSL"))
			return cSTIR_OSMAPOSLParameter(handle, name);
		else if (sirf::iequals(obj, "OSSPS"))
			return cSTIR_OSSPSParameter(handle, name);
		else if (sirf::iequals(obj, "FBP2D"))
			return cSTIR_FBP2DParameter(handle, name);
                else if(sirf::iequals(obj, "PETScatterEstimator"))
                        return cSTIR_ScatterEstimatorParameter(handle, name);
//                else if(sirf::iequals(obj, "PoissonNoiseGenerator"))
//                        return cSTIR_PoissonNoiseGeneratorParameter(handle, name);
		return unknownObject("object", obj, __FILE__, __LINE__);
	}
	CATCH;
}

extern "C"
void* cSTIR_objectFromFile(const char* name, const char* filename)
{
	try {
		if (sirf::iequals(name, "OSMAPOSLReconstruction"))
			return cSTIR_newReconstructionMethod
			<OSMAPOSLReconstruction<Image3DF> >
			(filename);
#ifdef USE_HKEM
		if (sirf::iequals(name, "KOSMAPOSLReconstruction"))
			return cSTIR_newReconstructionMethod
			<KOSMAPOSLReconstruction<Image3DF> >
			(filename);
#endif
		if (sirf::iequals(name, "OSSPSReconstruction"))
			return cSTIR_newReconstructionMethod
			<OSSPSReconstruction<Image3DF> >
			(filename);
		if (sirf::iequals(name, "Image")) {
                        std::shared_ptr<STIRImageData> sptr(new STIRImageData(filename));
			return newObjectHandle(sptr);
		}
		if (sirf::iequals(name, "AcquisitionData")) {

            std::shared_ptr<STIRAcquisitionData> sptr;
            if (STIRAcquisitionData::storage_scheme().compare("file") == 0)
                sptr.reset(new STIRAcquisitionDataInFile(filename));
            else
                sptr.reset(new STIRAcquisitionDataInMemory(filename));
			return newObjectHandle(sptr);
		}
		if (sirf::iequals(name, "ListmodeData")) {
			std::shared_ptr<STIRListmodeData>
				sptr(new STIRListmodeData(filename));
			return newObjectHandle(sptr);
		}
		if (sirf::iequals(name, "ListmodeToSinograms")) {
			std::shared_ptr<ListmodeToSinograms>
				sptr(new ListmodeToSinograms(filename));
			return newObjectHandle(sptr);
		}
                if (sirf::iequals(name, "PETSingleScatterSimulator")) {
                    stir::shared_ptr<PETSingleScatterSimulator>
                    sptr(new PETSingleScatterSimulator(filename));
                  return newObjectHandle(sptr);
                }
                if (sirf::iequals(name, "PETScatterEstimator")) {
                  stir::shared_ptr<PETScatterEstimator>
                    sptr(new PETScatterEstimator(filename));
                  return newObjectHandle(sptr);
                }
		return unknownObject("object", name, __FILE__, __LINE__);
	}
	CATCH;
}

typedef xSTIR_PoissonLLhLinModMeanListDataProjMatBin3DF LMObjFun;

extern "C"
void* cSTIR_objFunListModeSetInterval(void* ptr_f, size_t ptr_data)
{
	try {
		auto& objFun = objectFromHandle<LMObjFun>(ptr_f);
		float* data = (float*)ptr_data;
		objFun.set_time_interval((double)data[0], (double)data[1]);
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_setListmodeToSinogramsInterval(void* ptr_lm2s, size_t ptr_data)
{
	try {
		auto& lm2s = objectFromHandle<ListmodeToSinograms>(ptr_lm2s);
		float *data = (float *)ptr_data;
		lm2s.set_time_interval((double)data[0], (double)data[1]);
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_setListmodeToSinogramsFlag(void* ptr_lm2s, const char* flag, int v)
{
	try {
		DataHandle* handle = new DataHandle;
		ListmodeToSinograms& lm2s =
			objectFromHandle<ListmodeToSinograms>(ptr_lm2s);
		int err = lm2s.set_flag(flag, (bool)v);
		if (err) {
			std::string err_msg;
			err_msg = "ListmodeToSinogram does not have this flag: ";
			err_msg += flag;
			ExecutionStatus status(err_msg.c_str(), __FILE__, __LINE__);
			handle->set(0, &status);
		}
		return (void*)handle;
	}
	CATCH;
}

extern "C"
void* cSTIR_setupListmodeToSinogramsConverter(void* ptr)
{
	try {
		ListmodeToSinograms& lm2s = objectFromHandle<ListmodeToSinograms>(ptr);
		DataHandle* handle = new DataHandle;
		if (lm2s.set_up() == stir::Succeeded::no) {
			ExecutionStatus status
				("cSTIR_setupListmodeToSinogramConverter failed", 
					__FILE__, __LINE__);
			handle->set(0, &status);
		}
		return (void*)handle;
	}
	CATCH;
}

extern "C"
void* cSTIR_convertListmodeToSinograms(void* ptr)
{
	try {
		ListmodeToSinograms& lm2s = objectFromHandle<ListmodeToSinograms>(ptr);
		lm2s.process_data();
		return newObjectHandle(lm2s.get_output());
	}
	CATCH;
}

extern "C"
void* cSTIR_scatterSimulatorFwd
(void* ptr_am, void* ptr_im)
{
	try {
		auto& am = objectFromHandle<PETSingleScatterSimulator>(ptr_am);
		auto& id = objectFromHandle<STIRImageData>(ptr_im);
		return newObjectHandle(am.forward(id));
	}
	CATCH;
}

extern "C"
void* cSTIR_scatterSimulatorFwdReplace
(void* ptr_am, void* ptr_im, void* ptr_ad)
{
	try {
		auto& am = objectFromHandle<PETSingleScatterSimulator>(ptr_am);
		auto& id = objectFromHandle<STIRImageData>(ptr_im);
		auto& ad = objectFromHandle<STIRAcquisitionData>(ptr_ad);
                am.forward(ad, id);
		return new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_setupScatterSimulator
(void* ptr_am, void* ptr_ad, void* ptr_im)
{
	try {
		auto& am = objectFromHandle<PETSingleScatterSimulator>(ptr_am);
		SPTR_FROM_HANDLE(STIRImageData, id, ptr_im);
		SPTR_FROM_HANDLE(STIRAcquisitionData, ad, ptr_ad);
                am.set_up(ad, id);
		return new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_setupScatterEstimator(void* ptr_r)
{
    try {
        auto& se = objectFromHandle<PETScatterEstimator>(ptr_r);
        se.set_up();
        DataHandle* handle = new DataHandle;
        return handle;
    }
    CATCH;
}

extern "C"
void* cSTIR_runScatterEstimator(void* ptr_r)
{
    try {
        auto& se = objectFromHandle<PETScatterEstimator>(ptr_r);
        se.process();
        DataHandle* handle = new DataHandle;
        return handle;
    }
    CATCH;
}

extern "C"
void* cSTIR_computeRandoms(void* ptr)
{
	try {
		ListmodeToSinograms& lm2s = objectFromHandle<ListmodeToSinograms>(ptr);
		if (lm2s.estimate_randoms()) {
			ExecutionStatus status
				("cSTIR_computeRandoms failed", __FILE__, __LINE__);
			DataHandle* handle = new DataHandle;
			handle->set(0, &status);
			return handle;
		}
		return newObjectHandle(lm2s.get_randoms_sptr());
	}
	CATCH;
}

extern "C"
void* cSTIR_lm_num_prompts_exceeds_threshold(const void * ptr, const float threshold)
{
    try {
        ListmodeToSinograms& lm2s = objectFromHandle<ListmodeToSinograms>(ptr);
        return dataHandle<float>(lm2s.get_time_at_which_num_prompts_exceeds_threshold(threshold));
    }
    CATCH
}
extern "C"
void* cSTIR_setupImageDataProcessor(const void* ptr_p, void* ptr_i)
{
	try {
		DataProcessor<Image3DF>& processor =
			objectFromHandle<DataProcessor<Image3DF> >(ptr_p);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		Image3DF& image = id.data();
		processor.set_up(image);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_applyImageDataProcessor(const void* ptr_p, void* ptr_i)
{
	try {
		DataProcessor<Image3DF>& processor =
			objectFromHandle<DataProcessor<Image3DF> >(ptr_p);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		Image3DF& image = id.data();
		processor.apply(image);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_createPoissonNoiseGenerator(const float scaling_factor, const bool preserve_mean)
{
	try {
		shared_ptr<PoissonNoiseGenerator> 
			sptr(new PoissonNoiseGenerator(scaling_factor, preserve_mean));
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_generatePoissonNoise(const void* ptr_gen, const void* ptr_input)
{
	try {
		auto& generator = objectFromHandle<PoissonNoiseGenerator>(ptr_gen);
		auto& input = objectFromHandle<STIRAcquisitionData>(ptr_input);
		auto sptr_output = input.new_acquisition_data();
		generator.generate_random(*sptr_output, input);
		return newObjectHandle(sptr_output);
	}
	CATCH;
}

extern "C"
void* cSTIR_createPETAcquisitionSensitivityModel
	(const void* ptr_src, const char* src)
{
	try {
		shared_ptr<PETAcquisitionSensitivityModel> sptr;
		if (sirf::iequals(src, "s")) {
			STIRAcquisitionData& ad = objectFromHandle<STIRAcquisitionData>(ptr_src);
			sptr.reset(new PETAcquisitionSensitivityModel(ad));
		}
		else if (sirf::iequals(src, "n")) {
			CAST_PTR(DataHandle, h, ptr_src);
			sptr.reset(new PETAcquisitionSensitivityModel(charDataFromDataHandle(h)));
		}
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_createPETAttenuationModel(const void* ptr_img, const void* ptr_am)
{
	try {
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_img);
		PETAcquisitionModel& am = objectFromHandle<PETAcquisitionModel>(ptr_am);
		shared_ptr<PETAcquisitionSensitivityModel> 
			sptr(new PETAttenuationModel(id, am));
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_computeACF(const void* ptr_sino,
    const void* ptr_att, void* ptr_af, void* ptr_acf)
{
	try {
		STIRAcquisitionData& sino = objectFromHandle<STIRAcquisitionData>(ptr_sino);
		PETAttenuationModel& att = objectFromHandle<PETAttenuationModel>(ptr_att);
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_af, ptr_af);
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_acf, ptr_acf);
		PETAttenuationModel::compute_ac_factors(sino, att, sptr_af, sptr_acf);
		HANDLE_FROM_SPTR(STIRAcquisitionData, sptr_af, ptr_af);
		HANDLE_FROM_SPTR(STIRAcquisitionData, sptr_acf, ptr_acf);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_chainPETAcquisitionSensitivityModels
(const void* ptr_first, const void* ptr_second)
{
	try {
		PETAcquisitionSensitivityModel& first =
			objectFromHandle<PETAcquisitionSensitivityModel>(ptr_first);
		PETAcquisitionSensitivityModel& second =
			objectFromHandle<PETAcquisitionSensitivityModel>(ptr_second);
		shared_ptr<PETAcquisitionSensitivityModel> 
			sptr(new PETAcquisitionSensitivityModel(first, second));
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_setupAcquisitionSensitivityModel(void* ptr_sm, void* ptr_ad)
{
	try {
		PETAcquisitionSensitivityModel& sm = 
			objectFromHandle<PETAcquisitionSensitivityModel>(ptr_sm);
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_ad);
		sm.set_up(sptr_ad->get_exam_info_sptr(),
			sptr_ad->get_proj_data_info_sptr()->create_shared_clone());
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_applyAcquisitionSensitivityModel
(void* ptr_sm, void* ptr_ad, const char* job)
{
	try {
		PETAcquisitionSensitivityModel& sm =
			objectFromHandle<PETAcquisitionSensitivityModel>(ptr_sm);
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_ad);

		if (sirf::iequals(job, "fwd"))
			return newObjectHandle(sm.forward(*sptr_ad));
		else if (sirf::iequals(job, "inv"))
			return newObjectHandle(sm.invert(*sptr_ad));

		void* handle = new DataHandle;
		if (sirf::iequals(job, "unnormalise"))
			sm.unnormalise(*sptr_ad);
		else if (sirf::iequals(job, "normalise"))
			sm.normalise(*sptr_ad);
		return handle;
	}
	CATCH;
}

extern "C"
void* cSTIR_setupAcquisitionModel(void* ptr_am, void* ptr_dt, void* ptr_im)
{
	try {
		//writeText("setting up acquisition model\n");
		AcqMod3DF& am = objectFromHandle<AcqMod3DF>(ptr_am);
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_dt, ptr_dt);
		SPTR_FROM_HANDLE(STIRImageData, sptr_id, ptr_im);
		am.set_up(sptr_dt, sptr_id);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_linearAcquisitionModel(void* ptr_am)
{
	try {
		AcqMod3DF& am = objectFromHandle<AcqMod3DF>(ptr_am);
		return newObjectHandle(am.linear_acq_mod_sptr());
	}
	CATCH;
}

extern "C"
void* cSTIR_acquisitionModelNorm(void* ptr_am, int subset_num, int num_subsets, int num_iter, int verb)
{
	try {
		AcqMod3DF& am = objectFromHandle<AcqMod3DF>(ptr_am);
		return dataHandle(am.norm(subset_num, num_subsets, num_iter, verb));
	}
	CATCH;
}

extern "C"
void* cSTIR_acquisitionModelFwd
(void* ptr_am, void* ptr_im, int subset_num, int num_subsets)
{
	try {
		AcqMod3DF& am = objectFromHandle<AcqMod3DF>(ptr_am);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_im);
		return newObjectHandle(am.forward(id, subset_num, num_subsets));
	}
	CATCH;
}

extern "C"
void* cSTIR_acquisitionModelFwdReplace
(void* ptr_am, void* ptr_im, int subset_num, int num_subsets, void* ptr_ad)
{
	try {
		AcqMod3DF& am = objectFromHandle<AcqMod3DF>(ptr_am);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_im);
		STIRAcquisitionData& ad = objectFromHandle<STIRAcquisitionData>(ptr_ad);
		am.forward(ad, id, subset_num, num_subsets, num_subsets > 1);
		return new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_acquisitionModelBwd(void* ptr_am, void* ptr_ad, 
	int subset_num, int num_subsets)
{
	try {
		AcqMod3DF& am = objectFromHandle<AcqMod3DF>(ptr_am);
		STIRAcquisitionData& ad = objectFromHandle<STIRAcquisitionData>(ptr_ad);
		return newObjectHandle(am.backward(ad, subset_num, num_subsets));
	}
	CATCH;
}

extern "C"
void* cSTIR_setupSPECTUBMatrix
(const void* h_smx, const void* h_acq, const void* h_img)
{
	try {
		SPECTUBMatrix& matrix = objectFromHandle<SPECTUBMatrix>(h_smx);
		PETAcquisitionData& acq = objectFromHandle<PETAcquisitionData>(h_acq);
		STIRImageData& img = objectFromHandle<STIRImageData>(h_img);
		matrix.set_up(acq.get_proj_data_info_sptr(), img.data_sptr());
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_SPECTUBMatrixSetResolution
	(const void* ptr_acq_matrix,
         const float collimator_sigma_0_in_mm, const float collimator_slope_in_mm, const bool full_3D)
{
	try {
                SPECTUBMatrix& matrix = objectFromHandle<SPECTUBMatrix>(ptr_acq_matrix);
                matrix.set_resolution_model(collimator_sigma_0_in_mm, collimator_slope_in_mm, full_3D);
                return (void*)new DataHandle;
        }
        CATCH;
}

extern "C"
void* cSTIR_acquisitionModelBwdReplace(void* ptr_am, void* ptr_ad,
	int subset_num, int num_subsets, void* ptr_im)
{
	try {
		AcqMod3DF& am = objectFromHandle<AcqMod3DF>(ptr_am);
		STIRAcquisitionData& ad = objectFromHandle<STIRAcquisitionData>(ptr_ad);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_im);
		am.backward(id, ad, subset_num, num_subsets);
		return new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_get_MatrixInfo(void* ptr)
{
	try {
		SPTR_FROM_HANDLE(RayTracingMatrix, sptr, ptr);
		return charDataHandleFromCharData(
			sptr->parameter_info().c_str());
	}
	CATCH;
}

extern "C"
void* cSTIR_acquisitionDataFromListmode(void* ptr_t)
{
	try {
                SPTR_FROM_HANDLE(STIRListmodeData, sptr_t, ptr_t);
                auto sptr(sptr_t->acquisition_data_template());
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void*
cSTIR_setAcquisitionDataStorageScheme(const char* scheme)
{ 
	try {
		if (scheme[0] == 'f' || strcmp(scheme, "default") == 0)
			STIRAcquisitionDataInFile::set_as_template();
		else
			STIRAcquisitionDataInMemory::set_as_template();
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void*
cSTIR_getAcquisitionDataStorageScheme()
{
	return charDataHandleFromCharData
		(STIRAcquisitionData::storage_scheme().c_str());
}

extern "C"
void* cSTIR_acquisitionDataFromTemplate(void* ptr_t)
{
	try {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_t, ptr_t);
                std::shared_ptr<STIRAcquisitionData> sptr(sptr_t->new_acquisition_data());
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_cloneAcquisitionData(void* ptr_ad)
{
	try {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_ad);
                std::shared_ptr<STIRAcquisitionData> sptr(sptr_ad->clone());
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_rebinnedAcquisitionData(void* ptr_t, 
const int num_segments_to_combine,
const int num_views_to_combine,
const int num_tang_poss_to_trim,
const bool do_normalisation,
const int max_in_segment_num_to_process,
const int num_tof_bins_to_combine
)
{
	try {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_t, ptr_t);
                std::shared_ptr<STIRAcquisitionData> sptr =
			sptr_t->single_slice_rebinned_data(
			num_segments_to_combine,
			num_views_to_combine,
			num_tang_poss_to_trim,
			do_normalisation,
			max_in_segment_num_to_process,
			num_tof_bins_to_combine
		);
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_acquisitionDataFromScannerInfo
(const char* scanner, int span, int max_ring_diff, int view_mash_factor, int tof_mash_factor)
{
	try{
                stir::shared_ptr<ExamInfo> sptr_ei(new ExamInfo());
        sptr_ei->imaging_modality = ImagingModality::PT;
		stir::shared_ptr<stir::ProjDataInfo> sptr_pdi =
			STIRAcquisitionData::proj_data_info_from_scanner
			(scanner, span, max_ring_diff, view_mash_factor);
#if STIR_VERSION >= 050000
                sptr_pdi->set_tof_mash_factor(tof_mash_factor);
#endif
		STIRAcquisitionDataInFile::init();
		std::shared_ptr<STIRAcquisitionData> sptr_t =
			STIRAcquisitionData::storage_template();
		std::shared_ptr<STIRAcquisitionData> sptr(sptr_t->same_acquisition_data
			(sptr_ei, sptr_pdi));
		sptr->fill(0.0f);
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_getAcquisitionDataDimensions(const void* ptr_acq, size_t ptr_dim)
{
	try {
		int* dim = (int*)ptr_dim;
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_acq);
		dim[0] = sptr_ad->get_num_tangential_poss();
		dim[1] = sptr_ad->get_num_views();
		dim[2] = sptr_ad->get_num_non_TOF_sinograms();
		dim[3] = sptr_ad->get_num_TOF_bins();
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_getAcquisitionData(const void* ptr_acq, size_t ptr_data)
{
	try {
		float* data = (float*)ptr_data;
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_acq);
		if (sptr_ad->is_empty())
			return DataHandle::error_handle(
				"Failed to get acquisition data: dealing with empty template?",
				__FILE__, __LINE__);
		sptr_ad->copy_to(data);
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_fillAcquisitionData(void* ptr_acq, float v)
{
	try {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_acq);
		sptr_ad->fill(v);
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_fillAcquisitionDataFromAcquisitionData
(void* ptr_acq, const void* ptr_from)
{
	try {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_acq);
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_from, ptr_from);
		sptr_ad->fill(*sptr_from);
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_setAcquisitionData(void* ptr_acq, size_t ptr_data)
{
	try {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_acq);
		float *data = (float *)ptr_data;
		sptr_ad->fill_from(data);
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_writeAcquisitionData(void* ptr_acq, const char* filename)
{
	try {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_acq);
		sptr_ad->write(filename);
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_get_info(void* ptr_cont)
{
	try {
		std::string ret;
		SPTR_FROM_HANDLE(ContainerBase, sptr_cont, ptr_cont);
		if (auto sptr_ad = std::dynamic_pointer_cast<STIRAcquisitionData>(sptr_cont)) {
			ret = sptr_ad->get_info();
		}
		else if (auto sptr_ld = std::dynamic_pointer_cast<STIRListmodeData>(sptr_cont)) {
			ret = sptr_ld->get_info();
		}
		else if (auto sptr_id = std::dynamic_pointer_cast<STIRImageData>(sptr_cont)) {
			ret = sptr_id->get_info();
		}
		else
		        ret =  "get_info() not supported for this type";
		return charDataHandleFromCharData(
			ret.c_str());
	}
	CATCH;
}

extern "C"
void* cSTIR_get_subset(void* ptr_acq, int nv, size_t ptr_views)
{
	try {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_acq);
		int* ptr_v = (int*)ptr_views;
		std::vector<int> v(ptr_v, ptr_v + nv);
		std::shared_ptr<STIRAcquisitionData> sptr = std::move(sptr_ad->get_subset(v));
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_setupFBP2DReconstruction(void* ptr_r, void* ptr_i)
{
	try {
		DataHandle* handle = new DataHandle;
		xSTIR_FBP2DReconstruction& recon =
			objectFromHandle< xSTIR_FBP2DReconstruction >(ptr_r);
		SPTR_FROM_HANDLE(STIRImageData, sptr_id, ptr_i);
		recon.set_up(sptr_id);
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_runFBP2DReconstruction(void* ptr_r)
{
	try {
		DataHandle* handle = new DataHandle;
		xSTIR_FBP2DReconstruction& recon =
			objectFromHandle< xSTIR_FBP2DReconstruction >(ptr_r);
		recon.process();
		return (void*)new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_setupReconstruction(void* ptr_r, void* ptr_i)
{
	try {
		DataHandle* handle = new DataHandle;
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		sptrImage3DF sptr_image = id.data_sptr();
		xSTIR_IterativeReconstruction3DF& recon =
			objectFromHandle<xSTIR_IterativeReconstruction3DF>(ptr_r);
		Succeeded s = Succeeded::no;
		s = recon.set_up(sptr_image);
		recon.subiteration() = recon.get_start_subiteration_num();
		if (s != Succeeded::yes) {
			ExecutionStatus status("cSTIR_setupReconstruction failed",
				__FILE__, __LINE__);
			handle->set(0, &status);
		}
		return (void*)handle;
	}
	CATCH;
}

extern "C"
void* cSTIR_runReconstruction(void* ptr_r, void* ptr_i) 
{
	try {
		DataHandle* handle = new DataHandle;
		Reconstruction<Image3DF>& recon =
			objectFromHandle< Reconstruction<Image3DF> >(ptr_r);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		sptrImage3DF sptr_image = id.data_sptr();
		if (recon.reconstruct(sptr_image) != Succeeded::yes) {
			ExecutionStatus status("cSTIR_reconstruct failed",
				__FILE__, __LINE__);
			handle->set(0, &status);
		}
		return (void*)handle;
	}
	CATCH;
}

extern "C"
void* cSTIR_updateReconstruction(void* ptr_r, void* ptr_i)
{
	try {
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		Image3DF& image = id.data();
		xSTIR_IterativeReconstruction3DF& recon =
			objectFromHandle<xSTIR_IterativeReconstruction3DF>(ptr_r);
		recon.update(image);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_setupObjectiveFunction(void* ptr_r, void* ptr_i)
{
	try {
		DataHandle* handle = new DataHandle;
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		sptrImage3DF sptr_image = id.data_sptr();
		xSTIR_GeneralisedObjectiveFunction3DF& obj_fun =
			objectFromHandle<xSTIR_GeneralisedObjectiveFunction3DF>(ptr_r);
		Succeeded s = Succeeded::no;
		s = obj_fun.set_up(sptr_image);
		if (s != Succeeded::yes) {
			ExecutionStatus status("cSTIR_setupObjectiveFunction failed",
				__FILE__, __LINE__);
			handle->set(0, &status);
		}
		return (void*)handle;
	}
	CATCH;
}

extern "C"
void*
cSTIR_objectiveFunctionValue(void* ptr_f, void* ptr_i)
{
	try {
		ObjectiveFunction3DF& fun = objectFromHandle< ObjectiveFunction3DF>(ptr_f);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		Image3DF& image = id.data();
		double v = fun.compute_objective_function(image);
		return dataHandle<double>(v);
	}
	CATCH;
}

extern "C"
void*
cSTIR_subsetSensitivity(void* ptr_f, int subset)
{
	try {
		PoissonLogLhLinModMean3DF& fun =
			objectFromHandle<PoissonLogLhLinModMean3DF>(ptr_f);
		const Image3DF& s = fun.get_subset_sensitivity(subset);
		STIRImageData* ptr_id = new STIRImageData(s);
		shared_ptr<STIRImageData> sptr(ptr_id);
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void*
cSTIR_objectiveFunctionGradient(void* ptr_f, void* ptr_i, int subset)
{
	try {
		auto& fun = objectFromHandle<xSTIR_ObjFun3DF>(ptr_f);
		auto& id = objectFromHandle<STIRImageData>(ptr_i);
		auto sptr_gd = std::make_shared<STIRImageData>(id);
		fun.compute_gradient(id, subset, *sptr_gd);
		return newObjectHandle(sptr_gd);
	}
	CATCH;
}

extern "C"
void*
cSTIR_computeObjectiveFunctionGradient(void* ptr_f, void* ptr_i, int subset, void* ptr_g)
{
	try {
		xSTIR_ObjFun3DF& fun = objectFromHandle<xSTIR_ObjFun3DF>(ptr_f);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		STIRImageData& gd = objectFromHandle<STIRImageData>(ptr_g);
		fun.compute_gradient(id, subset, gd);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void*
cSTIR_objectiveFunctionGradientNotDivided(void* ptr_f, void* ptr_i, int subset)
{
	try {
		PoissonLogLhLinModMean3DF& fun = 
			objectFromHandle<PoissonLogLhLinModMean3DF>(ptr_f);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		Image3DF& image = id.data();
		shared_ptr<STIRImageData> sptr(new STIRImageData(image));
		Image3DF& grad = sptr->data();
		fun.compute_sub_gradient_without_penalty_plus_sensitivity
			(grad, image, subset);
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void*
cSTIR_computeObjectiveFunctionGradientNotDivided(void* ptr_f, void* ptr_i, int subset, void* ptr_g)
{
	try {
		PoissonLogLhLinModMean3DF& fun =
			objectFromHandle<PoissonLogLhLinModMean3DF>(ptr_f);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		STIRImageData& gd = objectFromHandle<STIRImageData>(ptr_g);
		Image3DF& image = id.data();
		Image3DF& grad = gd.data();
		fun.compute_sub_gradient_without_penalty_plus_sensitivity
			(grad, image, subset);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void*
cSTIR_objectiveFunctionAccumulateHessianTimesInput
    (void* ptr_fun, void* ptr_est, void* ptr_inp, int subset, void* ptr_out)
{
	try {
		auto& fun = objectFromHandle<ObjectiveFunction3DF>(ptr_fun);
		auto& est = objectFromHandle<STIRImageData>(ptr_est);
		auto& inp = objectFromHandle<STIRImageData>(ptr_inp);
		auto& out = objectFromHandle<STIRImageData>(ptr_out);
		auto& curr_est = est.data();
		auto& input    = inp.data();
		auto& output   = out.data();
		if (subset >= 0)
			fun.accumulate_sub_Hessian_times_input(output, curr_est, input, subset);
		else {
			for (int s = 0; s < fun.get_num_subsets(); s++) {
				fun.accumulate_sub_Hessian_times_input(output, curr_est, input, s);
			}
		}
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void*
cSTIR_objectiveFunctionComputeHessianTimesInput
    (void* ptr_fun, void* ptr_est, void* ptr_inp, int subset, void* ptr_out)
{
	try {
		auto& fun = objectFromHandle<xSTIR_GeneralisedObjectiveFunction3DF>(ptr_fun);
		auto& est = objectFromHandle<STIRImageData>(ptr_est);
		auto& inp = objectFromHandle<STIRImageData>(ptr_inp);
		auto& out = objectFromHandle<STIRImageData>(ptr_out);
		auto& curr_est = est.data();
		auto& input    = inp.data();
		auto& output   = out.data();
		fun.multiply_with_Hessian(output, curr_est, input, subset);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void*
cSTIR_setupPrior(void* ptr_p, void* ptr_i)
{
	try {
		DataHandle* handle = new DataHandle;
		xSTIR_GeneralisedPrior3DF& prior =
			objectFromHandle<xSTIR_GeneralisedPrior3DF>(ptr_p);
		STIRImageData& image = objectFromHandle<STIRImageData>(ptr_i);
		sptrImage3DF sptr_img = image.data_sptr();
		// empty image is a temporary measure for compatibility with old scripts
		// (valid for as long as the argument of prior.set_up() is not used)
		//sptrImage3DF sptr_img(new Voxels3DF);
		prior.set_up(sptr_img);
		return handle;
	}
	CATCH;
}

extern "C"
void*
cSTIR_priorValue(void* ptr_p, void* ptr_i)
{
	try {
		xSTIR_GeneralisedPrior3DF& prior =
			objectFromHandle<xSTIR_GeneralisedPrior3DF>(ptr_p);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		Image3DF& image = id.data();
		double v = prior.compute_value(image);
		return dataHandle<double>(v);
	}
	CATCH;
}

extern "C"
void*
cSTIR_priorGradient(void* ptr_p, void* ptr_i)
{
	try {
		Prior3DF& prior = objectFromHandle<stir::GeneralisedPrior <Image3DF> >(ptr_p);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		Image3DF& image = id.data();
		shared_ptr<STIRImageData> sptr(new STIRImageData(image));
		Image3DF& grad = sptr->data();
		prior.compute_gradient(grad, image);
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void*
cSTIR_priorAccumulateHessianTimesInput(void* ptr_prior, void* ptr_out, void* ptr_cur, void* ptr_inp)
{
	try {
		auto& prior = objectFromHandle<stir::GeneralisedPrior <Image3DF> >(ptr_prior);
		auto& out = objectFromHandle<STIRImageData>(ptr_out);
		auto& cur = objectFromHandle<STIRImageData>(ptr_cur);
		auto& inp = objectFromHandle<STIRImageData>(ptr_inp);
		auto& output  = out.data();
		auto& current = cur.data();
		auto& input   = inp.data();
		prior.accumulate_Hessian_times_input(output, current, input);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void*
cSTIR_priorComputeHessianTimesInput(void* ptr_prior, void* ptr_out, void* ptr_cur, void* ptr_inp)
{
	try {
		auto& prior = objectFromHandle<xSTIR_GeneralisedPrior3DF>(ptr_prior);
		auto& out = objectFromHandle<STIRImageData>(ptr_out);
		auto& cur = objectFromHandle<STIRImageData>(ptr_cur);
		auto& inp = objectFromHandle<STIRImageData>(ptr_inp);
		auto& output  = out.data();
		auto& current = cur.data();
		auto& input   = inp.data();
		prior.multiply_with_Hessian(output, current, input);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void*
cSTIR_computePriorGradient(void* ptr_p, void* ptr_i, void* ptr_g)
{
	try {
		Prior3DF& prior = objectFromHandle<Prior3DF>(ptr_p);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		STIRImageData& gd = objectFromHandle<STIRImageData>(ptr_g);
		Image3DF& image = id.data();
		Image3DF& grad = gd.data();
		prior.compute_gradient(grad, image);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void*
cSTIR_PLSPriorAnatomicalGradient(void* ptr_p, int dir)
{
	try {
		PLSPrior<float>& prior = objectFromHandle<PLSPrior<float> >(ptr_p);
		auto sptr_im = prior.get_anatomical_grad_sptr(dir);
		auto sptr_id = std::make_shared<STIRImageData>(*sptr_im);
		return newObjectHandle(sptr_id);
	}
	CATCH;
}

extern "C"
void* cSTIR_voxels3DF
(int nx, int ny, int nz,
float sx, float sy, float sz,
float x, float y, float z)
{
	try {
		shared_ptr<Voxels3DF> sptr(new Voxels3DF(IndexRange3D(0, nz - 1,
			-(ny / 2), -(ny / 2) + ny - 1, -(nx / 2), -(nx / 2) + nx - 1),
			Coord3DF(z, y, x),
			Coord3DF(sz, sy, sx)));
		sptr->fill(0.0);
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_imageFromVoxels(void* ptr_v)
{
	try {
		Voxels3DF& voxels = objectFromHandle<Voxels3DF>(ptr_v);
		shared_ptr<STIRImageData> sptr(new STIRImageData(voxels));
		return (void*)newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_imageFromImageData(void* ptr_v)
{
	try {
		ImageData& id = objectFromHandle<ImageData>(ptr_v);
		shared_ptr<STIRImageData> sptr(new STIRImageData(id));
		return (void*)newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_imageFromImage(void* ptr_i)
{
	try {
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		Image3DF& image = id.data();
		shared_ptr<STIRImageData> sptr(new STIRImageData(image));
		return (void*)newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_writeImage(void* ptr_i, const char* filename)
{
	try {
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		id.write(filename);
		return (void*) new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_writeImage_par(void* ptr_i, const char* filename, const char* par)
{
    try {
        STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
        id.write(filename,par);
        return (void*) new DataHandle;
	}
    CATCH;
}

extern "C"
void* cSTIR_ImageData_zoom_image(void* ptr_im, const size_t zooms_ptr_raw, const size_t offsets_in_mm_ptr_raw,
                                 const size_t new_sizes_ptr_raw, const char *const zoom_options)
{
    try {
        STIRImageData& id = objectFromHandle<STIRImageData>(ptr_im);

        const float* zooms_ptr         = (const float*)zooms_ptr_raw;
        const float* offsets_in_mm_ptr = (const float*)offsets_in_mm_ptr_raw;
        const  int*  new_sizes_ptr     = (const  int* )new_sizes_ptr_raw;

        Coord3DF zooms(zooms_ptr[0],zooms_ptr[1],zooms_ptr[2]);
        Coord3DF offsets_in_mm(offsets_in_mm_ptr[0],offsets_in_mm_ptr[1],offsets_in_mm_ptr[2]);
        Coord3DI new_sizes(new_sizes_ptr[0],new_sizes_ptr[1],new_sizes_ptr[2]);

        id.zoom_image(zooms, offsets_in_mm, new_sizes, zoom_options);

		return static_cast<void*>(new DataHandle);
	}
	CATCH;
}

extern "C"
void* cSTIR_ImageData_zoom_image_as_template(void* zoomed_image_ptr, const void* template_image_ptr, 
                                                const char *const zoom_options) 
{
    try {
        STIRImageData& zoomed_id = objectFromHandle<STIRImageData>(zoomed_image_ptr);
        STIRImageData& template_id = objectFromHandle<STIRImageData>(template_image_ptr);

        // Use the in_id image as the template for zooming
        zoomed_id.zoom_image_as_template(template_id, zoom_options);

        return static_cast<void*>(new DataHandle);
	}
	CATCH;
}

extern "C"
void* cSTIR_ImageData_move_to_scanner_centre(void* im_ptr, const void* acq_data_ptr)
{
    try {
        STIRImageData& im = objectFromHandle<STIRImageData>(im_ptr);
        STIRAcquisitionData& ad = objectFromHandle<STIRAcquisitionData>(acq_data_ptr);
        im.move_to_scanner_centre(ad);

        return static_cast<void*>(new DataHandle);
	}
	CATCH;

}

extern "C"
void* cSTIR_imageFromAcquisitionData(void* ptr_ad)
{
	try {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_ad);
		shared_ptr<STIRImageData> sptr(new STIRImageData(*sptr_ad));
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_imageFromAcquisitionDataAndNxNy(void* ptr_ad, int nx, int ny)
{
	try {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, ptr_ad);
		STIRImageData id(*sptr_ad);
		int dim[3];
		float vs[3];
		float is[3];
		id.get_dimensions(dim);
		id.get_voxel_sizes(vs);
		for (int i = 0; i < 3; i++)
			is[i] = dim[i] * vs[i];
		int nz = dim[0];
		float vx = is[2] / nx;
		float vy = is[1] / ny;
		float vz = vs[0];
		shared_ptr<Voxels3DF> sptr_v(new Voxels3DF(IndexRange3D(0, nz - 1,
			-(ny / 2), -(ny / 2) + ny - 1, -(nx / 2), -(nx / 2) + nx - 1),
			Coord3DF(0, 0, 0),
			Coord3DF(vz, vy, vx)));
		shared_ptr<STIRImageData> sptr(new STIRImageData(*sptr_v));
		sptr->fill(0.0);
		return newObjectHandle(sptr);
	}
	CATCH;
}

extern "C"
void* cSTIR_addShape(void* ptr_i, void* ptr_s, float v, int num_samples_in_each_direction)
{
	try {
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		Image3DF& image = id.data();
		sptrVoxels3DF sptr_v((Voxels3DF*)image.clone());
		Voxels3DF& voxels = *sptr_v;
		Shape3D& shape = objectFromHandle<Shape3D>(ptr_s);
		CartesianCoordinate3D<int> num_samples(
			num_samples_in_each_direction,
			num_samples_in_each_direction,
			num_samples_in_each_direction);
		voxels.fill(0);
		shape.construct_volume(voxels, num_samples);
		voxels *= v;
		image += voxels;
		return new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_fillImage(void* ptr_i, float v)
{
	try {
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		Image3DF& image = id.data();
		image.fill(v);
		return new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_getImageDimensions(const void* ptr_im, size_t ptr_dim) 
{
	try {
		int* dim = (int*)ptr_dim;
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_im);
		if (id.get_dimensions(dim)) {
				ExecutionStatus status("not a regular image", __FILE__, __LINE__);
				DataHandle* handle = new DataHandle;
				handle->set(0, &status);
				return (void*)handle;
		}
		return new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_getImageVoxelSizes(const void* ptr_im, size_t ptr_vs)
{
	try {
		float* vs = (float*)ptr_vs;
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_im);
		id.get_voxel_sizes(vs);
		return new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_getImageTransformMatrix(const void* ptr_im, size_t ptr_md)
{
	try {
        STIRImageData& id = objectFromHandle<STIRImageData>(ptr_im);
		float* data = (float*)ptr_md;
		TransformMatrix3D mx = id.get_geom_info_sptr()->calculate_index_to_physical_point_matrix();
		for (int j = 0; j < 4; j++)
			for (int i = 0; i < 4; i++)
				data[i + 4 * j] = mx[j][i];
		return new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_getImageData(const void* ptr_im, size_t ptr_data)
{
	try {
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_im);
		float* data = (float*)ptr_data;
		id.get_data(data);
		return new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_setImageData(void* ptr_im, size_t ptr_data)
{
	try {
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_im);
		float* data = (float*)ptr_data;
		id.set_data(data);
		return new DataHandle;
	}
	CATCH;
}

extern "C"
void* cSTIR_setImageDataFromImage(void* ptr_im, const void* ptr_src)
{
	try {
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_im);
		STIRImageData& id_src = objectFromHandle<STIRImageData>(ptr_src);
		Image3DF& data = id.data();
		data = id_src.data();
		return new DataHandle;
	}
	CATCH;
}

#ifdef USE_HKEM
extern "C"
void* cSTIR_computeKernelisedImage(void* ptr_r, void* ptr_i, void* ptr_a)
{
	try {
		xSTIR_KOSMAPOSLReconstruction3DF& recon =
			objectFromHandle<xSTIR_KOSMAPOSLReconstruction3DF>(ptr_r);
		STIRImageData& id = objectFromHandle<STIRImageData>(ptr_i);
		Image3DF& image = id.data();
		shared_ptr<STIRImageData> sptr_ki(new STIRImageData(id));
		STIRImageData& ki = *sptr_ki;
		Image3DF& kernelised_image = ki.data();
		STIRImageData& ad = objectFromHandle<STIRImageData>(ptr_a);
		Image3DF& alpha = ad.data();
		recon.compute_kernelised_image_x(kernelised_image, image, alpha);
		return (void*)newObjectHandle(sptr_ki);
	}
	CATCH;
}
#endif

//extern "C"
//void* setParameter
//(void* ptr_s, const char* obj, const char* name, const void* ptr_v)
//{
//	return cSTIR_setParameter(ptr_s, obj, name, ptr_v);
//}
//
//extern "C"
//void* parameter(const void* ptr, const char* obj, const char* name)
//{
//	return cSTIR_parameter(ptr, obj, name);
//}
