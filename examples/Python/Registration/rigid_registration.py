'''Demo for affine/rigid registration.

Usage:
  rigid_registration [--help | options]

Options:
  -e <engn>, --engine=<engn>   reconstruction engine [default: SIRFReg]
  --ref                        reference image
  --flo                        floating image
  --par                        parameter file
  #--rmask                     mask of reference image
  #--fmask                     mask of floating image
  --warped <file>              warped image filename [default: output.nii]
  --TM_forward                 forward transformation matrix
  --TM_inverse                 inverse transformation matrix
  --disp_fwd_4D                4D forward displacement field image
  --def_fwd_4D                 4D forward deformation field image
  --disp_inv_4D                4D inverse displacement field image
  --def_inv_4D                 4D inverse deformation field image
'''

## CCP PETMR Synergistic Image Reconstruction Framework (SIRF)
## Copyright 2018 Rutherford Appleton Laboratory STFC
## Copyright 2018 University College London.
##
## This is software developed for the Collaborative Computational
## Project in Positron Emission Tomography and Magnetic Resonance imaging
## (http://www.ccppetmr.ac.uk/).
##
## Licensed under the Apache License, Version 2.0 (the "License");
##   you may not use this file except in compliance with the License.
##   You may obtain a copy of the License at
##       http://www.apache.org/licenses/LICENSE-2.0
##   Unless required by applicable law or agreed to in writing, software
##   distributed under the License is distributed on an "AS IS" BASIS,
##   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##   See the License for the specific language governing permissions and
##   limitations under the License.

import os

__version__ = '0.1.0'
from docopt import docopt
args = docopt(__doc__, version=__version__)

# import engine module
exec('import p' + args['--engine'])

# process command-line options
SIRF_PATH = os.environ.get('SIRF_PATH')
if SIRF_PATH is not None:
        examples_path = SIRF_PATH + '/data/examples/Registration'
else:
    errorMsg = 'You need to set the SIRF_PATH environment variable to allow finding the raw data.'
    raise error(errorMsg)

# reference
ref_file = args['--ref']
if ref_file is False:
    ref_file = examples_path + "/test.nii.gz"

# floating
flo_file = args['--flo']
if flo_file is False:
    flo_file = examples_path + "/test2.nii.gz"

# parameter file
par_file = args['--par']
if par_file is False:
    par_file = examples_path + "/paramFiles/niftyreg_aladin.par"

def main():

    # Open reference and floating images
    ref = pSIRFReg.NiftiImageData3D(ref_file)
    flo = pSIRFReg.NiftiImageData3D(flo_file)

    # Registration
    na = pSIRFReg.NiftyAladinSym()
    na.set_reference_image(ref)
    na.set_floating_image(flo)
    na.set_parameter_file(par_file)
    # if args['--rmask'] is not False:
    #     na.set_reference_mask(args['--rmask'])
    # if args['--fmask'] is not False:
    #     na.set_floating_mask(args['--rmask'])
    na.process()

    # Output
    na.get_output().save_to_file(args['--warped'])

    # TMs
    if args['--TM_forward'] is not False:
        na.get_transformation_matrix_forward().save_to_file(args['--TM_forward'])
    if args['--TM_inverse'] is not False:
        na.get_transformation_matrix_inverse().save_to_file(args['--TM_inverse'])

    # Disp fields
    if args['--disp_fwd_4D'] is not False:
        na.get_displacement_field_forward().save_to_file(args['--disp_fwd_4D'])
    if args['--disp_inv_4D'] is not False:
        na.get_displacement_field_inverse().save_to_file(args['--disp_inv_4D'])

    # Def fields
    if args['--def_fwd_4D'] is not False:
        na.get_deformation_field_forward().save_to_file(args['--def_fwd_4D'])
    if args['--def_inv_4D'] is not False:
        na.get_deformation_field_inverse().save_to_file(args['--def_inv_4D'])

try:
    main()
    print('done')
except error as err:
    print('%s' % err.value)
