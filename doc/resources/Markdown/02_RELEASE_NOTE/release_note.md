# Release Note

## v1.0.2 (Latest)
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