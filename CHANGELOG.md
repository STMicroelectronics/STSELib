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
- **API change** — all platform-level SecureElement initialization functions now require an additional `void *pArg` parameter ([51aa3f8](https://github.com/STMicroelectronics/STSELib/commit/51aa3f8114d33adfe38da6fd261e4b9a71a1fa9b))
- **Platform AES API change** — all platform AES cryptographic functions now reference keys by secure storage index (`PLAT_UI32 key_idx`) instead of raw key pointer and length (`PLAT_UI8 *pKey, PLAT_UI16 key_length`). Affected functions: `stse_platform_aes_cmac_init`, `stse_platform_aes_cmac_compute`, `stse_platform_aes_cmac_verify`, `stse_platform_aes_cbc_enc`, `stse_platform_aes_cbc_dec`, `stse_platform_aes_ecb_enc`
- **Renamed** `stsafea_open_host_session` to `stsafea_open_host_session_from_idx` — signature updated to accept key indices (`PLAT_UI32 host_MAC_key_idx`, `PLAT_UI32 host_cypher_key_idx`) instead of raw key pointers

### Added

- `stse_platform_store_aes_key()` — new platform function to store an AES key into platform secure storage and retrieve its index
- `stse_platform_delete_aes_key()` — new platform function to delete an AES key from platform secure storage by index

### Changed

### Deprecated

### Removed
- `stsafea_open_host_session` function. Now replaced by `stsafea_open_host_session_from_idx` which accepts key indices instead of raw key pointers. Note that you must first store the AES keys in platform secure storage using `stse_platform_store_aes_key()` to obtain the required indices before opening a host session.

### Fixed

### Security
- AES keys are now securely stored in platform secure storage and referenced by index, eliminating the need to handle raw key material in application memory and enhancing overall security.

---

## [V1.1.7 — March 4, 2026](https://github.com/STMicroelectronics/STSELib/releases/tag/v1.1.7)
