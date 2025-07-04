"""Internal module for passing/getting Python parameters to/from C/C++."""

# SyneRBI Synergistic Image Reconstruction Framework (SIRF)
# Copyright 2015 - 2021 Rutherford Appleton Laboratory STFC
# Copyright 2015 - 2022 University College London
# Copyright 2019 University of Hull
#
# This is software developed for the Collaborative Computational
# Project in Synergistic Reconstruction for Biomedical Imaging
# (formerly CCP PETMR)
# (http://www.ccpsynerbi.ac.uk/).
#
# Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#       http://www.apache.org/licenses/LICENSE-2.0
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

import inspect

import sirf.select_module as select_module
import sirf.pyiutilities as pyiutil
from sirf.Utilities import check_status

#from sirf.pystir import setParameter, parameter
from sirf.pystir import cSTIR_setParameter as setParameter
from sirf.pystir import cSTIR_parameter as parameter


def set_parameter(hs, group, par, hv, stack = None):
    if stack is None:
        stack = inspect.stack()[1]
    h = setParameter(hs, group, par, hv)
    check_status(h, stack)
    pyiutil.deleteDataHandle(h)


def set_char_par(handle, group, par, value):
    h = pyiutil.charDataHandle(value)
    set_parameter(handle, group, par, h, inspect.stack()[1])
    pyiutil.deleteDataHandle(h)


def set_int_par(handle, group, par, value):
    h = pyiutil.intDataHandle(int(value))
    set_parameter(handle, group, par, h, inspect.stack()[1])
    pyiutil.deleteDataHandle(h)


def set_bool_par(handle, group, par, value):
    h = pyiutil.boolDataHandle(bool(value))
    set_parameter(handle, group, par, h, inspect.stack()[1])
    pyiutil.deleteDataHandle(h)


def set_float_par(handle, group, par, value):
    h = pyiutil.floatDataHandle(float(value))
    set_parameter(handle, group, par, h, inspect.stack()[1])
    pyiutil.deleteDataHandle(h)


def set_double_par(handle, group, par, value):
    h = pyiutil.doubleDataHandle(float(value))
    set_parameter(handle, group, par, h, inspect.stack()[1])
    pyiutil.deleteDataHandle(h)


def bool_par(handle, group, par):
    h = parameter(handle, group, par)
    check_status(h, inspect.stack()[1])
    value = pyiutil.boolDataFromHandle(h)
    pyiutil.deleteDataHandle(h)
    return value


def char_par(handle, group, par):
    h = parameter(handle, group, par)
    check_status(h)
    value = pyiutil.charDataFromHandle(h)
    pyiutil.deleteDataHandle(h)
    return value


def int_par(handle, group, par):
    h = parameter(handle, group, par)
    check_status(h, inspect.stack()[1])
    value = pyiutil.intDataFromHandle(h)
    pyiutil.deleteDataHandle(h)
    return value


def size_t_par(handle, group, par):
    h = parameter(handle, group, par)
    check_status(h, inspect.stack()[1])
    value = pyiutil.size_tDataFromHandle(h)
    pyiutil.deleteDataHandle(h)
    return value


def int_pars(handle, group, par, n):
    h = parameter(handle, group, par)
    check_status(h)
    value = ()
    for i in range(n):
        value += (pyiutil.intDataItemFromHandle(h, i),)
    pyiutil.deleteDataHandle(h)
    return value


def uint16_pars(handle, group, par, n):
    h = parameter(handle, group, par)
    check_status(h)
    value = ()
    for i in range(n):
        value += (pyiutil.uint16DataItemFromHandle(h, i),)
    pyiutil.deleteDataHandle(h)
    return value


def uint32_pars(handle, group, par, n):
    h = parameter(handle, group, par)
    check_status(h)
    value = ()
    for i in range(n):
        value += (pyiutil.uint32DataItemFromHandle(h, i),)
    pyiutil.deleteDataHandle(h)
    return value


def uint64_pars(handle, group, par, n):
    h = parameter(handle, group, par)
    check_status(h)
    value = ()
    for i in range(n):
        value += (pyiutil.uint64DataItemFromHandle(h, i),)
    pyiutil.deleteDataHandle(h)
    return value


def float_par(handle, group, par):
    h = parameter(handle, group, par)
    check_status(h)
    v = pyiutil.floatDataFromHandle(h)
    pyiutil.deleteDataHandle(h)
    return v


def float_pars(handle, group, par, n):
    h = parameter(handle, group, par)
    check_status(h)
    value = ()
    for i in range(n):
        value += (pyiutil.floatDataItemFromHandle(h, i),)
    pyiutil.deleteDataHandle(h)
    return value


def double_par(handle, group, par):
    h = parameter(handle, group, par)
    check_status(h)
    v = pyiutil.doubleDataFromHandle(h)
    pyiutil.deleteDataHandle(h)
    return v


def parameter_handle(hs, group, par):
    handle = parameter(hs, group, par)
    check_status(handle, inspect.stack()[1])
    return handle
