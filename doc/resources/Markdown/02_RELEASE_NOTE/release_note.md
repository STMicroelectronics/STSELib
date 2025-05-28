# Release Note

## v1.1.0 (Latest)
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