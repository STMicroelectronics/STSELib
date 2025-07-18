# Release Note

## v1.1.2 (Latest)
[api] fix issue when compiling with only STSE_CONF_STSAFE_L_SUPPORT directive
[api][services/stsafea] Fix AES CCM encrypt counter management leading to R-MAC error
[services/stsafea] fix frame encrypt/decrypt and MAC issues
[services/stsafea] Update generate ecc key pair & establish shared secret
[services/stsafea] Fix frame length if received frame is lower than expected
[services/stsafea] fix CURVE25519 support in stsafea_ecc_establish_shared_secret service
[services/stsafea] Update MAC counter increment
[services/stsafea] Update frame transmit return code
[services/stsafea] Fix extended header management to be into first frame element
[services/stsafel] Update frame management to avoid issue in case of response header != 0
[services/stsafel] Fix frame transfer when STSE_FRAME_DEBUG_LOG directive is activated
[doc] Add reference to STSAFE-L_echo in README.md
[doc] Add reference to STSAFE-A_echo example in ReadMe.md

## v1.1.1 
- [all] Format code using Pre-commit and clang formater
- [stsafea] fix compilation issue when STSE_CONF_USE_HOST_SESSION is disabled
- [api] [core] [services] Allocate personalization informations to device handler
- [api] [core] [services] Add device (STSAFE-A/L) & protocol (I2C/ST1WIRE) preprocessor check
- [doc] Update doxyfile for doxygen 1.14.0 & fix html display issues

## v1.1.0
- [all] Library performance and footprint optimization based on stse_conf.h
- [certificate parser] Add ED25519 support
- [api] Add STSAFEL support to STSE ECC APIs
- [api] Add STSAFEL support to STSE Data storage APIs
- [api] Add STSAFEL support to STSE Reset/Low Power/Echo APIs
- [stsafel] Add STSAFE-L010 ECC services
- [stsafel] Add STSAFE-L010 Device info services
- [stsafel] Add STSAFE-L010 Data partition services
- [stsafel] Add STSAFE-L010 Reset/Low Power/Echo services
- [stsafel] Create STSAFE-L010 device/commands/timings
- [core] Add STSE_DEVICE_STSAFEx_FAMILY_INDEX defines to get family offset in stse_device_t
- [core] Rework frame/platform to handle STSAFE-A/L frames formats
- [doc] Add new Configuration and Porting guidelines sections
- [services] Add authenticated host key establishment
- [services] Fix CCM encrypt error

## v1.0.3
- [core] Fix frame debug log usage
- [services] Fix echo usage into host session
- [services & api] Add host key establishment v2
- [api] Fix hash computation

## v1.0.2
- [doc] Add HTML documentation
- [api] Add AES-GCM & AES-CCM* by chunks api
- [api] Add symmetric key slot provisioning api
- [stsafea] Add AES-GCM & AES-CCM* by chunck
- [stsafea] Add public key slot management
- [stsafea] Add entity authentication
- [stsafea] Update A120 timings
- [core] Simplify I2C platform
- [core] Add SE return status when debug log activated
- [core] Fix command timing table
- [core] Fix stse_perso_info_t::ext_cmd_AC_status size

## v1.0.1
- [all] code cleanup for MISRA C compliancy
- [api] fix stse_device_unlock
- [api] update data storage's api name to reflect zone's type (data_zone or counter_zone)
- [stsafea] Fix STSAFE-A120 Encryption error 0x0201 when frame is near to 256 bytes
- [stsafea] fix and rename data zone services
- [stsafea] add clear session
- [stsafea] Fix query generic public key slot info
- [stsafea] Fix some issues with CMAC authenticated commands
- [core] Change STSE handler initialization
