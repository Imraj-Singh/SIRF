'''Acquisition data handling demo.

Usage:
  acquisition_data [--help | options]

Options:
  -f <file>, --file=<file>     raw data file [default: my_forward_projection.hs]
  -p <path>, --path=<path>     path to data files, defaults to data/examples/PET
                               subfolder of SIRF root folder
  -e <engn>, --engine=<engn>   reconstruction engine [default: STIR]
  -s <stsc>, --storage=<stsc>  acquisition data storage scheme [default: memory]
  --non-interactive            do not show plots
'''

## SyneRBI Synergistic Image Reconstruction Framework (SIRF)
## Copyright 2015 - 2020 Rutherford Appleton Laboratory STFC
## Copyright 2015 - 2017 University College London.
##
## This is software developed for the Collaborative Computational
## Project in Synergistic Reconstruction for Biomedical Imaging (formerly CCP PETMR)
## (http://www.ccpsynerbi.ac.uk/).
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

__version__ = '0.1.0'
from docopt import docopt
args = docopt(__doc__, version=__version__)

import math
import numpy

from sirf.Utilities import error, examples_data_path, existing_filepath

# import engine module
import importlib
engine = args['--engine']
pet = importlib.import_module('sirf.' + engine)

# process command-line options
data_file = args['--file']
data_path = args['--path']
if data_path is None:
    data_path = examples_data_path('PET')
storage = args['--storage']
show_plot = not args['--non-interactive']

# select acquisition data storage scheme
# storage = 'file' (default):
#     all acquisition data generated by the script is kept in
#     scratch files deleted after the script terminates
# storage = 'memory':
#     all acquisition data generated by the script is kept in RAM
#     (avoid if data is very large)
scheme = pet.AcquisitionData.get_storage_scheme()
if scheme != storage:
    print('default storage scheme is %s' % repr(scheme))
    print('setting storage scheme to %s' % repr(storage))
    pet.AcquisitionData.set_storage_scheme(storage)
else:
    print('using default storage scheme %s' % repr(scheme))


def main():
    engine_version = pet.get_engine_version_string()
    print('Using %s version %s as the reconstruction engine' % (engine, engine_version))
    print('%s doc path: %s' % (engine, pet.get_engine_doc_dir()))
    print('%s examples path: %s' % (engine, pet.get_engine_examples_dir()))

    # direct all engine's messages to files
    _ = pet.MessageRedirector('info.txt', 'warn.txt', 'errr.txt')

    # PET acquisition data to be read from this file
    raw_data_file = existing_filepath(data_path, data_file)
    print('raw data: %s' % raw_data_file)
    acq_data = pet.AcquisitionData(raw_data_file)

    # copy the acquisition data into a Python array and display
    dim = acq_data.dimensions()
    print('data dimensions: %d x %d x %d x %d' % dim)
    if show_plot:
        acq_data.show(range(dim[1]//4))
    acq_array = acq_data.as_array()

    if storage[0] == 'm':  # for now, we can only subset acquisition data stored in memory
        nv = dim[2]//2
        views = numpy.arange(nv)
        acq_subset = acq_data.get_subset(views)
        dim_subset = acq_subset.dimensions()
        print('subset dimensions: %d x %d x %d x %d' % dim_subset)
        if show_plot:
            acq_subset.show(range(dim[1]//4), title='Sinograms of a subset of views')

    # rebin the acquisition data
    new_acq_data = acq_data.rebin(3)
    rdim = new_acq_data.dimensions()
    print('rebinned data dimensions: %d x %d x %d x %d' % rdim)
    if show_plot:
        new_acq_data.show(range(rdim[1]//3), title = 'Rebinned acquisition data')

    # clone the acquisition data
    new_acq_data = acq_data.clone()
    if show_plot:
        # display the cloned data
        new_acq_data.show(range(dim[1]//4), title = 'Cloned acquisition data')

    print('norm of acq_data.as_array(): %f' % numpy.linalg.norm(acq_array))
    s = acq_data.norm()
    t = acq_data.dot(acq_data)
    print('acq_data.norm(): %f' % s)
    print('sqrt(acq_data.dot(acq_data)): %f' % math.sqrt(t))
    diff = new_acq_data - acq_data
    print('norm of acq_data.clone() - acq_data: %f' % diff.norm())
    acq_factor = acq_data.get_uniform_copy(0.1)
    new_acq_data = acq_data / acq_factor
    print('norm of acq_data*10: %f' % new_acq_data.norm())
    acq_data_max = new_acq_data.maximum(acq_data)
    print('norm of max(acq_data, acq_data*10): %f' % acq_data_max.norm())
    acq_data_min = new_acq_data.minimum(acq_data)
    print('norm of min(acq_data, acq_data*10): %f' % acq_data_min.norm())
    acq_copy = acq_data.get_uniform_copy(1.0)
    acq_copy *= acq_data
    diff = acq_copy - acq_data
    print('norm of acq_copy - acq_data: %f' % diff.norm())
    diff = -acq_copy.fill(acq_data) + acq_data
    print('norm of -acq_copy.fill(acq_data) + acq_data: %f' % diff.norm())
    print('TOF mashing factor %d' % acq_data.get_tof_mash_factor())

    if show_plot:
        # display the scaled data
        new_acq_data.show(range(dim[1]//4), title = 'Scaled acquisition data')

    image = acq_data.create_uniform_image(10.0)
    image_array = image.as_array()
    print('image dimensions: %d x %d x %d' % image_array.shape)
    s = image.norm()
    t = image.dot(image)
    print('norm of image.as_array(): %f' % numpy.linalg.norm(image_array))
    print('image.norm(): %f' % s)
    print('sqrt(image.dot(image)): %f' % math.sqrt(t))
    image_factor = image.get_uniform_copy(0.1)
    new_image = image / image_factor
    print('norm of image*10: %f' % new_image.norm())
    image_max = new_image.maximum(image)
    print('norm of max(image, image*10): %f' % image_max.norm())
    image_min = new_image.minimum(image)
    print('norm of min(image, image*10): %f' % image_min.norm())
    diff = image.clone() - image
    print('norm of image.clone() - image: %f' % diff.norm())
    image_copy = image.get_uniform_copy()
    image_copy *= image
    diff = image_copy - image
    print('norm of image_copy - image: %f' % diff.norm())
    diff = -image_copy.fill(image) + image
    print('norm of -image_copy.fill(image) + image: %f' % diff.norm())

    print('image voxel sizes:')
    print(image.voxel_sizes())
    print('image transform matrix:')
    tmx = image.transf_matrix()
    print(tmx)

    if scheme != storage:
        pet.AcquisitionData.set_storage_scheme(scheme)
try:
    main()
    print('\n=== done with %s' % __file__)

except error as err:
    print('%s' % err.value)

