/*
CCP PETMR Synergistic Image Reconstruction Framework (SIRF)
Copyright 2015 - 2017 Rutherford Appleton Laboratory STFC

This is software developed for the Collaborative Computational
Project in Positron Emission Tomography and Magnetic Resonance imaging
(http://www.ccppetmr.ac.uk/).

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

#include "stir_x.h"
#include "data_handle.h"
#include "csirfreg_p.h"
#include "NiftiImageData3D.h"
#include "SIRFReg.h"
#include "SIRFRegNiftyF3dSym.h"
#include "SIRFRegNiftyResample.h"
#include "SIRFRegImageWeightedMean.h"

using namespace stir;
using namespace sirf;

extern "C"
char* charDataFromHandle(const void* ptr);
extern "C"
int intDataFromHandle(const void* ptr);

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

// ------------------------------------------------------------------------------------ //
//   NiftiImageData
// ------------------------------------------------------------------------------------ //
void*
sirf::cSIRFReg_NiftiImageDataParameter(const DataHandle* handle, const char* name)
{
    NiftiImageData<float>& s = objectFromHandle<NiftiImageData<float> >(handle);
    if (boost::iequals(name, "max"))
        return dataHandle<float>(s.get_max());
    if (boost::iequals(name, "min"))
        return dataHandle<float>(s.get_min());
    if (boost::iequals(name, "sum"))
        return dataHandle<float>(s.get_sum());
    else
        return parameterNotFound(name, __FILE__, __LINE__);
}
// ------------------------------------------------------------------------------------ //
//   SIRFReg
// ------------------------------------------------------------------------------------ //
// set
void*
sirf::cSIRFReg_setSIRFRegParameter(void* hp, const char* name, const void* hv)
{
    SIRFReg<float>& s = objectFromHandle<SIRFReg<float> >(hp);
	if (boost::iequals(name, "parameter_file"))
        s.set_parameter_file(charDataFromHandle(hv));
    else if (boost::iequals(name, "reference_image"))
        s.set_reference_image(std::make_shared<const NiftiImageData3D<float> >(objectFromHandle<const NiftiImageData3D<float> >(hv)));
    else if (boost::iequals(name, "floating_image"))
        s.set_floating_image(std::make_shared<const NiftiImageData3D<float> >(objectFromHandle<const NiftiImageData3D<float> >(hv)));
    else if (boost::iequals(name, "reference_mask"))
        s.set_reference_mask(std::make_shared<const NiftiImageData3D<float> >(objectFromHandle<const NiftiImageData3D<float> >(hv)));
    else if (boost::iequals(name, "floating_mask"))
        s.set_floating_mask(std::make_shared<const NiftiImageData3D<float> >(objectFromHandle<const NiftiImageData3D<float> >(hv)));
	else
		return parameterNotFound(name, __FILE__, __LINE__);
	return new DataHandle;
}
// get
void*
sirf::cSIRFReg_SIRFRegParameter(const DataHandle* handle, const char* name)
{
    SIRFReg<float>& s = objectFromHandle<SIRFReg<float> >(handle);
	if (boost::iequals(name, "output")) {
        shared_ptr<NiftiImageData3D<float> > sptr_id(new NiftiImageData3D<float>(s.get_output()->deep_copy()));
        return newObjectHandle(sptr_id);
	}
	else
		return parameterNotFound(name, __FILE__, __LINE__);
}
// ------------------------------------------------------------------------------------ //
//   SIRFRegNiftyF3dSym
// ------------------------------------------------------------------------------------ //
// set
void*
sirf::cSIRFReg_setSIRFRegNiftyF3dSymParameter(void* hp, const char* name, const void* hv)
{
    SIRFRegNiftyF3dSym<float>& s = objectFromHandle<SIRFRegNiftyF3dSym<float> >(hp);
    if (boost::iequals(name, "floating_time_point"))
        s.set_floating_time_point(intDataFromHandle(hv));
    else if (boost::iequals(name, "reference_time_point"))
        s.set_reference_time_point(intDataFromHandle(hv));
    else if (boost::iequals(name, "initial_affine_transformation")) {
        const SIRFRegAffineTransformation<float>& mat = objectFromHandle<const SIRFRegAffineTransformation<float> >(hv);
        s.set_initial_affine_transformation(mat);
    }
    else
        return parameterNotFound(name, __FILE__, __LINE__);
    return new DataHandle;
}
// ------------------------------------------------------------------------------------ //
//   SIRFRegNiftyResample
// ------------------------------------------------------------------------------------ //
// set
void*
sirf::cSIRFReg_setSIRFRegNiftyResampleParameter(void* hp, const char* name, const void* hv)
{
    SIRFRegNiftyResample<float>& s = objectFromHandle<SIRFRegNiftyResample<float> >(hp);
    if (boost::iequals(name, "reference_image"))
        s.set_reference_image(std::make_shared<const NiftiImageData3D<float> >(objectFromHandle<const NiftiImageData3D<float> >(hv)));
    else if (boost::iequals(name, "floating_image"))
        s.set_floating_image(std::make_shared<const NiftiImageData3D<float> >(objectFromHandle<const NiftiImageData3D<float> >(hv)));
    else if (boost::iequals(name, "interpolation_type"))
        s.set_interpolation_type(static_cast<SIRFRegNiftyResample<float>::InterpolationType>(intDataFromHandle(hv)));
    else
        return parameterNotFound(name, __FILE__, __LINE__);
    return new DataHandle;
}
// get
void*
sirf::cSIRFReg_SIRFRegNiftyResampleParameter(const DataHandle* handle, const char* name)
{
    SIRFRegNiftyResample<float>& s = objectFromHandle<SIRFRegNiftyResample<float> >(handle);
    if (boost::iequals(name, "output")) {
        shared_ptr<NiftiImageData3D<float> > sptr_id(new NiftiImageData3D<float>(s.get_output()->deep_copy()));
        return newObjectHandle(sptr_id);
    }
    else
        return parameterNotFound(name, __FILE__, __LINE__);
}

// ------------------------------------------------------------------------------------ //
//   SIRFRegImageWeightedMean
// ------------------------------------------------------------------------------------ //
// get
void*
sirf::cSIRFReg_SIRFRegImageWeightedMeanParameter(const DataHandle* handle, const char* name)
{
    SIRFRegImageWeightedMean<float>& s = objectFromHandle<SIRFRegImageWeightedMean<float> >(handle);
    if (boost::iequals(name, "output")) {
        shared_ptr<NiftiImageData<float> > sptr_id(new NiftiImageData<float>(s.get_output()->deep_copy()));
        return newObjectHandle(sptr_id);
    }
    else
        return parameterNotFound(name, __FILE__, __LINE__);
}

// ------------------------------------------------------------------------------------ //
//   SIRFRegAffineTransformation
// ------------------------------------------------------------------------------------ //
// get
void*
sirf::cSIRFReg_SIRFRegAffineTransformationParameter(const DataHandle* handle, const char* name)
{
    SIRFRegAffineTransformation<float>& s = objectFromHandle<SIRFRegAffineTransformation<float> >(handle);
    if (boost::iequals(name, "determinant")) {
        return dataHandle<float>(s.get_determinant());
    }
    if (boost::iequals(name, "identity")) {
        shared_ptr<SIRFRegAffineTransformation<float> > sptr_id(new SIRFRegAffineTransformation<float>(SIRFRegAffineTransformation<float>::get_identity()));
        return newObjectHandle(sptr_id);
    }
    else
        return parameterNotFound(name, __FILE__, __LINE__);
}
