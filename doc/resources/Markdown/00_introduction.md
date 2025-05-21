# Introduction

<p align="center">
  <img src="../Pictures/STSELib.png" />
</p>

The STSELib middleware offers a comprehensive suite of high-level Application Programming Interface (API) functions for embedded system developers. This middleware abstracts the construction and sequencing of commands necessary to ensure the protection of peripherals, accessories, and consumables using secure authentication devices from STMicroelectronics.

Thanks to its modular architecture, this middleware enables seamless integration of one or multiple secure authentication devices from STMicroelectronics within various host MCU/MPU ecosystems. The STSELib middleware consists of three software modules, as depicted in the figure below.

![STSELib_architecture](../Pictures/STSELib_arch.png)

Each layer provides a distinct level of system abstraction for the embedded system developer:

- **Application Programming Interface (API) Layer**:
  This software layer serves as the entry point for the system application. It offers a set of high-level functions that facilitate interaction with STMicroelectronics Secure Elements.

- **Service Layer**:
  This layer provides a set of product services that format all commands supported by the targeted secure element and report responses to the higher layers (API/Application). Advanced users can directly utilize this layer from the application.

- **Core Layer**:
  This layer contains generic definitions for ST Secure Elements and functions for communicating with the target secure element.

For more details on each layer, please refer to the "Modules Documentation" section of the documentation.


\subpage stse_certificate

\subpage stse_api

\subpage stse_services

\subpage stse_core
