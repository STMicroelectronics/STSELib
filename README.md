# STSELib

![STSELib](doc/resources/Pictures/STSELib.png)

The STSELib middleware provides a complete set of high-level Application Programming Interface functions to the embedded system developer. This Middleware abstract the build and the sequencing of the commands required to ensure device , accessories and consumable brand protection using STMicroelectronics STSAFE-A secure element family.

This middleware allows a seamless integration of one or multiple STSAFE-A in various host MCU/MPU ecosystem.

The STSELib middleware is composed of three software modules as illustrated in the figure below. Each layer provides a different level of system abstraction to the embedded system developer.

![STSELib](doc/resources/Pictures/STSELib_arch.png)

<b>- Application Programming Interface (API) layer</b>
This software layer is the entry point for the system application. It provides a set of high level functions allowing interaction with STMicroelectronics Secure Elements.

<b>- Service layer</b>
Provides a set of product services that format all commands supported by the targeted secure element and reports response to higher layers API/Application . This layer can be used directly from Application (for advanced user).

<b>- Core layer</b>
Contains generic definition for ST Secure Element and functions for communicating with target secure element.

## Integration

### 1. Add STSELib as a Git submodule

From your project root:

```bash
git submodule add https://github.com/STMicroelectronics/STSELib.git lib/stselib
git submodule update --init --recursive
```

> [!NOTE]
>
> Don’t forget to add lib/stselib to your CMakeLists.txt include paths.

### 2. Base configuration

Two headers files are mantatories, [`stse_conf.h`](doc/resources/Markdown/03_LIBRARY_CONFIGURATION/03_LIBRARY_CONFIGURATION.md) and [`stse_platform_generic.h`](doc/resources/Markdown/04_PORTING_GUIDE/PAL_files/stse_platform_generic.h.md).

### 3. Optional files

For platform-specific integrations and STSafe use-cases, you may implement additional headers. Detailed specifications are available in the HTML documentation included in the release ZIP.

Sample is available on Grom-'s github : [STSAFE-A_Accessory_authentication](https://github.com/Grom-/STSAFE-A_Accessory_authentication). The platform files are located in the `Platforms/NUCLEO-L452RE/STSELib` directory.
