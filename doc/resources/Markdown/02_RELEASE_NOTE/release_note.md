# Release Note

## v1.0.3 (Latest)
- [api] Add stse_data_storage_get_data_partition_record API
- [api] Fix stse_compute_hash bug leading to STSE_COMMAND_NOT_AUTHORIZED
- [services & api] Add Host key establishment V2
- [services & api] Reword message_is_hashed argument from stse_ecc_verify_signature
- [services] update echo for host session usage
- [services] Update command Generate ECDHE key pair
- [services] Fix error code in case of handler not initialized
- [core] Fix stse_frame_receive
- [core] Add dedicated frame constant definitions & error codes
- [core] Update stse_frame_debug_print function

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