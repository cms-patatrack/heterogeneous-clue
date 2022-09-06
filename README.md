# Heterogeneous-CLUE: SYCL implementation

## Table of contents

* [Introduction](#introduction)
* [Building](#building)
  * [`sycl` and `sycltest`](#sycl-and-sycltest)
  
* [Runtime parameters](#runtime-parameters)
* [RUnning](#running)
* [Details](#details)
  * [SYCL device selection](#sycl-device-selection)
  * [Testing](#testing)
* [Code structure](#code-structure)
* [Build system](#build-system)

## Introduction
The purpose of this package is to explore various implementations of [CLUE](https://gitlab.cern.ch/kalos/clue) clustering algorithm, and their performance, when integrated in a framwork extremely similar to the one found in [Patatrack](https://patatrack.web.cern.ch/patatrack/wiki/) pixel
tracking application ([Pixeltrack](https://github.com/cms-patatrack/pixeltrack-standalone)). 

The application is designed to require minimal dependencies on the system. All programs require
* GNU Make, `curl`, `md5sum`, `tar`

SYCL compiler and libraries get automatically sourced from cvmfs, so there is no need for a local install when running on a CERN machine.

It is suggested to source a recent compiler from `cvmfs`, i.e.
```
source /cvmfs/sft.cern.ch/lcg/contrib/gcc/11.2.0/x86_64-centos7/setup.sh
```
in order to work with other implementations (i.e. alpaka).

## Building
### `sycl` and `sycltest`
If running on a machine with Intel GPU(s), building the project only requires to run:
```bash
make environment
source env.sh
make -j `nproc` sycl
```
 This will compile the project and produce an executable for Intel CPUs and specifically the GPU found on Olice-05.

When using a CUDA machine, since OneAPI doesn't officially support the CUDA backend yet, compilation falls back on the open source llvm complier, which requires to source devtoolset 9 first, then build the project with a slightly different command:
```bash
source scl_source enable devtoolset-9
USE_SYCL_PATATRACK=1 make -j `nproc` sycl
source env.sh
```
This will produce an executable tailored for any CUDA GPU. Note that the environment is different so the build might clash if executed in the same folder in afs.

## Runtime parameters
CLUE needs three parameters to run: `dc`, `rhoc` and `outlierDeltaFactor`. 

_dc_ is the critical distance used to compute the local density.
_rhoc_ is the minimum local density for a point to be promoted as a Seed.
_outlierDeltaFactor_ is  a multiplicative constant to be applied to `dc`.

A default set of these parameters, together with `produceOutput` (boolean which specifies whether to produce a csv output file or not) is found as a configuration file (.csv) in [`config`](config). Any other configuration can be passed to the program through a similar file which should follow the format:
```bash
[dc],[rhoc],[outlierDeltaFactor],[produceOutput]
```

## Running
The procedure to run the algorithm is the same on both CUDA and Intel backends.
Executing the program is as simple as running:
```bash
./sycl
```

Other than the configuration file which we've already touched, many other options are available at runtime:

```
./sycl: [--device DEV] [--numberOfThreads NT] [--numberOfStreams NS] [--maxEvents ME] [--inputFile PATH] [--configFile PATH] [--transfer] [--validation] [--empty]

Options
 --device            Specifies the device which should run the code
 --numberOfThreads   Number of threads to use (default 1, use 0 to use all CPU cores)
 --numberOfStreams   Number of concurrent events (default 0 = numberOfThreads)
 --maxEvents         Number of events to process (default -1 for all events in the input file)
 --runForMinutes     Continue processing the set of 1000 events until this many minutes have passed (default -1 for disabled; conflicts with --maxEvents)
 --inputFile         Path to the input file to cluster with CLUE (default is set to 'data/input/toyDetector_1k.csv')
 --configFile        Path to the config file with the parameters (dc, rhoc, outlierDeltaFactor, produceOutput) to run CLUE (default 'config/test_without_output.csv' in the directory of the exectuable)
 --transfer          Transfer results from GPU to CPU (default is to leave them on GPU)
 --validation        Run (rudimentary) validation at the end (implies --transfer)
 --empty             Ignore all producers (for testing only)
```

## Details

### SYCL device selection
The option to select the device is quite flexible. In the SYCL implementation, ```--device``` can accept either a class of devices (cpu, gpu or acc) or a specific device. If one class is selected and the program is executed with more than one stream, the load will be automatically divided among all the available devices at runtime. 

To select a particular device, specify its name as seen in the list obtained from the command ```sycl-ls```. For example, if the output from ```sycl-ls``` is as follows:
```
[opencl:acc:0] Intel(R) FPGA Emulation Platform for OpenCL(TM), Intel(R) FPGA Emulation Device 1.2 [2022.13.3.0.16_160000]
[opencl:cpu:1] Intel(R) OpenCL, Intel(R) Xeon(R) Gold 6336Y CPU @ 2.40GHz 3.0 [2022.13.3.0.16_160000]
[opencl:acc:2] Intel(R) FPGA Emulation Platform for OpenCL(TM), Intel(R) FPGA Emulation Device 1.2 [2021.13.11.0.23_160000]
[opencl:cpu:3] Intel(R) OpenCL, Intel(R) Xeon(R) Gold 6336Y CPU @ 2.40GHz 3.0 [2021.13.11.0.23_160000]
[opencl:gpu:4] Intel(R) OpenCL HD Graphics, Intel(R) Graphics [0x020a] 3.0 [22.17.023061]
[ext_oneapi_level_zero:gpu:0] Intel(R) Level-Zero, Intel(R) Graphics [0x020a] 1.3 [1.3.23061]
[host:host:0] SYCL host platform, SYCL host device 1.2 [1.2]
```
using the option ```--device ext_oneapi_level_zero:gpu:0``` will execute the program on Intel(R) Graphics [0x020a] using the Level-Zero backend.

### Testing 
To validate the algorithm, results get transferred back to host memory and compared against the correct file taken from a reference set. This option is available for toyDetectors from 1k to 10k with the default parameters. Any other file will not be validated. When using parameters different from the ones in the default configuration file, only input variables will be validated.

### Code structure

The project is organized very similarly to [Pixeltrack-standalone](https://github.com/cms-patatrack/pixeltrack-standalone). As such, it is split into several programs, one (or more) for each
test case. Each test case has its own directory under [`src`](src)
directory. A test case contains the full application: framework, data
formats, device tooling, plugins for the algorithmic modules ran
by the framework, and the executable.

Each test program is structured as follows within `src/<program name>`
(examples point to [`sycl`](src/sycl)
* [`Makefile`](src/sycl/Makefile) that defines the actual build rules for the program
* [`Makefile.deps`](src/sycl/Makefile.deps) that declares the external dependencies of the program, and the dependencies between shared objects within the program
* [`bin/`](src/sycl/bin/) directory that contains all the framework code for the executable binary. These files should not need to be modified, except [`main.cc`](src/sycl/bin/main.cc) for changin the set of modules to run, and possibly more command line options
* `plugin-<PluginName>/` directories contain the source code for plugins. The `<PluginName>` part specifies the name of the plugin, and the resulting shared object file is `plugin<PluginName>.so`. Note that no other library or plugin may depend on a plugin (either at link time or even thourgh the `#include` of a header). The plugins may only be loaded through the names of the modules by the [`PluginManager`](src/sycl/bin/PluginManager.h).
* `<LibraryName>/`: the remaining directories are for libraries. The `<LibraryName>` specifies the name of the library, and the resulting shared object file is `lib<LibraryName>.so`. Other libraries or plugins may depend on a library, in which case the dependence must be declared in [`Makefile.deps`](src/sycl/Makefile.deps).
  * [`CondFormats/`](src/sycl/CondFormats/):
  * [`SYCLDataFormats/`](src/sycl/CUDADataFormats/): SYCL-specific data structures that can be passed from one module to another via the `edm::Event`. A given portability technology likely needs its own data format directory, the `SYCLDataFormats` can be used as an example.
  * [`SYCLCore/`](src/sycl/CUDACore/): Various tools for SYCL. A given portability technology likely needs its own tool directory, the `SYCLCore` can be used as an example.
  * [`DataFormats/`](src/sycl/DataFormats/): mainly CPU-side data structures that can be passed from one module to another via the `edm::Event`. Some of these are produced by the [`edm::Source`](src/sycl/bin/Source.h) by reading the binary dumps. These files should not need to be modified. New classes may be added, but they should be independent of the portability technology.
  * [`Framework/`](src/sycl/Framework/): crude approximation of the CMSSW framework. Utilizes TBB tasks to orchestrate the processing of the events by the modules. These files should not need to be modified.
  * [`Geometry/`](src/sycl/Geometry/): geometry information, essentially handful of compile-time constants. May be modified.

For more detailed description of the application structure (mostly plugins) see
[CodeStructure.md](doc/CodeStructure.md)

### Build system

The build system is based on pure GNU Make. There are two levels of
Makefiles. The [top-level Makefile](Makefile) handles the building of
the entire project: it defines general build flags, paths to external
dependencies in the system, recipes to download and build the
externals, and targets for the test programs.

For more information see [BuildSystem.md](doc/BuildSystem.md).

The code is formatted with `clang-format` version 10.