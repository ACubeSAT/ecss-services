# ECSS-E-ST-70-41C Services

In this repository you can find the implementation of the ECSS services, based
on
the [ECSS-E-ST-70-41C](https://ecss.nl/standard/ecss-e-st-70-41c-space-engineering-telemetry-and-telecommand-packet-utilization-15-april-2016/)
standard.

## Documentation

You can read the complete documentation of this project at https://acubesat.gitlab.io/obc/ecss-services/docs/.
This includes installation instructions, a usage guide, and detailed descriptions of all public functions and
definitions.

## Directories

- **ci**: GitLab CI configuration files
- **docs**: Source code usage and development documentation
- **inc**: All headers and libraries used in the code
- **lib**: External libraries used in the code
- **src**: All source files are included here
- **test**: Unit test implementation

## Implementation status

| Service Type | Service Name                   | Implementation   |
|--------------|--------------------------------|------------------|
| ST[01]       | Request Verification           | Full             |
| ST[03]       | Housekeeping                   | Partial          |
| ST[04]       | Parameter Statistics Reporting | Partial          |
| ST[05]       | Event Reporting                | Partial          |
| ST[06]       | Memory Management              | Partial          |
| ST[08]       | Function Management            | Full             |
| ST[11]       | Time-based Scheduling          | Partial          |
| ST[12]       | On-board Monitoring            | Work in progress |
| ST[13]       | Large Packer Transfer          | Partial          |
| ST[15]       | On-board Storage and Retrieval | Work in progress |
| ST[17]       | Test                           | Partial          |
| ST[19]       | Event-action                   | Partial          |
| ST[20]       | Parameter Management           | Partial          |
| ST[23]       | File Management                | Work in progress |

## Build

If you're using CLion, you need to add in CMake options (File -> Settings -> Build, Execution, Deployment -> CMake ->
CMake Options) this `-DCMAKE_TOOLCHAIN_FILE=cmake-build-debug/build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release`.

If you just cmake from cli, just add the same flags in your command.

### Conan

To build, you first have run `conan profile detect --force`, which generates default profile detecting GCC. Then run
`conan install . --output-folder=cmake-build-debug --build=missing`.
If you're using CLion and don't see `cmake-build-debug`, you have to `Reload CMake project` to have it generated.
After you've run `conan install...` you can `Reload CMake project` and build as per usual.

<details>
<summary>Getting conan</summary>

You can install [conan](https://conan.io/) following the instructions from
[here](https://docs.conan.io/en/latest/installation.html). Just to be safe, you can follow the standard procedure from
[here](https://docs.conan.io/en/latest/getting_started.html):

</details>