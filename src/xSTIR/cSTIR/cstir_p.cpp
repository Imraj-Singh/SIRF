/*
SyneRBI Synergistic Image Reconstruction Framework (SIRF)
Copyright 2015 - 2020 Rutherford Appleton Laboratory STFC
Copyright 2018 - 2020, 2024 UCL

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

#include <stdio.h>
#include <stdlib.h>

#include "sirf/STIR/stir_types.h"
#include "sirf/iUtilities/DataHandle.h"
#include "sirf/STIR/cstir_p.h"
#include "sirf/STIR/stir_x.h"

using namespace stir;
using namespace sirf;

#define SPTR_FROM_HANDLE(Object, X, H) \
  std::shared_ptr<Object> X; getObjectSptrFromHandle<Object>(H, X);
#define STIRSPTR_FROM_HANDLE(Object, X, H) \
  stir::shared_ptr<Object> X; getObjectSptrFromHandle<Object>(H, X);
#define SIRF_DYNAMIC_CAST(T, X, Y) T& X = dynamic_cast<T&>(Y)

extern "C"
char* charDataFromHandle(const void* ptr);

extern "C"
void* charDataHandle(const char* ptr);

static void*
handle_error(const std::string& error_string, const char* file, int line)
{
	DataHandle* handle = new DataHandle;
	ExecutionStatus status(error_string.c_str(), file, line);
	handle->set(0, &status);
	return (void*)handle;
}

// TODO write below in terms of handle_error() function above
static void*
parameterNotFound(const char* name, const char* file, int line)
{
	DataHandle* handle = new DataHandle;
	std::string error = "parameter ";
	error += name;
	error += " not found";
	ExecutionStatus status(error.c_str(), file, line);
	handle->set(0, &status);
	return (void*)handle;
}

static void*
wrongParameterValue
(const char* name, const char* value, const char* file, int line)
{
	DataHandle* handle = new DataHandle;
	std::string error = "parameter ";
	error += name;
	error += " cannot be ";
	//error += " has wrong value ";
	error += value;
	ExecutionStatus status(error.c_str(), file, line);
	handle->set(0, &status);
	return (void*)handle;
}

static void*
wrongIntParameterValue
(const char* name, int value, const char* file, int line)
{
	char buff[32];
	sprintf(buff, "%d", value);
	return wrongParameterValue(name, buff, file, line);
}

static void*
wrongFloatParameterValue
(const char* name, float value, const char* file, int line)
{
	char buff[32];
	sprintf(buff, "%f", value);
	return wrongParameterValue(name, buff, file, line);
}

void*
sirf::cSTIR_AcquisitionDataParameter(void* hp, const char* name)
{
	STIRAcquisitionData& ad = objectFromHandle<STIRAcquisitionData>(hp);
	if (sirf::iequals(name, "tof_mash_factor"))
		return dataHandle<int>(ad.get_tof_mash_factor());
	if (sirf::iequals(name, "modality"))
		return charDataHandleFromCharData(ad.modality().c_str());
	else if (sirf::iequals(name, "address"))
		return dataHandle<size_t>(ad.address());
	else
		return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setImageDataParameter(void* hp, const char* name, const void* hv)
{
	STIRImageData& id = objectFromHandle<STIRImageData>(hp);
	if (sirf::iequals(name, "modality"))
		id.set_modality(charDataFromHandle(hv));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_ImageDataParameter(void* hp, const char* name)
{
	STIRImageData& id = objectFromHandle<STIRImageData>(hp);
	if (sirf::iequals(name, "modality"))
		return charDataHandleFromCharData(id.modality().c_str());
	else if (sirf::iequals(name, "address"))
		return dataHandle<size_t>(id.address());
	else
		return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setListmodeToSinogramsParameter(void* hp, const char* name, const void* hv)
{
	ListmodeToSinograms& lm2s = objectFromHandle<ListmodeToSinograms>(hp);
	if (sirf::iequals(name, "input_file"))
		lm2s.set_input(charDataFromHandle(hv));
	else if (sirf::iequals(name, "input"))
		lm2s.set_input(objectFromHandle<STIRListmodeData>(hv));
	else if (sirf::iequals(name, "output"))
		lm2s.set_output(charDataFromHandle(hv));
	else if (sirf::iequals(name, "template_file"))
		lm2s.set_template(charDataFromHandle(hv));
	else if (sirf::iequals(name, "template"))
		lm2s.set_template(objectFromHandle<STIRAcquisitionData>(hv));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_setSeparableGaussianImageFilterParameter
(void* hp, const char* name, const void* hv)
{
	xSTIR_SeparableGaussianImageFilter& f =
		objectFromHandle<xSTIR_SeparableGaussianImageFilter>(hp);
	if (sirf::iequals(name, "fwhms_x")) {
		float v = dataFromHandle<float>(hv);
		f.set_fwhms_xyz('x', v);
	}
	else if (sirf::iequals(name, "fwhms_y")) {
		float v = dataFromHandle<float>(hv);
		f.set_fwhms_xyz('y', v);
	}
	else if (sirf::iequals(name, "fwhms_z")) {
		float v = dataFromHandle<float>(hv);
		f.set_fwhms_xyz('z', v);
	}
	else if (sirf::iequals(name, "max_kernel_size_x")) {
		int mks = dataFromHandle<int>(hv);
		f.set_max_kernel_sizes_xyz('x', mks);
	}
	else if (sirf::iequals(name, "max_kernel_size_y")) {
		int mks = dataFromHandle<int>(hv);
		f.set_max_kernel_sizes_xyz('y', mks);
	}
	else if (sirf::iequals(name, "max_kernel_size_z")) {
		int mks = dataFromHandle<int>(hv);
		f.set_max_kernel_sizes_xyz('z', mks);
	}
	else if (sirf::iequals(name, "normalise")) {
		bool normalise = dataFromHandle<int>(hv);
		f.set_normalise(normalise);
	}
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_setShapeParameter(void* hp, const char* name, const void* hv)
{
	Shape3D& s = objectFromHandle<Shape3D>(hp);
	Coord3DF origin = s.get_origin();
	float value = dataFromHandle<float>(hv);
	if (sirf::iequals(name, "x"))
		origin.x() = value;
	else if (sirf::iequals(name, "y"))
		origin.y() = value;
	else if (sirf::iequals(name, "z"))
		origin.z() = value;
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	s.set_origin(origin);
	return new DataHandle;
}

void*
sirf::cSTIR_shapeParameter(const DataHandle* handle, const char* name)
{
	Shape3D& s = objectFromHandle<Shape3D>(handle);
	Coord3DF origin = s.get_origin();
	if (sirf::iequals(name, "x"))
		return dataHandle<float>(origin.x());
	else if (sirf::iequals(name, "y"))
		return dataHandle<float>(origin.y());
	else if (sirf::iequals(name, "z"))
		return dataHandle<float>(origin.z());
	else
		return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setBox3DParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	xSTIR_Box3D& box =
		objectFromHandle<xSTIR_Box3D>(hp);
	float value = dataFromHandle<float>(hv);
	if (sirf::iequals(name, "length_x"))
		box.set_length_x(value);
	else if (sirf::iequals(name, "length_y"))
		box.set_length_y(value);
	else if (sirf::iequals(name, "length_z"))
		box.set_length_z(value);
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_Box3DParameter(const DataHandle* handle, const char* name)
{
	xSTIR_Box3D& box =
		objectFromHandle<xSTIR_Box3D>(handle);
	if (sirf::iequals(name, "length_x"))
		return dataHandle<float>(box.get_length_x());
	if (sirf::iequals(name, "length_y"))
		return dataHandle<float>(box.get_length_y());
	if (sirf::iequals(name, "length_z"))
		return dataHandle<float>(box.get_length_z());
	return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setEllipsoidParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	Ellipsoid& c =
		objectFromHandle<Ellipsoid>(hp);
	float value = dataFromHandle<float>(hv);
	float x = c.get_radius_x();
	float y = c.get_radius_y();
	float z = c.get_radius_z();
	if (sirf::iequals(name, "radius_x"))
		x = value;
	else if (sirf::iequals(name, "radius_y"))
		y = value;
	else if (sirf::iequals(name, "radius_z"))
		z = value;
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	c.set_radii(Coord3DF(x, y, z));
	return new DataHandle;
}

void*
sirf::cSTIR_ellipsoidParameter(const DataHandle* handle, const char* name)
{
	Ellipsoid& c =
		objectFromHandle<Ellipsoid>(handle);
	if (sirf::iequals(name, "radius_x"))
		return dataHandle<float>(c.get_radius_x());
	if (sirf::iequals(name, "radius_y"))
		return dataHandle<float>(c.get_radius_y());
	if (sirf::iequals(name, "radius_z"))
		return dataHandle<float>(c.get_radius_z());
	return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setEllipsoidalCylinderParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	EllipsoidalCylinder& c =
		objectFromHandle<EllipsoidalCylinder>(hp);
	float value = dataFromHandle<float>(hv);
	if (sirf::iequals(name, "length"))
		c.set_length(value);
	else if (sirf::iequals(name, "radius_x"))
		c.set_radius_x(value);
	else if (sirf::iequals(name, "radius_y"))
		c.set_radius_y(value);
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_ellipsoidalCylinderParameter(const DataHandle* handle, const char* name)
{
	EllipsoidalCylinder& c =
		objectFromHandle<EllipsoidalCylinder>(handle);
	if (sirf::iequals(name, "length"))
		return dataHandle<float>(c.get_length());
	if (sirf::iequals(name, "radius_x"))
		return dataHandle<float>(c.get_radius_x());
	if (sirf::iequals(name, "radius_y"))
		return dataHandle<float>(c.get_radius_y());
	return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setTruncateToCylindricalFOVImageProcessorParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	CylindricFilter3DF& filter =
		objectFromHandle<CylindricFilter3DF>(hp);
	if (sirf::iequals(name, "strictly_less_than_radius"))
		filter.set_strictly_less_than_radius
			(sirf::iequals(charDataFromDataHandle(hv), "true"));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_truncateToCylindricalFOVImageProcessorParameter
(const DataHandle* handle, const char* name)
{
	CylindricFilter3DF& filter =
		objectFromHandle<CylindricFilter3DF>(handle);
	if (sirf::iequals(name, "strictly_less_than_radius"))
		return dataHandle<int>(filter.get_strictly_less_than_radius());
	else
		return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setRayTracingMatrixParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	RayTracingMatrix& matrix =
		objectFromHandle<RayTracingMatrix>(hp);
	if (sirf::iequals(name, "num_tangential_LORs"))
          {
            	const auto value = dataFromHandle<int>(hv);
                matrix.set_num_tangential_LORs(value);
          }
	else
          {
            const auto value = dataFromHandle<bool>(hv);
            if (sirf::iequals(name, "enable_cache"))
              matrix.enable_cache(value);
            else if (sirf::iequals(name, "restrict_to_cylindrical_FOV"))
              matrix.set_restrict_to_cylindrical_FOV(value);
            else if (sirf::iequals(name, "do_symmetry_90degrees_min_phi"))
              matrix.set_do_symmetry_90degrees_min_phi(value);
            else if (sirf::iequals(name, "do_symmetry_180degrees_min_phi"))
              matrix.set_do_symmetry_180degrees_min_phi(value);
            else if (sirf::iequals(name, "do_symmetry_swap_segment"))
              matrix.set_do_symmetry_swap_segment(value);
            else if (sirf::iequals(name, "do_symmetry_swap_s"))
		matrix.set_do_symmetry_swap_s(value);
            else if (sirf::iequals(name, "do_symmetry_shift_z"))
		matrix.set_do_symmetry_shift_z(value);
            else
		return parameterNotFound(name, __FILE__, __LINE__);
          }
	return new DataHandle;
}

void*
sirf::cSTIR_rayTracingMatrixParameter(const DataHandle* handle, const char* name)
{
	RayTracingMatrix& matrix =
		objectFromHandle<RayTracingMatrix>(handle);
	if (sirf::iequals(name, "num_tangential_LORs"))
		return dataHandle<int>(matrix.get_num_tangential_LORs());
	return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setSPECTUBMatrixParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	SPECTUBMatrix& matrix =
		objectFromHandle<SPECTUBMatrix>(hp);
        int value = dataFromHandle<int>(hv);
	if (boost::iequals(name, "keep_all_views_in_cache"))
		matrix.set_keep_all_views_in_cache(value);
        else if (boost::iequals(name, "attenuation_image"))
                {
                       STIRImageData& id = objectFromHandle<STIRImageData>(hv);
                       matrix.set_attenuation_image_sptr(id.data_sptr());
                }
        else
                return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_SPECTUBMatrixParameter(const DataHandle* handle, const char* name)
{
	SPECTUBMatrix& matrix =
		objectFromHandle<SPECTUBMatrix>(handle);
	if (boost::iequals(name, "keep_all_views_in_cache"))
		return dataHandle<int>(matrix.get_keep_all_views_in_cache());
        else if (boost::iequals(name, "attenuation_image"))
                {
                  shared_ptr<const DiscretisedDensity<3,float> > att_im_sptr(matrix.get_attenuation_image_sptr());
                  if (!att_im_sptr)
                       return handle_error("SPECTUBMatrix: attenuation image not set", __FILE__, __LINE__);
                  sptrImage3DF sptr_im(att_im_sptr->clone());
                  shared_ptr<STIRImageData> sptr_id(new STIRImageData(sptr_im));
                  return newObjectHandle(sptr_id);
                }
	return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setPinholeSPECTUBMatrixParameter(DataHandle* hp, const char* name, const DataHandle* hv)
{
#if STIR_VERSION < 050100
    DataHandle* handle = new DataHandle;
    ExecutionStatus status("STIR version older than 5.1 so PinholeSPECTUB not supported" , __FILE__, __LINE__);
    handle->set(0, &status);
    return (void*)handle;
#else

    PinholeSPECTUBMatrix& matrix =
        objectFromHandle< PinholeSPECTUBMatrix >(hp);
    if (sirf::iequals(name, "maximum_number_of_sigmas"))
        matrix.set_maximum_number_of_sigmas(dataFromHandle<float>(hv));
    else if (sirf::iequals(name, "spatial_resolution_PSF"))
        matrix.set_spatial_resolution_PSF(dataFromHandle<float>(hv));
    else if (sirf::iequals(name, "subsampling_factor_PSF"))
        matrix.set_subsampling_factor_PSF(dataFromHandle<int>(hv));
    else if (sirf::iequals(name, "detector_file"))
        matrix.set_detector_file(charDataFromDataHandle(hv));
    else if (sirf::iequals(name, "collimator_file"))
        matrix.set_collimator_file(charDataFromDataHandle(hv));
    else if (sirf::iequals(name, "psf_correction"))
        matrix.set_psf_correction(charDataFromDataHandle(hv));
    else if (sirf::iequals(name, "doi_correction"))
        matrix.set_doi_correction(charDataFromDataHandle(hv));
    else if (sirf::iequals(name, "attenuation_type"))
        matrix.set_attenuation_type(charDataFromDataHandle(hv));
    else if (sirf::iequals(name, "object_radius"))
        matrix.set_object_radius(dataFromHandle<float>(hv));
    else if (sirf::iequals(name, "attenuation_image"))
    {
        STIRImageData& id = objectFromHandle<STIRImageData>(hv);
        matrix.set_attenuation_image_sptr(id.data_sptr());
    }
    else if (sirf::iequals(name, "mask_image"))
    {
        STIRImageData& id = objectFromHandle<STIRImageData>(hv);
        matrix.set_mask_image_sptr(id.data_sptr());
    }
    else if (sirf::iequals(name, "mask_from_attenuation_map"))
        matrix.set_mask_from_attenuation_map(dataFromHandle<bool>(hv));
    else if (sirf::iequals(name, "keep_all_views_in_cache"))
        matrix.set_keep_all_views_in_cache(dataFromHandle<bool>(hv));
    else
        return parameterNotFound(name, __FILE__, __LINE__);
    return new DataHandle;
#endif
}

void*
sirf::cSTIR_PinholeSPECTUBMatrixParameter(const DataHandle* handle, const char* name)
{
#if STIR_VERSION < 050100
    DataHandle* h = new DataHandle;
    ExecutionStatus status("STIR version older than 5.1 so PinholeSPECTUB not supported" , __FILE__, __LINE__);
    h->set(0, &status);
    return (void*)h;
#else

    PinholeSPECTUBMatrix& matrix =
        objectFromHandle< PinholeSPECTUBMatrix >(handle);
    if (sirf::iequals(name, "maximum_number_of_sigmas"))
        return dataHandle<float>(matrix.get_maximum_number_of_sigmas());
    else if (sirf::iequals(name, "spatial_resolution_PSF"))
        return dataHandle<float>(matrix.get_spatial_resolution_PSF());
    else if (sirf::iequals(name, "subsampling_factor_PSF"))
        return dataHandle<int>(matrix.get_subsampling_factor_PSF());
    if (sirf::iequals(name, "psf_correction"))
        return charDataHandleFromCharData(matrix.get_psf_correction().c_str());
    if (sirf::iequals(name, "doi_correction"))
        return charDataHandleFromCharData(matrix.get_doi_correction().c_str());
    if (sirf::iequals(name, "attenuation_type"))
        return charDataHandleFromCharData(matrix.get_attenuation_type().c_str());
    if (sirf::iequals(name, "object_radius"))
        return dataHandle<float>(matrix.get_object_radius());
    else if (sirf::iequals(name, "attenuation_image"))
    {
        shared_ptr<const DiscretisedDensity<3,float> > att_im_sptr(matrix.get_attenuation_image_sptr());
        if (!att_im_sptr)
           return handle_error("PinholeSPECTUBMatrix: attenuation image not set", __FILE__, __LINE__);
        sptrImage3DF sptr_im(att_im_sptr->clone());
        shared_ptr<STIRImageData> sptr_id(new STIRImageData(sptr_im));
        return newObjectHandle(sptr_id);
    }
    else if (sirf::iequals(name, "mask_image"))
    {
        shared_ptr<const DiscretisedDensity<3,float> > msk_im_sptr(matrix.get_mask_image_sptr());
        if (!msk_im_sptr)
           return handle_error("PinholeSPECTUBMatrix: mask image not set", __FILE__, __LINE__);
        sptrImage3DF sptr_im(msk_im_sptr->clone());
        shared_ptr<STIRImageData> sptr_id(new STIRImageData(sptr_im));
        return newObjectHandle(sptr_id);
    }
    else if (sirf::iequals(name, "mask_from_attenuation_map"))
        return dataHandle<bool>(matrix.get_mask_from_attenuation_map());
    else if (sirf::iequals(name, "keep_all_views_in_cache"))
        return dataHandle<bool>(matrix.get_keep_all_views_in_cache());
    else
        return parameterNotFound(name, __FILE__, __LINE__);

#endif
}

void*
sirf::cSTIR_setAcquisitionModelParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	AcqMod3DF& am = objectFromHandle< AcqMod3DF >(hp);
	if (sirf::iequals(name, "additive_term")) {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, hv);
		am.set_additive_term(sptr_ad);
	}
	else if (sirf::iequals(name, "background_term")) {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, hv);
		am.set_background_term(sptr_ad);
	}
	else if (sirf::iequals(name, "asm")) {
		SPTR_FROM_HANDLE(PETAcquisitionSensitivityModel, sptr_asm, hv);
		am.set_asm(sptr_asm);
	}
	else if (sirf::iequals(name, "image_data_processor")) {
		STIRSPTR_FROM_HANDLE(ImageDataProcessor, sptr_proc, hv);
		am.set_image_data_processor(sptr_proc);
	}
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_AcquisitionModelParameter(DataHandle* hm, const char* name)
{
	AcqMod3DF& am = objectFromHandle< AcqMod3DF >(hm);
	if (sirf::iequals(name, "range geometry"))
		return newObjectHandle(am.acq_template_sptr());
	else if (sirf::iequals(name, "domain geometry"))
		return newObjectHandle(am.image_template_sptr());
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_setAcqModUsingMatrixParameter
(DataHandle* hm, const char* name, const DataHandle* hv)
{
	AcqModUsingMatrix3DF& am = objectFromHandle<AcqModUsingMatrix3DF>(hm);
	if (sirf::iequals(name, "matrix")) {
		STIRSPTR_FROM_HANDLE(ProjMatrixByBin, sptr_m, hv);
		am.set_matrix(sptr_m);
	}
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

#ifdef STIR_WITH_NiftyPET_PROJECTOR
void*
sirf::cSTIR_setAcqModUsingNiftyPETParameter
(DataHandle* hm, const char* name, const DataHandle* hv)
{
    AcqModUsingNiftyPET3DF& am = objectFromHandle<AcqModUsingNiftyPET3DF>(hm);
    if (sirf::iequals(name, "cuda_verbosity"))
        am.set_cuda_verbosity(dataFromHandle<int>((void*)hv));
    else if (sirf::iequals(name, "use_truncation"))
        am.set_use_truncation(dataFromHandle<int>((void*)hv));
    else
        return parameterNotFound(name, __FILE__, __LINE__);
    return new DataHandle;
}
#endif

void*
sirf::cSTIR_acqModUsingMatrixParameter
(DataHandle* hm, const char* name)
{
	AcqModUsingMatrix3DF& am = objectFromHandle<AcqModUsingMatrix3DF>(hm);
	if (sirf::iequals(name, "matrix"))
		return newObjectHandle(am.matrix_sptr());
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_setGeneralisedPriorParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	auto& prior = objectFromHandle< Prior3DF >(hp);
	if (sirf::iequals(name, "penalisation_factor"))
		prior.set_penalisation_factor(dataFromHandle<float>((void*)hv));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_generalisedPriorParameter(const DataHandle* handle, const char* name)
{
	auto& prior = objectFromHandle<Prior3DF>(handle);
	if (sirf::iequals(name, "penalisation_factor"))
		return dataHandle<float>(prior.get_penalisation_factor());
	return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setQuadraticPriorParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	auto& prior = objectFromHandle<xSTIR_QuadraticPrior3DF>(hp);
	if (sirf::iequals(name, "only_2D"))
		prior.only2D(dataFromHandle<int>((void*)hv));
	else if (sirf::iequals(name, "kappa")) {
		auto& id = objectFromHandle<STIRImageData>(hv);
		prior.set_kappa_sptr(id.data_sptr());
	}
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_QuadraticPriorParameter
(DataHandle* hp, const char* name)
{
	auto& prior = objectFromHandle<xSTIR_QuadraticPrior3DF>(hp);
	if (sirf::iequals(name, "kappa")) {
		auto sptr_im = std::make_shared<STIRImageData>(*prior.get_kappa_sptr());
		return newObjectHandle(sptr_im);
	}
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_setLogcoshPriorParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	auto& prior = objectFromHandle<xSTIR_LogcoshPrior3DF>(hp);
	if (sirf::iequals(name, "only_2D"))
		prior.only2D(dataFromHandle<int>((void*)hv));
	else if (sirf::iequals(name, "kappa")) {
		auto& id = objectFromHandle<STIRImageData>(hv);
		prior.set_kappa_sptr(id.data_sptr());
	}
	else if (sirf::iequals(name, "scalar"))
		prior.set_scalar(dataFromHandle<float>((void*)hv));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_LogcoshPriorParameter
(DataHandle* hp, const char* name)
{
	auto& prior = objectFromHandle<xSTIR_LogcoshPrior3DF >(hp);
	if (sirf::iequals(name, "kappa")) {
		auto sptr_im = std::make_shared<STIRImageData>(*prior.get_kappa_sptr());
		return newObjectHandle(sptr_im);
	}
	else if (sirf::iequals(name, "scalar"))
		return dataHandle<float>(prior.get_scalar());
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_setRelativeDifferencePriorParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
        auto& prior = objectFromHandle<stir::RelativeDifferencePrior<float>>(hp);
	if (sirf::iequals(name, "only_2D")) {
		auto& xrdp = objectFromHandle<xSTIR_RelativeDifferencePrior3DF>(hp);
		xrdp.only2D(dataFromHandle<int>((void*)hv));
        }
	else if (sirf::iequals(name, "kappa")) {
		auto& id = objectFromHandle<STIRImageData>(hv);
		prior.set_kappa_sptr(id.data_sptr());
	}
	else if (sirf::iequals(name, "gamma"))
		prior.set_gamma(dataFromHandle<float>((void*)hv));
	else if (sirf::iequals(name, "epsilon"))
		prior.set_epsilon(dataFromHandle<float>((void*)hv));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_RelativeDifferencePriorParameter
(DataHandle* hp, const char* name)
{
	auto& prior = objectFromHandle<stir::RelativeDifferencePrior<float>>(hp);
	if (sirf::iequals(name, "kappa")) {
		auto sptr_im = std::make_shared<STIRImageData>(*prior.get_kappa_sptr());
		return newObjectHandle(sptr_im);
	}
	else if (sirf::iequals(name, "gamma"))
		return dataHandle<float>(prior.get_gamma());
	else if (sirf::iequals(name, "epsilon"))
		return dataHandle<float>(prior.get_epsilon());
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_setPLSPriorParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	auto& prior = objectFromHandle<PLSPrior<float> >(hp);
	if (sirf::iequals(name, "only_2D"))
		prior.set_only_2D(dataFromHandle<int>((void*)hv));
	else if (sirf::iequals(name, "alpha"))
		prior.set_alpha(dataFromHandle<float>((void*)hv));
	else if (sirf::iequals(name, "eta"))
		prior.set_eta(dataFromHandle<float>((void*)hv));
	else if (sirf::iequals(name, "anatomical_image")) {
		auto& id = objectFromHandle<STIRImageData>(hv);
		prior.set_anatomical_image_sptr(id.data_sptr());
	}
	else if (sirf::iequals(name, "kappa")) {
		auto& id = objectFromHandle<STIRImageData>(hv);
		prior.set_kappa_sptr(id.data_sptr());
	}
	else if (sirf::iequals(name, "kappa_filename"))
		prior.set_kappa_filename(charDataFromDataHandle(hv));
	else if (sirf::iequals(name, "anatomical_filename"))
		prior.set_anatomical_filename(charDataFromDataHandle(hv));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_PLSPriorParameter
(DataHandle* hp, const char* name)
{
	auto& prior = objectFromHandle<PLSPrior<float> >(hp);
	if (sirf::iequals(name, "only_2D"))
		return dataHandle<int>(prior.get_only_2D());
	else if (sirf::iequals(name, "alpha"))
		return dataHandle<float>(prior.get_alpha());
	else if (sirf::iequals(name, "eta"))
		return dataHandle<float>(prior.get_eta());
	else if (sirf::iequals(name, "anatomical_image")) {
		auto sptr_im = prior.get_anatomical_image_sptr();
		auto sptr_id = std::make_shared<STIRImageData>(*sptr_im);
		return newObjectHandle(sptr_id);
	}
	else if (sirf::iequals(name, "kappa")) {
		auto sptr_im = std::make_shared<STIRImageData>(*prior.get_kappa_sptr());
		return newObjectHandle(sptr_im);
	}
	else if (sirf::iequals(name, "norm")) {
		auto sptr_im = prior.get_norm_sptr();
		auto sptr_id = std::make_shared<STIRImageData>(*sptr_im);
		return newObjectHandle(sptr_id);
	}
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_setScatterSimulatorParameter
(const DataHandle *hp, const char* name, const DataHandle* hv)
{

    auto& obj = objectFromHandle< PETSingleScatterSimulator >(hp);

    if (sirf::iequals(name, "setActivityImage"))
    {
        SPTR_FROM_HANDLE(STIRImageData, sptr_id, hv);
        obj.set_activity_image_sptr(sptr_id);
    }
    else if (sirf::iequals(name, "setAttenuationImage"))
    {
        SPTR_FROM_HANDLE(STIRImageData, sptr_id, hv);
        obj.set_attenuation_image_sptr(sptr_id);
    }
    else
        return parameterNotFound(name, __FILE__, __LINE__);

    return new DataHandle;
}

void*
sirf::cSTIR_setScatterEstimatorParameter
(const DataHandle *hp, const char* name, const DataHandle* hv)
{

    PETScatterEstimator& obj =
            objectFromHandle< PETScatterEstimator >(hp);

    if (sirf::iequals(name, "setInput"))
    {
        SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_pd, hv);
        obj.set_input_sptr(sptr_pd);
    }
    else if (sirf::iequals(name, "setRandoms"))
    {
        SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_pd, hv);
        obj.set_background_sptr(sptr_pd);
    }
    else if (sirf::iequals(name, "setAttenuationImage"))
    {
        SPTR_FROM_HANDLE(STIRImageData, sptr_id, hv);
        obj.set_attenuation_image_sptr(sptr_id);
    }
    else if (sirf::iequals(name, "setMaskImage"))
    {
        SPTR_FROM_HANDLE(STIRImageData, sptr_id, hv);
        obj.set_mask_image_sptr(sptr_id);
    }
    else if (sirf::iequals(name, "setMaskAcqData"))
    {
        SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_pd, hv);
        obj.set_mask_acq_data_sptr(sptr_pd);
    }
#if STIR_VERSION >= 060200
    else if (sirf::iequals(name, "setRecomputeMaskImage"))
    {
        bool value = dataFromHandle<bool>(hv);
        obj.set_recompute_mask_image(value);
    }
#endif
    else if (sirf::iequals(name, "setAttenuationCorrectionFactors"))
    {
        SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, hv);
        obj.set_attenuation_correction_factors_sptr(sptr_ad);
    }
    else if (sirf::iequals(name, "setASM"))
    {
      SPTR_FROM_HANDLE(PETAcquisitionSensitivityModel, sptr_asm, hv);
        obj.set_asm(sptr_asm);
    }
    else if (sirf::iequals(name, "set_num_iterations"))
    {
        int value = dataFromHandle<int>(hv);
        obj.set_num_iterations(value);
    }
    else if (sirf::iequals(name, "set_OSEM_num_subiterations"))
    {
        int value = dataFromHandle<int>(hv);
        obj.set_OSEM_num_subiterations(value);
    }
    else if (sirf::iequals(name, "set_OSEM_num_subsets"))
    {
        int value = dataFromHandle<int>(hv);
        obj.set_OSEM_num_subsets(value);
    }
    else if (sirf::iequals(name, "set_output_prefix"))
    {
        obj.set_output_prefix(charDataFromHandle(hv));
    }
    else if (sirf::iequals(name, "set_max_scale_value"))
    {
        obj.set_max_scale_value(dataFromHandle<float>(hv));
    }
    else if (sirf::iequals(name, "set_min_scale_value"))
    {
        obj.set_min_scale_value(dataFromHandle<float>(hv));
    }
    else
        return parameterNotFound(name, __FILE__, __LINE__);

    return new DataHandle;
}

void*
sirf::cSTIR_ScatterEstimatorParameter(DataHandle* hp, const char* name)
{
	auto& processor = objectFromHandle<PETScatterEstimator>(hp);
	if (sirf::iequals(name, "output"))
		return newObjectHandle(processor.get_output());
	if (sirf::iequals(name, "num_iterations"))
          return dataHandle<int>(processor.get_num_iterations());
	if (sirf::iequals(name, "OSEM_num_subiterations"))
          return dataHandle<int>(processor.get_OSEM_num_subiterations());
	if (sirf::iequals(name, "OSEM_num_subsets"))
          return dataHandle<int>(processor.get_OSEM_num_subsets());
	return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setPoissonNoiseGeneratorParameter
(const DataHandle *hp, const char* name, const DataHandle* hv)
{
    auto& obj = objectFromHandle<PoissonNoiseGenerator>(hp);
    if (sirf::iequals(name, "seed"))
        obj.seed(dataFromHandle<int>(hv));
    return new DataHandle;
}

void*
sirf::cSTIR_setGeneralisedObjectiveFunctionParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	ObjectiveFunction3DF& obj_fun =
		objectFromHandle< ObjectiveFunction3DF >(hp);
	if (sirf::iequals(name, "prior")) {
		STIRSPTR_FROM_HANDLE(GeneralisedPrior<Image3DF>, sptr_p, hv);
		obj_fun.set_prior_sptr(sptr_p);
	}
	else if (sirf::iequals(name, "num_subsets"))
		obj_fun.set_num_subsets(dataFromHandle<int>((void*)hv));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_generalisedObjectiveFunctionParameter
(const DataHandle* handle, const char* name)
{
	ObjectiveFunction3DF& obj_fun =
		objectFromHandle< ObjectiveFunction3DF >(handle);
	if (sirf::iequals(name, "prior"))
		return newObjectHandle(obj_fun.get_prior_sptr());
	else if (sirf::iequals(name, "num_subsets"))
		return dataHandle<int>(obj_fun.get_num_subsets());
	return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setPoissonLogLikelihoodWithLinearModelForMeanParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	PoissonLogLhLinModMean3DF& obj_fun =
		objectFromHandle<PoissonLogLhLinModMean3DF>(hp);
	if (sirf::iequals(name, "sensitivity_filename"))
		obj_fun.set_sensitivity_filename(charDataFromDataHandle(hv));
	else if (sirf::iequals(name, "use_subset_sensitivities"))
		obj_fun.set_use_subset_sensitivities
			(sirf::iequals(charDataFromDataHandle(hv), "true"));
	else if (sirf::iequals(name, "recompute_sensitivity"))
		obj_fun.set_recompute_sensitivity
			(sirf::iequals(charDataFromDataHandle(hv), "true"));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_setPoissonLogLikelihoodWithLinearModelForMeanAndProjDataParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	xSTIR_PoissonLogLikelihoodWithLinearModelForMeanAndProjData3DF&
		obj_fun = objectFromHandle
		<xSTIR_PoissonLogLikelihoodWithLinearModelForMeanAndProjData3DF>
		(hp);
	if (sirf::iequals(name, "input_filename"))
		obj_fun.set_input_file(charDataFromDataHandle(hv));
	else if (sirf::iequals(name, "zero_seg0_end_planes"))
		obj_fun.set_zero_seg0_end_planes
			(sirf::iequals(charDataFromDataHandle(hv), "true"));
	//else if (sirf::iequals(name, "max_segment_num_to_process"))
	//	obj_fun.set_max_segment_num_toa_process(dataFromHandle<int>((void*)hv));
	else if (sirf::iequals(name, "acquisition_data")) {
		SPTR_FROM_HANDLE(STIRAcquisitionData, sptr_ad, hv);
		obj_fun.set_acquisition_data(sptr_ad);
	}
	else if (sirf::iequals(name, "acquisition_model")) {
		SPTR_FROM_HANDLE(AcqMod3DF, sptr_am, hv);
		obj_fun.set_acquisition_model(sptr_am);
	}
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_setPoissonLogLikelihoodWithLinearModelForMeanAndListModeDataWithProjMatrixByBinParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
    xSTIR_PoissonLLhLinModMeanListDataProjMatBin3DF&
        obj_fun = objectFromHandle
        <xSTIR_PoissonLLhLinModMeanListDataProjMatBin3DF>
        (hp);
    if (sirf::iequals(name, "cache_path")) {
        std::string s(charDataFromDataHandle(hv));
        obj_fun.set_cache_path(s);
    }
    else if (sirf::iequals(name, "set_recompute_cache")) {
        obj_fun.set_skip_lm_input_file(dataFromHandle<int>(hv));
    }
    else if (sirf::iequals(name, "acquisition_model")) {
        SPTR_FROM_HANDLE(AcqMod3DF, sptr_am, hv);
        obj_fun.set_acquisition_model(sptr_am);
    }
    else if (sirf::iequals(name, "acquisition_data")) {
        SPTR_FROM_HANDLE(ContainerBase, sptr_cont, hv);
        if (auto sptr_ld = std::dynamic_pointer_cast<STIRListmodeData>(sptr_cont)) {
            obj_fun.set_input_data(sptr_ld->data_sptr());
        }
        else
            THROW("set_acquisition_data argument needs to be ListmodeData");
    }
#if 0
    else if (sirf::iequals(name, "skip_lm_input_file")) {
        obj_fun.set_skip_lm_input_file(dataFromHandle<int>(hv));
    }
#endif
    else if (sirf::iequals(name, "skip_balanced_subsets")) {
        obj_fun.set_skip_balanced_subsets(dataFromHandle<int>(hv));
    }
    else if (sirf::iequals(name, "max_segment_num_to_process")) {
        obj_fun.set_max_segment_num_to_process(dataFromHandle<int>(hv));
    }
    else if (sirf::iequals(name, "cache_max_size")) {
        obj_fun.set_cache_max_size(dataFromHandle<int>(hv));
    }
    else if (sirf::iequals(name, "subsensitivity_filenames"))
    {
        std::string s(charDataFromDataHandle(hv));
        obj_fun.set_subsensitivity_filenames(s.c_str());
    }
    else
        return parameterNotFound(name, __FILE__, __LINE__);
    return new DataHandle;
}

void*
sirf::cSTIR_PoissonLogLikelihoodWithLinearModelForMeanAndProjDataParameter
(const DataHandle* handle, const char* name)
{
	xSTIR_PoissonLogLikelihoodWithLinearModelForMeanAndProjData3DF&
		obj_fun = objectFromHandle
		<xSTIR_PoissonLogLikelihoodWithLinearModelForMeanAndProjData3DF>
		(handle);
	//if (sirf::iequals(name, "projector_pair_type"))
	//	return newObjectHandle(obj_fun.get_projector_pair_sptr());
	if (sirf::iequals(name, "acquisition_model"))
		return newObjectHandle(obj_fun.acquisition_model_sptr());
	return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_PoissonLogLikelihoodWithLinearModelForMeanAndListModeDataWithProjMatrixByBinParameter
(DataHandle* handle, const char* name)
{
    xSTIR_PoissonLLhLinModMeanListDataProjMatBin3DF&
        obj_fun = objectFromHandle
        <xSTIR_PoissonLLhLinModMeanListDataProjMatBin3DF>
        (handle);
    if (sirf::iequals(name, "cache_path")) {
        return charDataHandle(obj_fun.get_cache_path().c_str());
    }
    if (sirf::iequals(name, "cache_max_size"))
        return dataHandle<int>(obj_fun.get_cache_max_size());
    if (sirf::iequals(name, "subsensitivity_filenames"))
        return charDataHandle(obj_fun.get_subsensitivity_filenames().c_str());
    return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setReconstructionParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	Reconstruction<Image3DF>& recon =
		objectFromHandle< Reconstruction<Image3DF> >(hp);
	if (sirf::iequals(name, "output_filename_prefix"))
		recon.set_output_filename_prefix(charDataFromDataHandle(hv));
	else if (sirf::iequals(name, "input_data")) {
		SPTR_FROM_HANDLE(ContainerBase, sptr_cont, hv);
		if (auto sptr_ad = std::dynamic_pointer_cast<STIRAcquisitionData>(sptr_cont)) {
			recon.set_input_data(sptr_ad->data_sptr());
		}
		else if (auto sptr_ld = std::dynamic_pointer_cast<STIRListmodeData>(sptr_cont)) {
			recon.set_input_data(sptr_ld->data_sptr());
		}
		else
		THROW("input_data needs to be either ListmodeData or AcquisitionData");
	}
	else if (sirf::iequals(name, "disable_output")) {
		recon.set_disable_output(true);
	}
	else if (sirf::iequals(name, "enable_output")) {
		recon.set_disable_output(false);
	}
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_setIterativeReconstructionParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	IterativeReconstruction3DF& recon =
		objectFromHandle<IterativeReconstruction3DF>(hp);
	if (sirf::iequals(name, "inter_iteration_filter_type")) {
		STIRSPTR_FROM_HANDLE(DataProcessor3DF, sptr_f, hv);
		recon.set_inter_iteration_filter_ptr(sptr_f);
	}
	else if (sirf::iequals(name, "objective_function")) {
		STIRSPTR_FROM_HANDLE(ObjectiveFunction3DF, sptr_obf, hv);
		recon.set_objective_function_sptr(sptr_obf);
	}
	else if (sirf::iequals(name, "initial_estimate")) {
		//xSTIR_IterativeReconstruction3DF& xrecon =
		//	(xSTIR_IterativeReconstruction3DF&)(recon);
		SIRF_DYNAMIC_CAST(xSTIR_IterativeReconstruction3DF, xrecon, recon);
		xrecon.set_initial_estimate_file(charDataFromDataHandle(hv));
	}
	else {
		int value = dataFromHandle<int>((void*)hv);
		if (sirf::iequals(name, "num_subsets"))
			recon.set_num_subsets(value);
		else if (sirf::iequals(name, "start_subset_num"))
			recon.set_start_subset_num(value);
		else if (sirf::iequals(name, "num_subiterations"))
			recon.set_num_subiterations(value);
		else if (sirf::iequals(name, "start_subiteration_num"))
			recon.set_start_subiteration_num(value);
		else if (sirf::iequals(name, "subiteration_num")) {
			//xSTIR_IterativeReconstruction3DF& xrecon =
			//	(xSTIR_IterativeReconstruction3DF&)(recon);
			SIRF_DYNAMIC_CAST(xSTIR_IterativeReconstruction3DF, xrecon, recon);
			xrecon.subiteration() = value;
		}
		else if (sirf::iequals(name, "save_interval"))
			recon.set_save_interval(value);
		else if (sirf::iequals(name, "inter_iteration_filter_interval"))
			recon.set_inter_iteration_filter_interval(value);
		else
			return parameterNotFound(name, __FILE__, __LINE__);
	}
	return new DataHandle;
}

void*
sirf::cSTIR_iterativeReconstructionParameter
(const DataHandle* handle, const char* name)
{
	IterativeReconstruction3DF& recon =
		objectFromHandle<IterativeReconstruction3DF>(handle);
	if (sirf::iequals(name, "num_subsets"))
		return dataHandle<int>(recon.get_num_subsets());
	if (sirf::iequals(name, "start_subset_num"))
		return dataHandle<int>(recon.get_start_subset_num());
	if (sirf::iequals(name, "num_subiterations"))
		return dataHandle<int>(recon.get_num_subiterations());
	if (sirf::iequals(name, "start_subiteration_num"))
		return dataHandle<int>(recon.get_start_subiteration_num());
	if (sirf::iequals(name, "subiteration_num")) {
		//xSTIR_IterativeReconstruction3DF& xrecon =
		//	(xSTIR_IterativeReconstruction3DF&)(recon);
		//SIRF_DYNAMIC_CAST(xSTIR_IterativeReconstruction3DF, xrecon, recon);
		int iter = recon.get_subiteration_num();
		return dataHandle<int>(iter);
		//return dataHandle<int>(xrecon.subiteration());
	}
	if (sirf::iequals(name, "objective_function"))
		return newObjectHandle(recon.get_objective_function_sptr());
	if (sirf::iequals(name, "inter_iteration_filter_type"))
		return newObjectHandle(recon.get_inter_iteration_filter_sptr());
	return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setOSMAPOSLParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	OSMAPOSLReconstruction<Image3DF>& recon =
		objectFromHandle<OSMAPOSLReconstruction<Image3DF> >(hp);
    if (sirf::iequals(name, "set_maximum_relative_change"))
            recon.set_maximum_relative_change(dataFromHandle<double>((void*)hv));
    else if (sirf::iequals(name, "set_minimum_relative_change"))
            recon.set_minimum_relative_change(dataFromHandle<double>((void*)hv));
	else if (sirf::iequals(name, "MAP_model"))
		recon.set_MAP_model(charDataFromDataHandle(hv));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_OSMAPOSLParameter(const DataHandle* handle, const char* name)
{
	OSMAPOSLReconstruction<Image3DF>& recon =
		objectFromHandle<OSMAPOSLReconstruction<Image3DF> >(handle);
	if (sirf::iequals(name, "objective_function"))
		return newObjectHandle(recon.get_objective_function_sptr());
	return parameterNotFound(name, __FILE__, __LINE__);
}

#ifdef USE_HKEM
void*
sirf::cSTIR_setKOSMAPOSLParameter
(DataHandle* hp, const char* name, const DataHandle* hv)
{
	KOSMAPOSLReconstruction<Image3DF>& recon =
		objectFromHandle<KOSMAPOSLReconstruction<Image3DF> >(hp);
	if (sirf::iequals(name, "anatomical_prior")) {
		SPTR_FROM_HANDLE(STIRImageData, sptr_ap, hv);
		recon.set_anatomical_prior_sptr(sptr_ap->data_sptr());
	}
	else if (sirf::iequals(name, "num_neighbours")) {
		int value = dataFromHandle<int>((void*)hv);
		recon.set_num_neighbours(value);
	}
	else if (sirf::iequals(name, "num_non_zero_features"))
		recon.set_num_non_zero_feat(dataFromHandle<int>((void*)hv));
	else if (sirf::iequals(name, "sigma_m"))
		recon.set_sigma_m(dataFromHandle<float>((void*)hv));
	else if (sirf::iequals(name, "sigma_p"))
		recon.set_sigma_p(dataFromHandle<float>((void*)hv));
	else if (sirf::iequals(name, "sigma_dp"))
		recon.set_sigma_dp(dataFromHandle<float>((void*)hv));
	else if (sirf::iequals(name, "sigma_dm"))
		recon.set_sigma_dm(dataFromHandle<float>((void*)hv));
	else if (sirf::iequals(name, "only_2D"))
		recon.set_only_2D(dataFromHandle<int>((void*)hv));
	else if (sirf::iequals(name, "hybrid"))
		recon.set_hybrid(dataFromHandle<int>((void*)hv));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_KOSMAPOSLParameter(const DataHandle* handle, const char* name)
{
	OSMAPOSLReconstruction<Image3DF>& recon =
		objectFromHandle<OSMAPOSLReconstruction<Image3DF> >(handle);
	if (sirf::iequals(name, "objective_function"))
		return newObjectHandle(recon.get_objective_function_sptr());
	return parameterNotFound(name, __FILE__, __LINE__);
}
#endif

void*
sirf::cSTIR_setOSSPSParameter(DataHandle* hp, const char* name, const DataHandle* hv)
{
	xSTIR_OSSPSReconstruction3DF& recon =
		objectFromHandle<xSTIR_OSSPSReconstruction3DF >(hp);
	if (sirf::iequals(name, "relaxation_parameter"))
		recon.relaxation_parameter_value() = dataFromHandle<float>(hv);
	else if (sirf::iequals(name, "relaxation_gamma"))
		recon.relaxation_gamma_value() = dataFromHandle<float>(hv);
	else if (sirf::iequals(name, "upper_bound"))
		recon.upper_bound_value() = dataFromHandle<double>(hv);
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_OSSPSParameter(const DataHandle* handle, const char* name)
{
	xSTIR_OSSPSReconstruction3DF& recon =
		objectFromHandle<xSTIR_OSSPSReconstruction3DF>(handle);
	if (sirf::iequals(name, "relaxation_parameter"))
		return dataHandle<float>(recon.relaxation_parameter_value());
	else if (sirf::iequals(name, "relaxation_gamma"))
		return dataHandle<float>(recon.relaxation_gamma_value());
	else if (sirf::iequals(name, "upper_bound"))
		return dataHandle<double>(recon.upper_bound_value());
	return parameterNotFound(name, __FILE__, __LINE__);
}

void*
sirf::cSTIR_setFBP2DParameter(DataHandle* hp, const char* name, const DataHandle* hv)
{
	xSTIR_FBP2DReconstruction& recon =
		objectFromHandle<xSTIR_FBP2DReconstruction >(hp);
	if (sirf::iequals(name, "input")) {
		STIRAcquisitionData& acq_data = objectFromHandle<STIRAcquisitionData>(hv);
		recon.set_input(acq_data);
	}
	else if (sirf::iequals(name, "zoom")) {
		float zoom = dataFromHandle<float>(hv);
		recon.set_zoom(zoom);
		recon.cancel_setup();
	}
	else if (sirf::iequals(name, "xy")) {
		int xy = dataFromHandle<int>(hv);
		recon.set_output_image_size_xy(xy);
		recon.cancel_setup();
	}
	else if (sirf::iequals(name, "alpha")) {
		float alpha = dataFromHandle<float>(hv);
		recon.set_alpha_ramp(alpha);
	}
	else if (sirf::iequals(name, "fc")) {
		double fc = dataFromHandle<float>(hv);
		recon.set_frequency_cut_off(fc);
	}
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}

void*
sirf::cSTIR_FBP2DParameter(DataHandle* hp, const char* name)
{
	xSTIR_FBP2DReconstruction& recon =
		objectFromHandle<xSTIR_FBP2DReconstruction >(hp);
	if (sirf::iequals(name, "output"))
		return newObjectHandle(recon.get_output());
	return parameterNotFound(name, __FILE__, __LINE__);
}
