# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

> **Note:** This changelog starts from version v1.2.0. For prior releases, refer to the
> [GitHub Releases page](https://github.com/STMicroelectronics/STSELib/releases).

---

## [Unreleased](https://github.com/STMicroelectronics/STSELib/compare/v1.1.7...HEAD) - v1.2.0

### Breaking Changes

- **Renamed** `stse_Handler_t` to `stse_Handle_t` — all references to the handle type must be updated in application code ([31fb0fa](https://github.com/STMicroelectronics/STSELib/commit/31fb0fa9865828d82479e8c40fa3499c68b33fe9))
- **API renamed** — all STSE host keys management functions must be updated in application code
  -  `stse_host_key_provisioning` to `stse_host_secure_channel_keys_provisioning`
  -  `stse_host_key_provisioning_wrapped` to `stse_host_secure_channel_keys_provisioning_wrapped`
  -  `stse_host_key_provisioning_wrapped_authenticated` to `stse_host_secure_channel_keys_provisioning_wrapped_authenticated`
  -  `stse_establish_host_key` to `stse_establish_host_secure_channel_keys`
  -  `stse_establish_host_key_authenticated` to `stse_establish_host_secure_channel_keys_authenticated`
- **API change** — all platform-level SecureElement initialization functions now require an additional `void *pArg` parameter ([51aa3f8](https://github.com/STMicroelectronics/STSELib/commit/51aa3f8114d33adfe38da6fd261e4b9a71a1fa9b))
- **Platform AES API change** — all platform AES cryptographic functions now reference keys by secure storage index (`PLAT_UI32 key_idx`) instead of raw key pointer and length (`PLAT_UI8 *pKey, PLAT_UI16 key_length`). Affected functions: `stse_platform_aes_cmac_init`, `stse_platform_aes_cmac_compute`, `stse_platform_aes_cmac_verify`, `stse_platform_aes_cbc_enc`, `stse_platform_aes_cbc_dec`, `stse_platform_aes_ecb_enc`
- **Renamed** `stsafea_open_host_session` to `stsafea_open_host_session_from_idx` — signature updated to accept key indices (`PLAT_UI32 host_MAC_key_idx`, `PLAT_UI32 host_cypher_key_idx`) instead of raw key pointers
- **printf() calls replaced** with `stse_platform_printf()` in all library code to abstract away standard I/O and allow platform-specific implementations for logging and output
- **Platform CRC API change** — introduces a context (`stse_crc16_context_t *`) parameter to make the CRC PAL APIs multi-context-safe. Affected functions: `stse_platform_Crc16_Accumulate` (PR [#90](https://github.com/STMicroelectronics/STSELib/pull/90))

### Added

- `stse_platform_store_aes_key()` — new platform function to store an AES key into platform secure storage and retrieve its index
- `stse_platform_delete_aes_key()` — new platform function to delete an AES key from platform secure storage by index
- `stse_platform_printf()` — new platform function for formatted output, replacing all direct calls to `printf()` in the library with this abstraction to allow platform-specific implementations and avoid direct use of standard I/O in library code
- `stse_platform_Crc16_ContextInit()` — new platform function to initialize a CRC16 context
- `stse_platform_Crc16_ContextRelease()` — new platform function to release a CRC16 context

### Changed
- `stse_platform_Crc16_Accumulate()` — Now it takes `stse_crc16_context_t *crc16_context` as param

### Deprecated

### Removed
- `stsafea_open_host_session` function. Now replaced by `stsafea_open_host_session_from_idx` which accepts key indices instead of raw key pointers. Note that you must first store the AES keys in platform secure storage using `stse_platform_store_aes_key()` to obtain the required indices before opening a host session.
- `stsafea_host_key_type_t` enum. Usage replaced by existing `stse_aes_key_type_t` enum.
- `stsafea_aes_128_host_keys_t` type.
- `stsafea_aes_256_host_keys_t` type.
- `stsafea_host_keys_t` type.
- `stse_platform_Crc16_Calculate()` — Now replaced by `stse_platform_Crc16_ContextInit()` which doesn't calculate a CRC16 but initialize the CRC context which will be used to calculate the CRC via `stse_platform_Crc16_Accumulate()`.

### Fixed

### Security
- AES keys are now securely stored in platform secure storage and referenced by index, eliminating the need to handle raw key material in application memory and enhancing overall security.

---

## [V1.1.7 — March 4, 2026](https://github.com/STMicroelectronics/STSELib/releases/tag/v1.1.7)
